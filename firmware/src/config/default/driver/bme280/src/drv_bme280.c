/******************************************************************************
  DRV_BME280 Library Interface Implementation

  Company:
    Microchip Technology Inc.

  File Name:
    drv_bme280.c

  Summary:
    BME280 Weather Sensor Driver Library Interface implementation

  Description:
    The BME280 Library provides a interface to access the BME280 Weather Sensor.
    This file implements the BME280 Library interface.
*******************************************************************************/

//DOM-IGNORE-BEGIN
/*******************************************************************************
* Copyright (C) 2018 Microchip Technology Inc. and its subsidiaries.
*
* Subject to your compliance with these terms, you may use Microchip software
* and any derivatives exclusively with Microchip products. It is your
* responsibility to comply with third party license terms applicable to your
* use of third party software (including open source software) that may
* accompany Microchip software.
*
* THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
* EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
* WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
* PARTICULAR PURPOSE.
*
* IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
* INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
* WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
* BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
* FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
* ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
* THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
*******************************************************************************/
//DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Include Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"
#include "driver/bme280/drv_bme280.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global objects
// *****************************************************************************
// *****************************************************************************

/* This is the driver instance object array. */
static DRV_BME280_OBJ gDrvBME280Obj[DRV_BME280_INSTANCES_NUMBER];

// *****************************************************************************
// *****************************************************************************
// Section: DRV_BME280 Driver Local Functions
// *****************************************************************************
// *****************************************************************************

/* Generate the driver handle based upon the driver index and client ID */
static inline uint32_t _DRV_BME280_MAKE_HANDLE(uint8_t drvIndex, uint8_t clientIndex)
{
    return ((drvIndex << 8) | clientIndex);
}

/* parse the read data from the BME280 sensor */
static bool _DRV_BME280_ParseData(DRV_BME280_OBJ* dObj, const uint8_t* reg_data)
{
    uint32_t data_xlsb;
    uint32_t data_lsb;
    uint32_t data_msb;
    
    if ((dObj == NULL) || (reg_data == NULL))
    {
        return false;
    }
    
    /* pressure data */
    data_msb = (uint32_t) reg_data[0] << 12;
    data_lsb = (uint32_t) reg_data[1] << 4;
    data_xlsb = (uint32_t) reg_data[2] >> 4;
    dObj->uncompData.pressure = data_msb | data_lsb | data_xlsb;
    
    /* temperature data */
    data_msb = (uint32_t) reg_data[3] << 12;
    data_lsb = (uint32_t) reg_data[4] << 4;
    data_xlsb = (uint32_t) reg_data[5] >> 4;
    dObj->uncompData.temperature = data_msb | data_lsb | data_xlsb;

    /* humidity */
    data_msb = (uint32_t) reg_data[6] << 8;
    data_lsb = (uint32_t) reg_data[7];
    dObj->uncompData.humidity = data_msb | data_lsb;
    
    return true;
}

/* generate a compensated temperature reading  */
static int32_t _DRV_BME280_Compensate_T(DRV_BME280_UNCOMP_DATA* uncompData, DRV_BME280_COMPENSATION_DATA* calib_data)
{
    int32_t var1;
    int32_t var2;
    int32_t temperature;
    int32_t temperature_min = -4000;
    int32_t temperature_max = 8500;

    var1 = (int32_t)((uncompData->temperature / 8) - ((int32_t)calib_data->dig_T1 * 2));
    var1 = (var1 * ((int32_t)calib_data->dig_T2)) / 2048;
    var2 = (int32_t)((uncompData->temperature / 16) - ((int32_t)calib_data->dig_T1));
    var2 = (((var2 * var2) / 4096) * ((int32_t)calib_data->dig_T3)) / 16384;
    calib_data->t_fine = var1 + var2;
    temperature = (calib_data->t_fine * 5 + 128) / 256;

    if (temperature < temperature_min)
    {
        temperature = temperature_min;
    }
    else if (temperature > temperature_max)
    {
        temperature = temperature_max;
    }

    return temperature;
}

