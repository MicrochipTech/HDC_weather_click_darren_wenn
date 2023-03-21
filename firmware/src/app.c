/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app.c

  Summary:
    This file contains the source code for the MPLAB Harmony application.

  Description:
    This file contains the source code for the MPLAB Harmony application.  It
    implements the logic of the application's state machine and it may call
    API routines of other MPLAB Harmony modules in the system, such as drivers,
    system services, and middleware.  However, it does not call any of the
    system interfaces (such as the "Initialize" and "Tasks" functions) of any of
    the modules in the system or make any assumptions about when those functions
    are called.  That is the responsibility of the configuration-specific system
    files.
 *******************************************************************************/

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include "app.h"
#include "app_sdcard.h"
#include "driver/bme280/drv_bme280.h"
#include "peripheral/sercom/usart/plib_sercom2_usart.h"
#include "system/time/sys_time.h"
#include "peripheral/port/plib_port.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************

const uint8_t main_menu[] = 
{
    "*** BME280 Weather Sensor Demonstration ***\r\n"
    "Connect BME280 Mikroe Click board to EXT1\r\n"
    "1: Read data from BME280\r\n"  
    "Press any key to clear screen and print menu\r\n\r\n"
};

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_DATA appData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************

void appDRVBME280EventHandler(const DRV_BME280_TRANSFER_STATUS event, uintptr_t context)
{
    APP_DATA* pApp = NULL;
    
    /* check for read completion and advance state */
    if (event == DRV_BME280_TRANSFER_STATUS_COMPLETED)
    {
        pApp = (APP_DATA*) context;
        if (pApp != NULL)
        {
            pApp->state = APP_STATE_DISPLAY_WEATHER;
        }
    }
}

void APP_SENSOR_TimerEventHandler(uintptr_t context)
{
    APP_DATA* pApp = (APP_DATA*) context;
    
    /* request a read of the weather */
    pApp->sampleCount++;
    DRV_BME280_Read(pApp->drvBME280);    
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************

/* TODO:  Add any necessary local functions.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_Initialize ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Initialize ( void )
{
    /* Place the App state machine in its initial state. */
    appData.state = APP_STATE_INIT;
    appData.sampleCount = 0;
}


/******************************************************************************
  Function:
    void APP_Tasks ( void )

  Remarks:
    See prototype in app.h.
 */

void APP_Tasks ( void )
{
    uint8_t inChar;
    int32_t temperature;
    uint32_t pressure;
    uint32_t humidity;
    double fTemperature, fPressure, fHumidity;
    
    /* Check the application's current state. */
    switch ( appData.state )
    {
        /* Application's initial state. */
        case APP_STATE_INIT:
         
            appData.drvBME280 = DRV_BME280_Open(0, DRV_IO_INTENT_EXCLUSIVE);
                        
            if (appData.drvBME280 == DRV_HANDLE_INVALID)
            {
                //* unable to open the driver at this point */
                return;
            }

            /* register a callback for reading the weather */
            SYS_TIME_CallbackRegisterMS(APP_SENSOR_TimerEventHandler, (uintptr_t) &appData,
                    5000, SYS_TIME_PERIODIC);
            
            printf("\33[H\33[2J");
            printf("%s", main_menu);
    
            /* register a callback with the BME280 driver for when new data is available */
            DRV_BME280_ClientEventHandlerSet(appData.drvBME280, appDRVBME280EventHandler, (uintptr_t) &appData);
            appData.state = APP_STATE_WAIT_FOR_BME280;
            break;

        case APP_STATE_WAIT_FOR_BME280:
            if (DRV_BME280_Status(0) == SYS_STATUS_READY)
            {
                appData.state = APP_STATE_IDLE;
            }
            break;
            
        case APP_STATE_IDLE:
            /* check for a key press and act accordingly */
            if (SERCOM2_USART_ReadIsBusy() == false)
            {
                inChar = getc(stdin); 
                if (inChar == '1')
                {
                    appData.state = APP_STATE_READ_WEATHER;
                    /* request a read of the weather */
                    DRV_BME280_Read(appData.drvBME280);
                }
            }
            break;     
            
        case APP_STATE_READ_WEATHER:
            /* this is a holding state until the value comes back */
            break;
            
        case APP_STATE_DISPLAY_WEATHER:
            /* data read has completed, display results */
            DRV_BME280_Get_Temperature(appData.drvBME280, &temperature);
            DRV_BME280_Get_Pressure(appData.drvBME280, &pressure);
            DRV_BME280_Get_Humidity(appData.drvBME280, &humidity);
            fTemperature = ((double) temperature) / 100.0f;
            fPressure = ((double) pressure) / 100.0f;
            fHumidity = ((double) humidity) / 1024.0f;
            //printf("%6ld\tTemperature = %6.2f\tPressure = %7.2f\tHumidity = %5.1f\r\n",
            //        appData.sampleCount, fTemperature, fPressure, fHumidity);
            
            /* log the temperature if SD card is present */
            APP_SDCARD_Notify(fTemperature, fPressure, fHumidity);
            appData.state = APP_STATE_IDLE;
            break;

        /* The default state should never be executed. */
        default:        
            break;
    }
}


/*******************************************************************************
 End of File
 */
