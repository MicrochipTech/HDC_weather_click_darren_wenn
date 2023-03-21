/*******************************************************************************
  DRV_BME280 Driver Interface Definition

  Company:
    Microchip Technology Inc.

  File Name:
    drv_bme280.h

  Summary:
    BME280 EEPROM Library Interface header.

  Description:
    The BME280 Driver Library provides an interface to access the 
    Mikroe Weather Click featuring a BME280 Sensor.
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

#ifndef _DRV_BME280_H
#define _DRV_BME280_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <stdio.h>
#include <stdbool.h>
#include "driver/driver.h"
#include "system/system.h"
#include "drv_bme280_definitions.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility
    extern "C" {
#endif

// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Data Types
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* DRV_BME280 Transfer Status

 Summary:
    Defines the data type for BME280 Driver transfer status.

 Description:
    This will be used to indicate the current transfer status of the
    BME280 driver operations.

 Remarks:
    None.
*/

typedef enum
{
    /* Transfer is being processed */
    DRV_BME280_TRANSFER_STATUS_BUSY,

    /* Transfer is successfully completed */
    DRV_BME280_TRANSFER_STATUS_COMPLETED,

    /* Transfer had error */
    DRV_BME280_TRANSFER_STATUS_ERROR

} DRV_BME280_TRANSFER_STATUS;

/* function prototype of application callback function */
typedef void (*DRV_BME280_APP_CALLBACK)(DRV_BME280_TRANSFER_STATUS event, uintptr_t conext);

