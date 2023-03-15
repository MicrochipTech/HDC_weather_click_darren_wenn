//driver_xxx_definitions.h : contains data structures and definitions that will be needed by initialization.c file
    //This file along with the driver_xxx.h file gets included in the definitions.h file which in turn is included in the initialization.c file
//driver_xxx_local.h : contains data structures and definitions that are local to the driver
    //-->includes driver_xxx_definitions file as the driver_xxx_local may use some of the definitions defined in this file
//driver_xxx.h : contains data structues, definitions and function prototypes. This file will be included by application using this driver.
   //-->includes driver_xxx_definitions file as the driver_xxx_local may use some of the definitions defined in this file
//driver_xxx.c : contains driver implementation and public APIs.
  //-->includes driver_xxx.h file

//------------------------------------------------------------------------------------------------  

#include "drv_temp_sensor.h"
#include <string.h>

#define DRV_TEMP_SENSOR_INSTANCES_NUMBER            2
#define APP_TEMP_TEMPERATURE_REG_ADDR               0x00

DRV_TEMP_SENSOR_OBJ gDrvTempSensorObj[DRV_TEMP_SENSOR_INSTANCES_NUMBER];

static void DRV_TEMP_SENSOR_PLIBEventHandler(uintptr_t contextHandle)
{
    DRV_TEMP_SENSOR_OBJ* dObj = (DRV_TEMP_SENSOR_OBJ *)contextHandle;
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = NULL;
    DRV_TEMP_SENSOR_ERROR error;
    
    if (dObj == NULL)
    {
        return;
    }
    
    clientObj = dObj->activeClient;
    
    error = dObj->plibInterface->errorGet();
    
    if (dObj->wrInProgress == true && error == DRV_TEMP_SENSOR_ERROR_NONE)
    {    
        dObj->wrInProgress = false;
        dObj->eepChkBusyStatus = true;
        
        // Initiate checking of EEPROM's write busy status
        dObj->wrBuffer[0] = 0xFF;
        
        dObj->plibInterface->write(clientObj->configParams.eepromAddr, (void *)dObj->wrBuffer, 1);
    }
    else if (dObj->eepChkBusyStatus == true)
    {
        if (error == DRV_TEMP_SENSOR_ERROR_NACK)
        {
            // EEPROM is still busy with the internal write. Keep checking for the EEPROM write busy status.
            dObj->wrBuffer[0] = 0xFF;

            dObj->plibInterface->write(clientObj->configParams.eepromAddr, (void *)dObj->wrBuffer, 1);
        }
        else
        {
            // EEPROM internal write is completed.
            dObj->eepChkBusyStatus = false;
        }
    }
    
    if (dObj->wrInProgress == false && dObj->eepChkBusyStatus == false)
    {
        if(error != DRV_TEMP_SENSOR_ERROR_NONE)
        {
            dObj->event = DRV_TEMP_SENSOR_EVENT_ERROR;
        }
        
        dObj->activeClient = NULL;
        
        if(clientObj->callback != NULL)
        {
            clientObj->callback(dObj->event, clientObj->context);
        }
    }
}

static DRV_TEMP_SENSOR_CLIENT_OBJ* DRV_TEMP_SENSOR_ClientObjGet(const DRV_HANDLE handle)
{
    uint32_t drvIndex = handle >> 8;
    uint32_t clientIndex = handle & 0xFF;
    
    if (drvIndex < DRV_TEMP_SENSOR_INSTANCES_NUMBER)
    {
        if (clientIndex < gDrvTempSensorObj[drvIndex].maxClients)
        {
            if (gDrvTempSensorObj[drvIndex].clientObjPool[clientIndex].inUse == true)
            {
                return &gDrvTempSensorObj[drvIndex].clientObjPool[clientIndex];
            }
        }
    }
    
    return NULL;
}

static int32_t DRV_TEMP_SENSOR_FreeClientGet(DRV_TEMP_SENSOR_OBJ* dObj)
{
    for (int32_t i = 0; i < dObj->maxClients; i++)
    {
        if (dObj->clientObjPool[i].inUse == false)
        {
            dObj->clientObjPool[i].inUse = true;
            return i;
        }
    }
    
    return -1;
}

SYS_MODULE_OBJ DRV_TEMP_SENSOR_Initialize(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT* const init
)
{
    DRV_TEMP_SENSOR_OBJ* dObj     = NULL;
    DRV_TEMP_SENSOR_INIT* tempSensorInit = (DRV_TEMP_SENSOR_INIT*)init;
    
    /* Validate the request */
    if(drvIndex >= DRV_TEMP_SENSOR_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    else
    {
        dObj = &gDrvTempSensorObj[drvIndex];
    }

    /* Is the driver instance already initialized? */
    if(dObj->isInitialized == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }    

    dObj->nClients = 0;
    
    dObj->activeClient = NULL;

    dObj->plibInterface = tempSensorInit->plibInterface;
    
    dObj->clientObjPool = (DRV_TEMP_SENSOR_CLIENT_OBJ*)tempSensorInit->clientObjPool;
    
    dObj->maxClients = tempSensorInit->maxClients;
    
    dObj->plibInterface->callbackRegister(DRV_TEMP_SENSOR_PLIBEventHandler, (uintptr_t)dObj);
    
    dObj->isInitialized = true;
    
    return (SYS_MODULE_OBJ)drvIndex;
}

DRV_HANDLE DRV_TEMP_SENSOR_Open(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    int32_t clientObjIndex;
    DRV_TEMP_SENSOR_OBJ* dObj = NULL;
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = NULL;
    
    /* Validate the request */
    if(drvIndex >= DRV_TEMP_SENSOR_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }
    
    dObj = &gDrvTempSensorObj[drvIndex];
    
    /* Driver must be initialized before it can be opened by a client. Also, it must not already be in use (opened state). */
    if(dObj->isInitialized == false || dObj->nClients >= dObj->maxClients)
    {
        return DRV_HANDLE_INVALID;
    }
    else
    {
        clientObjIndex = DRV_TEMP_SENSOR_FreeClientGet(dObj);
        
        if (clientObjIndex >= 0)
        {
            clientObj = &dObj->clientObjPool[clientObjIndex];
            
            clientObj->drvIndex = drvIndex;

            dObj->nClients += 1;
            
            return ((uint32_t)drvIndex << 8) | (clientObjIndex);
        }
    }
    
    return DRV_HANDLE_INVALID;
}

void DRV_TEMP_SENSOR_Close(const DRV_HANDLE handle)
{
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = DRV_TEMP_SENSOR_ClientObjGet(handle);
    DRV_TEMP_SENSOR_OBJ* dObj = NULL;
    
    if (clientObj != NULL)
    {
        dObj = &gDrvTempSensorObj[clientObj->drvIndex];
        dObj->nClients--;
        clientObj->inUse = false;
    }
}

void DRV_TEMP_SENSOR_TransferEventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_TEMP_SENSOR_APP_CALLBACK callback,
    const uintptr_t context
)
{
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = DRV_TEMP_SENSOR_ClientObjGet(handle);
    
    if (clientObj != NULL)
    {
        clientObj->callback = callback;
        clientObj->context = context;
    }     
}