/* generate a compensated pressure reading  */
static uint32_t _DRV_BME280_Compensate_P(DRV_BME280_UNCOMP_DATA* uncompData, DRV_BME280_COMPENSATION_DATA* calib_data)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    uint32_t var5;
    uint32_t pressure;
    uint32_t pressure_min = 30000;
    uint32_t pressure_max = 110000;

    var1 = (((int32_t)calib_data->t_fine) / 2) - (int32_t)64000;
    var2 = (((var1 / 4) * (var1 / 4)) / 2048) * ((int32_t)calib_data->dig_P6);
    var2 = var2 + ((var1 * ((int32_t)calib_data->dig_P5)) * 2);
    var2 = (var2 / 4) + (((int32_t)calib_data->dig_P4) * 65536);
    var3 = (calib_data->dig_P3 * (((var1 / 4) * (var1 / 4)) / 8192)) / 8;
    var4 = (((int32_t)calib_data->dig_P2) * var1) / 2;
    var1 = (var3 + var4) / 262144;
    var1 = (((32768 + var1)) * ((int32_t)calib_data->dig_P1)) / 32768;

    /* avoid exception caused by division by zero */
    if (var1)
    {
        var5 = (uint32_t)((uint32_t)1048576) - uncompData->pressure;
        pressure = ((uint32_t)(var5 - (uint32_t)(var2 / 4096))) * 3125;

        if (pressure < 0x80000000)
        {
            pressure = (pressure << 1) / ((uint32_t)var1);
        }
        else
        {
            pressure = (pressure / (uint32_t)var1) * 2;
        }

        var1 = (((int32_t)calib_data->dig_P9) * ((int32_t)(((pressure / 8) * (pressure / 8)) / 8192))) / 4096;
        var2 = (((int32_t)(pressure / 4)) * ((int32_t)calib_data->dig_P8)) / 8192;
        pressure = (uint32_t)((int32_t)pressure + ((var1 + var2 + calib_data->dig_P7) / 16));

        if (pressure < pressure_min)
        {
            pressure = pressure_min;
        }
        else if (pressure > pressure_max)
        {
            pressure = pressure_max;
        }
    }
    else
    {
        pressure = pressure_min;
    }

    return pressure;
}

/* generate a compensated humidity reading  */
static uint32_t _DRV_BME280_Compensate_H(DRV_BME280_UNCOMP_DATA* uncompData, DRV_BME280_COMPENSATION_DATA* calib_data)
{
    int32_t var1;
    int32_t var2;
    int32_t var3;
    int32_t var4;
    int32_t var5;
    uint32_t humidity;
    uint32_t humidity_max = 102400;

    var1 = calib_data->t_fine - ((int32_t)76800);
    var2 = (int32_t)(uncompData->humidity * 16384);
    var3 = (int32_t)(((int32_t)calib_data->dig_H4) * 1048576);
    var4 = ((int32_t)calib_data->dig_H5) * var1;
    var5 = (((var2 - var3) - var4) + (int32_t)16384) / 32768;
    var2 = (var1 * ((int32_t)calib_data->dig_H6)) / 1024;
    var3 = (var1 * ((int32_t)calib_data->dig_H3)) / 2048;
    var4 = ((var2 * (var3 + (int32_t)32768)) / 1024) + (int32_t)2097152;
    var2 = ((var4 * ((int32_t)calib_data->dig_H2)) + 8192) / 16384;
    var3 = var5 * var2;
    var4 = ((var3 / 32768) * (var3 / 32768)) / 128;
    var5 = var3 - ((var4 * ((int32_t)calib_data->dig_H1)) / 16);
    var5 = (var5 < 0 ? 0 : var5);
    var5 = (var5 > 419430400 ? 419430400 : var5);
    humidity = (uint32_t)(var5 / 4096);

    if (humidity > humidity_max)
    {
        humidity = humidity_max;
    }

    return humidity;
}

