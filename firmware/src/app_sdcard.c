/*******************************************************************************
  MPLAB Harmony Application Source File

  Company:
    Microchip Technology Inc.

  File Name:
    app_sdcard.c

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

#include "app_sdcard.h"
#include "peripheral/rtc/plib_rtc.h"
#include "peripheral/port/plib_port.h"
#include "system/fs/sys_fs.h"

// *****************************************************************************
// *****************************************************************************
// Section: Global Data Definitions
// *****************************************************************************
// *****************************************************************************
#define SDCARD_MOUNT_NAME    SYS_FS_MEDIA_IDX0_MOUNT_NAME_VOLUME_IDX0
#define SDCARD_DEV_NAME      SYS_FS_MEDIA_IDX0_DEVICE_NAME_VOLUME_IDX0
#define SDCARD_FILE_NAME     "data_log.txt"

#define BUILD_TIME_HOUR     ((__TIME__[0] - '0') * 10 + __TIME__[1] - '0')
#define BUILD_TIME_MIN      ((__TIME__[3] - '0') * 10 + __TIME__[4] - '0')
#define BUILD_TIME_SEC      ((__TIME__[6] - '0') * 10 + __TIME__[7] - '0')

#define LOG_TIME_LEN        10
#define LOG_TEMP_LEN        18
#define LOG_LEN             (LOG_TIME_LEN + LOG_TEMP_LEN)

// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    This structure should be initialized by the APP_SDCARD_Initialize function.

    Application strings and buffers are be defined outside this structure.
*/