uint8_t DRV_TEMP_SENSOR_TemperatureGet(const DRV_HANDLE handle, uint16_t* temperatureData)
{
    int16_t temp;
    (void)handle;
    
    // Convert the temperature value read from sensor to readable format (Degree Celsius)
    // For demonstration purpose, temperature value is assumed to be positive.
    // The maximum positive temperature measured by sensor is +125 C
    temp = (*temperatureData << 8) | (*temperatureData >> 8);
    temp = (temp >> 7) * 0.5;
    return (uint8_t)temp;
}

bool DRV_TEMP_SENSOR_TemperatureRead(const DRV_HANDLE handle, uint16_t* temperatureData)
{
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = DRV_TEMP_SENSOR_ClientObjGet(handle);
    DRV_TEMP_SENSOR_OBJ* dObj     = NULL;
    
    if (clientObj == NULL)
    {
        return false;
    }
    
    dObj = &gDrvTempSensorObj[clientObj->drvIndex];
    
    if (dObj->activeClient != NULL)
    {
        return false;
    }
    
    dObj->activeClient = clientObj;
    
    dObj->wrBuffer[0] = APP_TEMP_TEMPERATURE_REG_ADDR;
    
    dObj->plibInterface->transferSetup(&clientObj->configParams.transferParams, 0);
    
    dObj->event = DRV_TEMP_SENSOR_EVENT_TEMP_READ_DONE;
    
    dObj->plibInterface->writeRead(clientObj->configParams.tempSensorAddr, (void*)dObj->wrBuffer, 1, (void *)temperatureData, 2);
    
    return true;
}

bool DRV_TEMP_SENSOR_EEPROMWrite(const DRV_HANDLE handle, uint8_t memAddr, uint8_t* wrBuffer, uint8_t nWrBytes)
{
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = DRV_TEMP_SENSOR_ClientObjGet(handle);
    DRV_TEMP_SENSOR_OBJ* dObj     = NULL;
    
    if (clientObj == NULL)
    {
        return false;
    }
    
    dObj = &gDrvTempSensorObj[clientObj->drvIndex];
    
    if (dObj->activeClient != NULL)
    {
        return false;
    }
    
    dObj->activeClient = clientObj;
    
    dObj->wrBuffer[0] = memAddr;
    
    memcpy(&dObj->wrBuffer[1], wrBuffer, nWrBytes);
    
    dObj->wrInProgress = true;
    
    dObj->plibInterface->transferSetup(&clientObj->configParams.transferParams, 0);
    
    dObj->event = DRV_TEMP_SENSOR_EVENT_EEPROM_WRITE_DONE;
    
    return dObj->plibInterface->write(clientObj->configParams.eepromAddr, (void *)dObj->wrBuffer, nWrBytes + 1);
}

bool DRV_TEMP_SENSOR_EEPROMRead(const DRV_HANDLE handle, uint8_t memAddr, uint8_t* rdBuffer, uint8_t nRdBytes)
{
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = DRV_TEMP_SENSOR_ClientObjGet(handle);
    DRV_TEMP_SENSOR_OBJ* dObj     = NULL;
    
    if (clientObj == NULL)
    {
        return false;
    }
    
    dObj = &gDrvTempSensorObj[clientObj->drvIndex];
    
    if (dObj->activeClient != NULL)
    {
        return false;
    }
    
    dObj->activeClient = clientObj;
    
    dObj->plibInterface->transferSetup(&clientObj->configParams.transferParams, 0);
    
    dObj->wrBuffer[0] = memAddr;
    
    dObj->event = DRV_TEMP_SENSOR_EVENT_EEPROM_READ_DONE;
    
    return dObj->plibInterface->writeRead(clientObj->configParams.eepromAddr, (void *)dObj->wrBuffer, 1, rdBuffer, nRdBytes);    
}

bool DRV_TEMP_SENSOR_TransferSetup(const DRV_HANDLE handle, DRV_TEMP_SENSOR_CONFIG_PARAMS* configParams)
{
    DRV_TEMP_SENSOR_CLIENT_OBJ* clientObj = DRV_TEMP_SENSOR_ClientObjGet(handle);
    
    if (clientObj == NULL)
    {
        return false;
    }  

    clientObj->configParams = *configParams;
    
    return true;
    
}