/* This function will be called by I2C PLIB when transfer is completed */
static void _DRV_BME280_PLIBEventHandler(uintptr_t context)
{
    DRV_BME280_OBJ* dObj = (DRV_BME280_OBJ*) context;
    DRV_BME280_CLIENT_OBJ* clientObj = NULL;
    DRV_BME280_ERROR error;
    int16_t msb, lsb;
    
    if (dObj == NULL)
    {
        return;
    }
    
    clientObj = dObj->activeClient;
    error = dObj->plibInterface->errorGet(); 
    
    if (error != DRV_BME280_ERROR_NONE)
    {
        dObj->status = SYS_STATUS_READY;
        dObj->activeClient = NULL;
        dObj->taskState = DRV_BME280_TASK_ERROR;
        return;
    }
    
    /* test for initial calibration stage */
    if (dObj->event == DRV_BME280_EVENT_WRITE_DONE)
    {
        /* the write operation completed. The next device state will already
         * have been written to the taskState so allow the transition by
         * setting SYS_STATUS_READY */
        dObj->status = SYS_STATUS_READY;
        dObj->activeClient = NULL;
        return;
    } 
    else if (dObj->event == DRV_BME280_EVENT_READ_DONE)
    {
        switch (dObj->taskState)
        {
            case DRV_BME280_TASK_READ_ID:
                /* read ID completed */
                dObj->deviceID = dObj->readBuffer[0];
                if (dObj->deviceID == DRV_BME280_CHIP_ID)
                {
                    dObj->taskState = DRV_BME280_TASK_READ_CALIBT;
                } else {
                    dObj->taskState = DRV_BME280_TASK_ERROR;
                }
                break;
            case DRV_BME280_TASK_READ_CALIBT:
                /* record the temperature calibration data */
                dObj->calibData.dig_T1 = DRV_BME280_CONCAT_BYTES(dObj->readBuffer[1], dObj->readBuffer[0]);
                dObj->calibData.dig_T2 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[3], dObj->readBuffer[2]);
                dObj->calibData.dig_T3 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[5], dObj->readBuffer[4]);
                /* advance the initialisation to the next state */
                dObj->taskState = DRV_BME280_TASK_READ_CALIBP;
                break;
            case DRV_BME280_TASK_READ_CALIBP:
                /* record pressure calibration data */
                dObj->calibData.dig_P1 = DRV_BME280_CONCAT_BYTES(dObj->readBuffer[1], dObj->readBuffer[0]);
                dObj->calibData.dig_P2 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[3], dObj->readBuffer[2]);
                dObj->calibData.dig_P3 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[5], dObj->readBuffer[4]);
                dObj->calibData.dig_P4 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[7], dObj->readBuffer[6]);
                dObj->calibData.dig_P5 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[9], dObj->readBuffer[8]);
                dObj->calibData.dig_P6 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[11], dObj->readBuffer[10]);
                dObj->calibData.dig_P7 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[13], dObj->readBuffer[12]);
                dObj->calibData.dig_P8 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[15], dObj->readBuffer[14]);
                dObj->calibData.dig_P9 = (int16_t) DRV_BME280_CONCAT_BYTES(dObj->readBuffer[17], dObj->readBuffer[16]);
                /* advance the initialisation to the next state */
                dObj->taskState = DRV_BME280_TASK_READ_CALIBH1;
                break;            
            case DRV_BME280_TASK_READ_CALIBH1:
                /* record humidity calibration data */
                dObj->calibData.dig_H1 = dObj->readBuffer[0];
                /* advance the initialisation to the next state */
                dObj->taskState = DRV_BME280_TASK_READ_CALIBH2;   
                break;            
            case DRV_BME280_TASK_READ_CALIBH2:
                dObj->calibData.dig_H2 = (int16_t) dObj->readBuffer[1] << 8;
                dObj->calibData.dig_H2 |= dObj->readBuffer[0];
                dObj->calibData.dig_H3 = dObj->readBuffer[2];
                
                msb = (int16_t) (int8_t)dObj->readBuffer[3] * 16;
                lsb = (int16_t) (dObj->readBuffer[4] & 0x0F);
                dObj->calibData.dig_H4 = msb | lsb;
                
                msb = (int16_t) (int8_t)dObj->readBuffer[5] * 16;
                lsb = (int16_t) (dObj->readBuffer[4] >> 4);
                dObj->calibData.dig_H5 = msb | lsb;
                               
                dObj->calibData.dig_H6 = dObj->readBuffer[6];                
                dObj->taskState = DRV_BME280_SET_OVERSAMPLING1;
                break; 
            case DRV_BME280_TASK_READ:
                /* parse the read data from the sensor */
                _DRV_BME280_ParseData(dObj, dObj->readBuffer);
                
                /* compensate the data */
                dObj->compData.temperature = _DRV_BME280_Compensate_T(&dObj->uncompData, &dObj->calibData);
                dObj->compData.pressure = _DRV_BME280_Compensate_P(&dObj->uncompData, &dObj->calibData);
                dObj->compData.humidity = _DRV_BME280_Compensate_H(&dObj->uncompData, &dObj->calibData);
                dObj->taskState = DRV_BME280_TASK_IDLE;
                break;
            default:
                break;
        }
        
        dObj->status = SYS_STATUS_READY;        
        dObj->activeClient = NULL;
    }
    
    if (clientObj != NULL) 
    {
        if (clientObj->callback != NULL)
        {
            clientObj->callback(DRV_BME280_TRANSFER_STATUS_COMPLETED, clientObj->context);
        }
    }
}