APP_SDCARD_DATA app_sdcardData;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Functions
// *****************************************************************************
// *****************************************************************************
void APP_SDCARD_Notify(double temperature, double pressure, double humidity)
{
    /* New weather data ready */
    app_sdcardData.temperature = temperature;
    app_sdcardData.pressure = pressure;
    app_sdcardData.humidity = humidity;
    
    app_sdcardData.isDataReady = true;
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Local Functions
// *****************************************************************************
// *****************************************************************************
static void APP_SysFSEventHandler(SYS_FS_EVENT event,void* eventData,uintptr_t context)
{
    switch(event)
    {
        /* If the event is mount then check which media has been mounted */
        case SYS_FS_EVENT_MOUNT:
            if(strcmp((const char *)eventData, SDCARD_MOUNT_NAME) == 0)
            {
                /* Set SDCARD Mount flag */
                app_sdcardData.sdCardMountFlag = true;
            }
            break;

        /* If the event is unmount then check which media has been unmount */
        case SYS_FS_EVENT_UNMOUNT:
            if(strcmp((const char *)eventData, SDCARD_MOUNT_NAME) == 0)
            {
                app_sdcardData.sdCardMountFlag = false;
            }

            if (app_sdcardData.state != APP_SDCARD_STATE_IDLE)
            {
                printf("!!! WARNING SDCARD Ejected Abruptly !!!\r\n\r\n");

                LED0_Clear();

                app_sdcardData.state = APP_SDCARD_STATE_ERROR;
            }
            break;
            
        case SYS_FS_EVENT_MOUNT_WITH_NO_FILESYSTEM:
            printf("SD Card has no filesystem!!!\r\n\n\n");
            break;

        case SYS_FS_EVENT_ERROR:
            break;
    }
}

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_SDCARD_Initialize ( void )

  Remarks:
    See prototype in app_sdcard.h.
 */

void APP_SDCARD_Initialize ( void )
{
    struct tm sys_time;
    char s_month[5];
    int month, day, year;
    static const char month_names[] = "JanFebMarAprMayJunJulAugSepOctNovDec";

    /* Intialize the app state to wait for media attach. */
    app_sdcardData.state                    = APP_SDCARD_STATE_MOUNT_WAIT;

    app_sdcardData.isDataReady              = false;
   
    app_sdcardData.sdCardMountFlag          = false;

    /* calculate the system date and time from the build time */
    sscanf(__DATE__, "%s %d %d", s_month, &day, &year);
    month = (strstr(month_names, s_month) - month_names) / 3;
    
    sys_time.tm_year                        = year - 1900;
    sys_time.tm_mon                         = month;
    sys_time.tm_mday                        = day;
    sys_time.tm_hour                        = BUILD_TIME_HOUR;
    sys_time.tm_min                         = BUILD_TIME_MIN;
    sys_time.tm_sec                         = BUILD_TIME_SEC;

    /* Set RTC Time to current system time.*/
    RTC_RTCCTimeSet(&sys_time);

    /* Register the File System Event handler.*/
    SYS_FS_EventHandlerSet(APP_SysFSEventHandler,(uintptr_t)NULL);
}


/******************************************************************************
  Function:
    void APP_SDCARD_Tasks ( void )

  Remarks:
    See prototype in app_sdcard.h.
 */

void APP_SDCARD_Tasks ( void )
{
    struct tm sys_time = { 0 };
    char log_date[30];
    char log_data[128];

    switch (app_sdcardData.state)
    {
        case APP_SDCARD_STATE_MOUNT_WAIT:
        {
            /* Wait for SDCARD to be Auto Mounted. Here -----> Step #4 */
            if(app_sdcardData.sdCardMountFlag == true)
            {
                app_sdcardData.state = APP_SDCARD_STATE_OPEN_FILE;
                app_sdcardData.sdCardMountFlag = false;
            }
            break;
        }

        case APP_SDCARD_STATE_OPEN_FILE:
        {
            /* Open Temperature Log file. Here -----> Step #5 */
            app_sdcardData.fileHandle = SYS_FS_FileOpen(SDCARD_MOUNT_NAME"/"SDCARD_FILE_NAME,
                                                       (SYS_FS_FILE_OPEN_WRITE));

            if(app_sdcardData.fileHandle == SYS_FS_HANDLE_INVALID)
            {
                /* Could not open the file. Error out*/
                app_sdcardData.state = APP_SDCARD_STATE_ERROR;
                break;
            }

            app_sdcardData.state = APP_SDCARD_STATE_WRITE;

            break;
        }

        case APP_SDCARD_STATE_WRITE:
        {
            /* Check if temperature data is ready to be written to SDCARD. */
            if (app_sdcardData.isDataReady == true)
            {
                app_sdcardData.isDataReady = false;

                /* Get System Time from RTC */
                RTC_RTCCTimeGet(&sys_time);
                
                sprintf(log_date, "[%04d/%02d/%02d %02d:%02d:%02d]", sys_time.tm_year + 1900, sys_time.tm_mon + 1,
                        sys_time.tm_mday, sys_time.tm_hour, sys_time.tm_min, sys_time.tm_sec);
                sprintf(log_data, "%s %6.2f %7.2f %5.1f", log_date, app_sdcardData.temperature,
                        app_sdcardData.pressure, app_sdcardData.humidity);
                
                printf("%s\r\n", log_data);

                /* Log System time and temperature value to log file. */
                if(SYS_FS_FilePrintf(app_sdcardData.fileHandle, "%s\r\n", log_data)
                                    == SYS_FS_RES_FAILURE)
                {
                    /* There was an error while reading the file error out. */
                    app_sdcardData.state = APP_SDCARD_STATE_ERROR;
                }
                else
                {
                    /* The test was successful. */
                    LED0_Toggle();
                    app_sdcardData.state = APP_SDCARD_STATE_SWITCH_CHECK;
                }
            }
            break;
        }

        case APP_SDCARD_STATE_SWITCH_CHECK:
        {
            /* Check if Switch is pressed to stop logging of data. Here -----> Step #9 */
            if ( SWITCH_Get() == 0)
            {
                app_sdcardData.state = APP_SDCARD_STATE_CLOSE_FILE;
            }
            else
            {
                app_sdcardData.state = APP_SDCARD_STATE_WRITE;
            }
                 
            break;
        }

        case APP_SDCARD_STATE_CLOSE_FILE:
        {
            SYS_FS_FileClose(app_sdcardData.fileHandle);

            printf("Logging temperature to SDCARD Stopped \r\n");
            printf("Safe to Eject SDCARD \r\n\r\n");

            LED0_Clear();

            app_sdcardData.state = APP_SDCARD_STATE_IDLE;

            break;
        }

        case APP_SDCARD_STATE_ERROR:
        {
            printf("SDCARD Task Error \r\n\r\n");
            app_sdcardData.state = APP_SDCARD_STATE_IDLE;
            break;
        }

        case APP_SDCARD_STATE_IDLE:
        default:
        {
            break;
        }
    }
}


/*******************************************************************************
 End of File
 */