// *****************************************************************************
// *****************************************************************************
// Section: DRV_BME280 Driver Module Interface Routines
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* BME280 Driver Transfer Event Handler Function Pointer

   Summary
    Pointer to a BME280 Driver Event handler function

   Description
    This data type defines the required function signature for the BME280 driver
    event handling callback function. A client must register a pointer
    using the event handling function whose function signature (parameter
    and return value types) match the types specified by this function pointer
    in order to receive transfer related event calls back from the driver.

    The parameters and return values are described here and a partial example
    implementation is provided.

  Parameters:
    event - Identifies the type of event

    context - Value identifying the context of the application that
    registered the event handling function.

  Returns:
    None.

  Example:
    <code>
    void APP_MyTransferEventHandler( DRV_BME280_TRANSFER_STATUS event, uintptr_t context )
    {
        MY_APP_DATA_STRUCT* pAppData = (MY_APP_DATA_STRUCT *) context;

        switch(event)
        {
            case DRV_BME280_TRANSFER_STATUS_COMPLETED:

                // Handle the transfer complete event.
                break;

            case DRV_BME280_TRANSFER_STATUS_ERROR:
            default:

                // Handle error.
                break;
        }
    }
    </code>

  Remarks:
    If the event is DRV_BME280_TRANSFER_STATUS_COMPLETED, it means that the data was
    transferred successfully and new weather data is available.

    If the event is DRV_BME280_TRANSFER_STATUS_ERROR, it means that the data was not
    transferred successfully.

    The context parameter contains the handle to the client context,
    provided at the time the event handling function was registered using the
    DRV_BME280_EventHandlerSet function.  This context handle value is
    passed back to the client as the "context" parameter.  It can be any value
    necessary to identify the client context or instance (such as a pointer to
    the client's data) instance of the client that made the request.

    The event handler function executes in the driver's interrupt
    context. It is recommended of the application to not perform process
    intensive or blocking operations with in this function.
*/

typedef void (*DRV_BME280_EVENT_HANDLER )( DRV_BME280_TRANSFER_STATUS event, uintptr_t context );

// *****************************************************************************
/* Function:
    SYS_MODULE_OBJ DRV_BME280_Initialize(
        const SYS_MODULE_INDEX drvIndex,
        const SYS_MODULE_INIT * const init
    )

  Summary:
    Initializes the BME280 device

  Description:
    This routine initializes the BME280 device driver making it ready for
    clients to open and use. The initialization data is specified by the init
    parameter. Initialization is a multi-step process as the BME280 device driver
    will interrogate the device and read the calibration parameters. This process
    must be allowed to complete before the driver will respond with 
    SYS_STATUS_READY.

  Precondition:
    None.

  Parameters:
    drvIndex - Identifier for the instance to be initialized

    init  - Pointer to the init data structure containing any data necessary to
            initialize the driver.

  Returns:
    If successful, returns a valid handle to a driver instance object.
    Otherwise, returns SYS_MODULE_OBJ_INVALID.

  Example:
    <code>
    SYS_MODULE_OBJ   sysObjDrvBME280;

    DRV_BME280_PLIB_INTERFACE gDrvBME280PLIBIntf[1] = {
        .read = (DRV_BME280_PLIB_READ) SERCOM3_I2C_Read,
        .write = (DRV_BME280_PLIB_WRITE) SERCOM3_I2C_Write, 
        .writeRead = (DRV_BME280_PLIB_WRITE_READ) SERCOM3_I2C_WriteRead,
        .errorGet = (DRV_BME280_PLIB_ERROR_GET) SERCOM3_I2C_ErrorGet,
        .callbackRegister = (DRV_BME280_PLIB_CALLBACK_REGISTER) SERCOM3_I2C_CallbackRegister,
        .transferSetup = (DRV_BME280_PLIB_TRANSFER_SETUP) SERCOM3_I2C_TransferSetup,
    };

    DRV_BME280_CLIENT_OBJ gDrvBME280Sensor0ClientObjPool[1];

    DRV_BME280_INIT drvBME280InitData = {
        .plibInterface = &gDrvBME280PLIBIntf[0],
        .configParams.sensorAddr = DRV_BME280_I2C_ADDRESS,
        .configParams.transferParams.clockSpeed = 400000,
        .clientObjPool = (uintptr_t) gDrvBME280Sensor0ClientObjPool,
        .maxClients = 1,
    };

    sysObjDrvBME280 = DRV_BME280_Initialize(DRV_BME280_INDEX, (SYS_MODULE_INIT *)&drvBME280InitData);

    </code>

  Remarks:
    This routine must be called before any other DRV_BME280 routine is called.
    This routine should only be called once during system initialization.

*/

SYS_MODULE_OBJ DRV_BME280_Initialize( const SYS_MODULE_INDEX drvIndex, const SYS_MODULE_INIT * const init);

// *************************************************************************
/* Function:
    SYS_STATUS DRV_BME280_Status( const SYS_MODULE_INDEX drvIndex )

  Summary:
    Gets the current status of the BME280 driver module.

  Description:
    This routine provides the current status of the BME280 driver module.

  Preconditions:
    Function DRV_BME280_Initialize should have been called before calling
    this function.

  Parameters:
    drvIndex   -  Identifier for the instance used to initialize driver

  Returns:
    SYS_STATUS_READY - Indicates that the driver is ready and accept
                       requests for new operations.

    SYS_STATUS_UNINITIALIZED - Indicates the driver is not initialized.

  Example:
    <code>
    SYS_STATUS status;

    status = DRV_BME280_Status(DRV_BME280_INDEX);

    if (status == SYS_STATUS_READY)
    {
        // BME280 driver is initialized and ready to accept requests.
    }
    </code>

  Remarks:
    None.
*/

SYS_STATUS DRV_BME280_Status( const SYS_MODULE_INDEX drvIndex );

// *****************************************************************************
/* Function:
    DRV_HANDLE DRV_BME280_Open
    (
        const SYS_MODULE_INDEX drvIndex,
        const DRV_IO_INTENT ioIntent
    )

  Summary:
    Opens the specified BME280 driver instance and returns a handle to it.

  Description:
    This routine opens the specified BME280 driver instance and provides a
    handle that must be provided to all other client-level operations to
    identify the caller and the instance of the driver. The ioIntent
    parameter defines how the client interacts with this driver instance.

  Precondition:
    Function DRV_BME280_Initialize must have been called before calling this
    function.

  Parameters:
    drvIndex  - Identifier for the object instance to be opened

    ioIntent -  Zero or more of the values from the enumeration DRV_IO_INTENT
                "ORed" together to indicate the intended use of the driver.
                Note: This driver ignores the ioIntent argument.

  Returns:
    If successful, the routine returns a valid open-instance handle (a number
    identifying both the caller and the module instance).

    If an error occurs, the return value is DRV_HANDLE_INVALID. Error can occur
    - if the driver instance being opened is not initialized or is invalid.

  Example:
    <code>
    DRV_HANDLE handle;

    handle = DRV_BME280_Open(DRV_BME280_INDEX, DRV_IO_INTENT_READWRITE);
    if (handle == DRV_HANDLE_INVALID)
    {
        // Unable to open the driver
        // May be the driver is not initialized
    }
    </code>

  Remarks:
    The handle returned is valid until the DRV_BME280_Close routine is called.
*/
DRV_HANDLE DRV_BME280_Open(const SYS_MODULE_INDEX drvIndex, const DRV_IO_INTENT ioIntent);

// *****************************************************************************
/* Function:
    void DRV_BME280_Close( const DRV_HANDLE handle )

  Summary:
    Closes the opened-instance of the BME280 driver.

  Description:
    This routine closes opened-instance of the BME280 driver, invalidating the
    handle. A new handle must be obtained by calling DRV_BME280_Open
    before the caller may use the driver again.

  Precondition:
    DRV_BME280_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle -    A valid open-instance handle, returned from the driver's
                open routine

  Returns:
    None.

  Example:
    <code>
    // 'handle', returned from the DRV_BME280_Open

    DRV_BME280_Close(handle);

    </code>

  Remarks:
    None.
*/
void DRV_BME280_Close(const DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    bool DRV_BME280_Read(const DRV_HANDLE handle)

  Summary:
    Requests a read of the BME280 sensor

  Description:
    This function schedules a non-blocking read operation of the temperature,
    pressure and humidity from the BME280.

    The requesting client should call DRV_BME280_Status API to know
    the current status of the request OR the requesting client can register a
    callback function with the driver to get notified of the status.

  Precondition:
    DRV_BME280_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle         - A valid open-instance handle, returned from the driver's
                      open routine
  Returns:
    true
        - if the read request is accepted.

    false
        - if handle is invalid

  Example:
    <code>

    if (DRV_BME280_Read(myHandle) != true)
    {
        // Error handling here
    }
    else
    {
        // Wait for read to be completed
        while(DRV_BME280_Status(myHandle) == SYS_STATUS_BUSY);
    }

    </code>

  Remarks:
    None.
*/
bool DRV_BME280_Read(const DRV_HANDLE handle);

// *****************************************************************************
/* Function:
    bool DRV_BME280_Get_Temperature(const DRV_HANDLE handle, int32_t* temperature);
    bool DRV_BME280_Get_Pressure(const DRV_HANDLE handle, uint32_t* pressure);
    bool DRV_BME280_Get_Humidity(const DRV_HANDLE handle, uint32_t* humidity); 

  Summary:
    Read the correctly converted Temperature/Pressure/Humidity applying the
    calibration parameters to the raw uncompensated values.

  Description:
    These functions return a calibrated version of the last completed read
    operation from the BME280 driver.
    The temperature and pressure values will be returned in integers values
    with a resolution of 0.01 To convert to real values they should be cast to
    a double and divided by 100.0f
    The humidity value is returned multiplied by 1024. To convert to real values
    it should be divided by 1024.0f


  Precondition:
    DRV_BME280_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle         - A valid open-instance handle, returned from the driver's
                      open routine
    temperature/pressure/humidity
                   - A pointer to the correct type to receive the return value
  Returns:
    true
        - if the read request is accepted.

    false
        - if handle is invalid

  Example:
    <code>
    int32_t temperature;
    double fTemperature;
 
    if (DRV_BME280_Get_Temperature(myHandle, &temperature) != true)
    {
        // Error handling here
    }
    else
    {
        fTemperature = ((double) temperature) / 100.0f;   
    }

    </code>

  Remarks:
    None.
*/
bool DRV_BME280_Get_Temperature(const DRV_HANDLE handle, int32_t* temperature);
bool DRV_BME280_Get_Pressure(const DRV_HANDLE handle, uint32_t* pressure);
bool DRV_BME280_Get_Humidity(const DRV_HANDLE handle, uint32_t* humidity);


// *****************************************************************************
/* Function:
    void DRV_BME280_ClientEventHandlerSet(
        const DRV_HANDLE handle,
        const DRV_BME280_APP_CALLBACK eventHandler,
        const uintptr_t context
    )

  Summary:
    Allows a client to identify a transfer event handling function for the driver
    to call back when the requested transfer has finished.

  Description:
    This function allows a client to register a transfer event handling function
    with the driver to call back when the requested transfer has finished.

    The event handler should be set before the client submits any transfer
    requests that could generate events. The event handler once set, persists
    until the client closes the driver or sets another event handler (which
    could be a "NULL" pointer to indicate no callback).

  Precondition:
    DRV_BME280_Open must have been called to obtain a valid opened device handle.

  Parameters:
    handle - A valid open-instance handle, returned from the driver's open routine.

    callback - Pointer to the event handler function.

    context - The value of parameter will be passed back to the client
    unchanged, when the eventHandler function is called.  It can be used to
    identify any client specific data object that identifies the instance of the
    client module (for example, it may be a pointer to the client module's state
    structure).

  Returns:
    None.

  Example:
    <code>

    DRV_BME280_ClientEventHandlerSet(appData.drvBME280, appDRVBME280EventHandler, (uintptr_t) &appData);
    </code>

  Remarks:
    If the client does not want to be notified when the queued buffer transfer
    has completed, it does not need to register a callback.
*/

void DRV_BME280_ClientEventHandlerSet(
    const DRV_HANDLE handle,
    const DRV_BME280_APP_CALLBACK callback,
    const uintptr_t context
);

void DRV_BME280_Tasks(
    SYS_MODULE_OBJ object
);

#ifdef __cplusplus
}
#endif

#include "driver/BME280/src/drv_BME280_local.h"

#endif // #ifndef _DRV_BME280_H
/*******************************************************************************
 End of File
*/