static DRV_BME280_CLIENT_OBJ* _DRV_BME280_ClientObjGet(const DRV_HANDLE handle)
{
    uint32_t drvIndex = handle >> 8;
    uint32_t clientIndex = handle & 0xFF;
    
    if (drvIndex < DRV_BME280_INSTANCES_NUMBER)
    {
        if (clientIndex < gDrvBME280Obj[drvIndex].nClientsMax)
        {
            if (gDrvBME280Obj[drvIndex].clientObjPool[clientIndex].inUse == true)
            {
                return &gDrvBME280Obj[drvIndex].clientObjPool[clientIndex];
            }
        }
    }
    
    return NULL;
}

static int32_t _DRV_BME280_FreeClientGet(DRV_BME280_OBJ* dObj)
{
    for (int32_t i = 0; i < dObj->nClientsMax; i++) 
    {
        if (dObj->clientObjPool[i].inUse == false)
        {
            dObj->clientObjPool[i].inUse = true;
            return i;
        }
    }
    
    return -1;
}

/* perform a writeRead of the specified register expecting length bytes back */
/* Data will be returned via the device driver callback */
static void _DRV_BME280_ReadReg(DRV_BME280_OBJ* dObj, uint8_t reg, uint8_t length)
{
    if (dObj == NULL)
    {
        return;
    }
    
    /* this function is used by the driver itself so there is no active client */
    dObj->activeClient = NULL;
    dObj->event = DRV_BME280_EVENT_READ_DONE;
    dObj->status = SYS_STATUS_BUSY;
            
    /* send the request */
    dObj->writeBuffer[0] = reg;
    dObj->plibInterface->writeRead(dObj->configParams.sensorAddr, (void*) dObj->writeBuffer, 1, (void*) dObj->readBuffer, length);      
}

// *****************************************************************************
// *****************************************************************************
// Section: DRV_BME280 Driver Global Functions
// *****************************************************************************
// *****************************************************************************

