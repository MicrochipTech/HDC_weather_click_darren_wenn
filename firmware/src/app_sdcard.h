/*******************************************************************************
  MPLAB Harmony Application Header File

  Company:
    Microchip Technology Inc.

  File Name:
    app_sdcard.h

  Summary:
    This header file provides prototypes and definitions for the application.

  Description:
    This header file provides function prototypes and data type definitions for
    the application.  Some of these are required by the system (such as the
    "APP_SDCARD_Initialize" and "APP_SDCARD_Tasks" prototypes) and some of them are only used
    internally by the application (such as the "APP_SDCARD_STATES" definition).  Both
    are defined here for convenience.
*******************************************************************************/

#ifndef _APP_SDCARD_H
#define _APP_SDCARD_H

// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "system/fs/sys_fs.h"
#include "configuration.h"

// DOM-IGNORE-BEGIN
#ifdef __cplusplus  // Provide C++ Compatibility

extern "C" {

#endif
// DOM-IGNORE-END

// *****************************************************************************
// *****************************************************************************
// Section: Type Definitions
// *****************************************************************************
// *****************************************************************************

// *****************************************************************************
/* Application states

  Summary:
    Application states enumeration

  Description:
    This enumeration defines the valid application states.  These states
    determine the behavior of the application at various times.
*/

typedef enum
{
    /* Initial state. */
    APP_SDCARD_STATE_MOUNT_WAIT,

    /* The app opens the file */
    APP_SDCARD_STATE_OPEN_FILE,

    /* Check For Switch */
    APP_SDCARD_STATE_SWITCH_CHECK,

    /* Write temperature data to SDCARD */
    APP_SDCARD_STATE_WRITE,

    /* Close SDCARD File */
    APP_SDCARD_STATE_CLOSE_FILE,

    /* Error state */
    APP_SDCARD_STATE_ERROR,

    /* Idle State */
    APP_SDCARD_STATE_IDLE,
} APP_SDCARD_STATES;


// *****************************************************************************
/* Application Data

  Summary:
    Holds application data

  Description:
    This structure holds the application's data.

  Remarks:
    Application strings and buffers are be defined outside this structure.
 */

typedef struct
{
    /* Application's current state */
    APP_SDCARD_STATES  state;

    /* SYS_FS File Handle */
    SYS_FS_HANDLE      fileHandle;

    /* Indicates whether SD card is mounted or not */
    bool               sdCardMountFlag;

    /* values to be written to SDCARD */
    double              temperature;
    double              pressure;
    double              humidity;

    /* Flag to indicate whether data is read from the BME280 sensor */
    volatile bool isDataReady;
} APP_SDCARD_DATA;

// *****************************************************************************
// *****************************************************************************
// Section: Application Callback Routines
// *****************************************************************************
// *****************************************************************************
/* These routines are called by drivers when certain events occur.
*/

// *****************************************************************************
// *****************************************************************************
// Section: Application Initialization and State Machine Functions
// *****************************************************************************
// *****************************************************************************

/*******************************************************************************
  Function:
    void APP_SDCARD_Initialize ( void )

  Summary:
     MPLAB Harmony application initialization routine.

  Description:
    This function initializes the Harmony application.  It places the
    application in its initial state and prepares it to run so that its
    APP_SDCARD_Tasks function can be called.

  Precondition:
    All other system initialization routines should be called before calling
    this routine (in "SYS_Initialize").

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_SDCARD_Initialize();
    </code>

  Remarks:
    This routine must be called from the SYS_Initialize function.
*/

void APP_SDCARD_Initialize ( void );


/*******************************************************************************
  Function:
    void APP_SDCARD_Tasks ( void )

  Summary:
    MPLAB Harmony Demo application tasks function

  Description:
    This routine is the Harmony Demo application's tasks function.  It
    defines the application's state machine and core logic.

  Precondition:
    The system and application initialization ("SYS_Initialize") should be
    called before calling this.

  Parameters:
    None.

  Returns:
    None.

  Example:
    <code>
    APP_SDCARD_Tasks();
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */

void APP_SDCARD_Tasks( void );

/*******************************************************************************
  Function:
    void APP_SDCARD_Notify(uint8_t temperature)

  Summary:
    MPLAB Harmony SDCARD application Notify function

  Description:
    This routine is used to update the temperature sensor value received from
    I2C Temperature Sensor Task. It also updates the temperature ready flag
    which is polled by SDCARD Task routine to write the received values to SDCARD.

    This function will called by I2C Temperature Sensor Task.

  Precondition:
    None

  Parameters:
    temperature - Temperature Sensor value

  Returns:
    None.

  Example:
    <code>
    APP_SDCARD_Notify(temperature);
    </code>

  Remarks:
    This routine must be called from SYS_Tasks() routine.
 */
void APP_SDCARD_Notify(double temperature, double pressure, double humidity);


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif /* _APP_SDCARD_H */

/*******************************************************************************
 End of File
 */