SYS_MODULE_OBJ DRV_BME280_Initialize(
    const SYS_MODULE_INDEX drvIndex,
    const SYS_MODULE_INIT * const init
)
{
    DRV_BME280_OBJ* dObj = NULL;
    DRV_BME280_INIT *BME280Init = (DRV_BME280_INIT *)init;

    /* Validate the request */
    if(drvIndex >= DRV_BME280_INSTANCES_NUMBER)
    {
        return SYS_MODULE_OBJ_INVALID;
    }
    else
    {
        dObj = &gDrvBME280Obj[drvIndex];
    }
    
    dObj->status = SYS_STATUS_UNINITIALIZED;
    
    if(dObj->inUse == true)
    {
        return SYS_MODULE_OBJ_INVALID;
    }

    /* initialize the parameters */
    dObj->inUse = true;
    dObj->nClients = 0;
    dObj->activeClient = NULL;
    dObj->plibInterface = BME280Init->plibInterface;
    dObj->configParams = BME280Init->configParams;
    dObj->clientObjPool = (DRV_BME280_CLIENT_OBJ*) BME280Init->clientObjPool;
    dObj->nClientsMax = BME280Init->maxClients;
    dObj->plibInterface->callbackRegister(_DRV_BME280_PLIBEventHandler, (uintptr_t) dObj);
    dObj->taskState = DRV_BME280_TASK_INIT;

    /* set status */
    dObj->status = SYS_STATUS_READY;
    
    /* the SYS_MODULE_OBJ for this driver will be the index */
    return (SYS_MODULE_OBJ) drvIndex;
}

SYS_STATUS DRV_BME280_Status( const SYS_MODULE_INDEX drvIndex )
{
    DRV_BME280_OBJ* dObj = NULL;
    
    if (drvIndex >= DRV_BME280_INSTANCES_NUMBER)
    {
        return SYS_STATUS_ERROR;
    }
    
    /* if the driver is still initializing or in the middle of a read 
     *  return a BUSY status to the application code rather than the true status */
    dObj = &gDrvBME280Obj[drvIndex];
    if (dObj->taskState != DRV_BME280_TASK_IDLE)
    {
        return SYS_STATUS_BUSY;
    }
    
    /* Return the actual driver status */
    return dObj->status;
}

DRV_HANDLE DRV_BME280_Open(
    const SYS_MODULE_INDEX drvIndex,
    const DRV_IO_INTENT ioIntent
)
{
    DRV_BME280_OBJ* dObj = NULL;
    DRV_BME280_CLIENT_OBJ* clientObj = NULL;
    int32_t iClient;
    
    /* Validate the request */
    if (drvIndex >= DRV_BME280_INSTANCES_NUMBER)
    {
        return DRV_HANDLE_INVALID;
    }

    dObj = &gDrvBME280Obj[drvIndex];
    
    /* Flag error if: 
     * Driver is already opened by maximum configured clients
     */
    if((dObj->status != SYS_STATUS_READY) || 
            (dObj->nClients >= dObj->nClientsMax))
    {
        return DRV_HANDLE_INVALID;
    }

    /* get a free client handle */
    iClient = _DRV_BME280_FreeClientGet(dObj);
    if (iClient >= 0)
    {
        clientObj = &dObj->clientObjPool[iClient];
        clientObj->drvIndex = drvIndex;            
        dObj->nClients++;
        
        return (_DRV_BME280_MAKE_HANDLE(drvIndex, iClient));
    }
    
    return DRV_HANDLE_INVALID;
}

void DRV_BME280_Close( const DRV_HANDLE handle )
{
    DRV_BME280_CLIENT_OBJ* clientObj = _DRV_BME280_ClientObjGet(handle);
    DRV_BME280_OBJ* dObj = NULL;
    
    if (clientObj != NULL)
    {
        dObj = &gDrvBME280Obj[clientObj->drvIndex];
        dObj->nClients--;
        clientObj->inUse = false;
    }
}

void DRV_BME280_ClientEventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_BME280_APP_CALLBACK callback,
    const uintptr_t context
)
{
    DRV_BME280_CLIENT_OBJ* clientObj = _DRV_BME280_ClientObjGet(handle);
    
    if (clientObj != NULL)
    {
        clientObj->callback = callback;
        clientObj->context = context;
    }
}

bool DRV_BME280_Get_Temperature(const DRV_HANDLE handle, int32_t* temperature)
{   
    DRV_BME280_OBJ* dObj;
    DRV_BME280_CLIENT_OBJ* clientObj = NULL;
   
    if (handle == DRV_HANDLE_INVALID)
    {
        return false;
    }
    
    clientObj = _DRV_BME280_ClientObjGet(handle);
    if ((clientObj == NULL) || (clientObj->drvIndex >= DRV_BME280_INSTANCES_NUMBER))
    {
        return false;
    }
    
    dObj = &gDrvBME280Obj[clientObj->drvIndex];
    *temperature = dObj->compData.temperature;
    return true;
}

bool DRV_BME280_Get_Pressure(const DRV_HANDLE handle, uint32_t* pressure)
{
    DRV_BME280_OBJ* dObj;
    DRV_BME280_CLIENT_OBJ* clientObj = NULL;
   
    if (handle == DRV_HANDLE_INVALID)
    {
        return false;
    }
    
    clientObj = _DRV_BME280_ClientObjGet(handle);
    if ((clientObj == NULL) || (clientObj->drvIndex >= DRV_BME280_INSTANCES_NUMBER))
    {
        return false;
    }
    
    dObj = &gDrvBME280Obj[clientObj->drvIndex];
    *pressure = dObj->compData.pressure;
    return false;
}

bool DRV_BME280_Get_Humidity(const DRV_HANDLE handle, uint32_t* humidity)
{    
    DRV_BME280_OBJ* dObj;
    DRV_BME280_CLIENT_OBJ* clientObj = NULL;
   
    if (handle == DRV_HANDLE_INVALID)
    {
        return false;
    }
    
    clientObj = _DRV_BME280_ClientObjGet(handle);
    if ((clientObj == NULL) || (clientObj->drvIndex >= DRV_BME280_INSTANCES_NUMBER))
    {
        return false;
    }
    
    dObj = &gDrvBME280Obj[clientObj->drvIndex];
    *humidity = dObj->compData.humidity;
    return false;
}

/* request an asynchronous read of the BME280 sensor */
bool DRV_BME280_Read(const DRV_HANDLE handle)
{
    DRV_BME280_OBJ* dObj;
    DRV_BME280_CLIENT_OBJ* clientObj = NULL;
   
    if (handle == DRV_HANDLE_INVALID)
    {
        return false;
    }
    
    clientObj = _DRV_BME280_ClientObjGet(handle);
    if ((clientObj == NULL) || (clientObj->drvIndex >= DRV_BME280_INSTANCES_NUMBER))
    {
        return false;
    }
    
    dObj = &gDrvBME280Obj[clientObj->drvIndex];
    dObj->status = SYS_STATUS_BUSY;
    dObj->activeClient = clientObj;
    dObj->taskState = DRV_BME280_TASK_READ;
    dObj->event = DRV_BME280_EVENT_READ_DONE;

    /* send the request */
    dObj->writeBuffer[0] = DRV_BME280_REG_DATA_ADDR;
    dObj->plibInterface->writeRead(dObj->configParams.sensorAddr, (void*) dObj->writeBuffer, 1, (void*) dObj->readBuffer, DRV_BME280_REG_DATA_LEN);   
    
    return true;    
}

void DRV_BME280_Tasks(SYS_MODULE_OBJ object)
{
    DRV_BME280_OBJ* dObj = NULL;
    
    if ((object == SYS_MODULE_OBJ_INVALID) ||
        (object >= DRV_BME280_INSTANCES_NUMBER))
    {
        /* invalid system object */
        return;
    }
    
    dObj = &gDrvBME280Obj[object];  
    
    if (dObj->status == SYS_STATUS_BUSY)
    {
        return;
    }
    
    switch (dObj->taskState)
    {
        case DRV_BME280_TASK_INIT:
            /* perform a device reset */    
            /* advance the reset we will automatically advance to the next state */
            dObj->taskState = DRV_BME280_TASK_READ_ID;    
            dObj->activeClient = NULL;
            dObj->event = DRV_BME280_EVENT_WRITE_DONE;
            dObj->status = SYS_STATUS_BUSY;
            
            /* configure the i2c interface */
            dObj->plibInterface->transferSetup(&dObj->configParams.transferParams, 0);
            
            /* send the request */
            dObj->writeBuffer[0] = DRV_BME280_REG_RESET;
            dObj->writeBuffer[1] = DRV_BME280_SOFT_RESET;
            dObj->plibInterface->write(dObj->configParams.sensorAddr, (void*) dObj->writeBuffer, 2);
            break;
        
        case DRV_BME280_TASK_READ_ID:
            /* read the device ID */        
            _DRV_BME280_ReadReg(dObj, DRV_BME280_CHIP_ID_ADDR, 1);
            break;
            
        case DRV_BME280_TASK_READ_CALIBT:
            /* read the Temperature calibration data into the readBuffer */
            /* state will only be advanced once read has completed and cal data stored */
            _DRV_BME280_ReadReg(dObj, DRV_BME280_CALIB_TEMP_DIG_T1_LSB_REG, 6);        
            break;
            
        case DRV_BME280_TASK_READ_CALIBP:
            /* read the Pressure calibration data into the readBuffer */
            /* state will only be advanced once read has completed and cal data stored */ 
            _DRV_BME280_ReadReg(dObj, DRV_BME280_CALIB_PRESS_DIG_P1_LSB_REG, 18);             
            break;
            
        case DRV_BME280_TASK_READ_CALIBH1:
            /* read the Humidity calibration data into the readBuffer */
            /* performed in 2 steps because the data is not contiguous */
            /* state will only be advanced once read has completed and cal data stored */
            _DRV_BME280_ReadReg(dObj, DRV_BME280_CALIB_HUM_DIG_H1_REG, 1); 
            break;
            
        case DRV_BME280_TASK_READ_CALIBH2:
            /* read the Humidity calibration data into the readBuffer */
            /* state will only be advanced once read has completed and cal data stored */
            _DRV_BME280_ReadReg(dObj, DRV_BME280_CALIB_HUM_DIG_H2_LSB_REG, 7);  
            break;
             
        case DRV_BME280_SET_OVERSAMPLING1:
            /*set the power mode to normal sampling */
            /* this must occur before power mode set */
            dObj->taskState = DRV_BME280_TASK_POWERMODE_SET;    
            dObj->activeClient = NULL;
            dObj->event = DRV_BME280_EVENT_WRITE_DONE;
            dObj->status = SYS_STATUS_BUSY;
            
            /* send the request */
            dObj->writeBuffer[0] = DRV_BME280_REG_CTRL_HUMIDITY;
            dObj->writeBuffer[1] = DRV_BME280_MODE_SAMPLING_H;
            dObj->plibInterface->write(dObj->configParams.sensorAddr, (void*) dObj->writeBuffer, 2);
            break;

        case DRV_BME280_TASK_POWERMODE_SET:
            /*set the power mode to normal sampling with x1 temp and pressure sampling*/
            dObj->taskState = DRV_BME280_TASK_IDLE;    
            dObj->activeClient = NULL;
            dObj->event = DRV_BME280_EVENT_WRITE_DONE;
            dObj->status = SYS_STATUS_BUSY;
            
            /* send the request */
            dObj->writeBuffer[0] = DRV_BME280_REG_CTRL_MEAS;
            /* enable normal sampling, 1x pressure, 1x temperature*/
            dObj->writeBuffer[1] = DRV_BME280_MODE_SAMPLING_T | DRV_BME280_MODE_SAMPLING_P |DRV_BME280_MODE_NORMAL;
            dObj->plibInterface->write(dObj->configParams.sensorAddr, (void*) dObj->writeBuffer, 2);
            break;
            
        case DRV_BME280_TASK_READ:
            /* read is currently being performed */
            break;
            
        case DRV_BME280_TASK_IDLE:
            /* do nothing until the next read interval or async read */
            break;
            
        case DRV_BME280_TASK_ERROR:
            break;
    }
}

