#ifndef DRV_TEMP_SENSOR_DEFINITIONS_H
#define DRV_TEMP_SENSOR_DEFINITIONS_H

#include "stddef.h"
#include "system/system_module.h"
#include "driver/driver.h"
#include "driver/driver_common.h"
#include "system/int/sys_int.h"

typedef enum
{
    DRV_TEMP_SENSOR_ERROR_NONE,
    DRV_TEMP_SENSOR_ERROR_NACK,
    DRV_TEMP_SENSOR_ERROR_BUS,
} DRV_TEMP_SENSOR_ERROR;

typedef enum
{
    DRV_TEMP_SENSOR_EVENT_TEMP_READ_DONE = 0,
    DRV_TEMP_SENSOR_EVENT_EEPROM_WRITE_DONE,
    DRV_TEMP_SENSOR_EVENT_EEPROM_READ_DONE,
    DRV_TEMP_SENSOR_EVENT_ERROR,
} DRV_TEMP_SENSOR_EVENT;

typedef struct
{
    /* clock speed */
    uint32_t clockSpeed;
} DRV_TEMP_SENSOR_TRANSFER_SETUP;

typedef struct
{
    uint8_t tempSensorAddr;
    uint8_t eepromAddr;    
    DRV_TEMP_SENSOR_TRANSFER_SETUP transferParams;
}DRV_TEMP_SENSOR_CONFIG_PARAMS;

// function prototype of application callback function
typedef void (*DRV_TEMP_SENSOR_APP_CALLBACK)(DRV_TEMP_SENSOR_EVENT event, uintptr_t context);

// function prototype of callback registered by driver with the PLIB
typedef void (* DRV_TEMP_SENSOR_PLIB_CALLBACK)( uintptr_t context);

// function prototypes of PLIB APIs
typedef bool (* DRV_TEMP_SENSOR_PLIB_READ)( uint16_t, uint8_t *, uint32_t );
typedef bool (* DRV_TEMP_SENSOR_PLIB_WRITE)( uint16_t, uint8_t *, uint32_t );
typedef bool (* DRV_TEMP_SENSOR_PLIB_WRITE_READ)( uint16_t, uint8_t *, uint32_t, uint8_t *, uint32_t );
typedef DRV_TEMP_SENSOR_ERROR (* DRV_TEMP_SENSOR_PLIB_ERROR_GET)( void );
typedef bool (* DRV_TEMP_SENSOR_PLIB_TRANSFER_SETUP)(DRV_TEMP_SENSOR_TRANSFER_SETUP* setup, uint32_t srcClkFreq);
typedef void (* DRV_TEMP_SENSOR_PLIB_CALLBACK_REGISTER)(DRV_TEMP_SENSOR_PLIB_CALLBACK, uintptr_t);

typedef struct
{
    DRV_TEMP_SENSOR_PLIB_READ                           read;
    DRV_TEMP_SENSOR_PLIB_WRITE                          write;
    DRV_TEMP_SENSOR_PLIB_WRITE_READ                     writeRead;
    DRV_TEMP_SENSOR_PLIB_ERROR_GET                      errorGet;
    DRV_TEMP_SENSOR_PLIB_TRANSFER_SETUP                 transferSetup;
    DRV_TEMP_SENSOR_PLIB_CALLBACK_REGISTER              callbackRegister;
} DRV_TEMP_SENSOR_PLIB_INTERFACE;

typedef struct
{
    const DRV_TEMP_SENSOR_PLIB_INTERFACE* plibInterface;
    const uintptr_t clientObjPool;
    uint8_t maxClients;
}DRV_TEMP_SENSOR_INIT;

#endif      //DRV_TEMP_SENSOR_DEFINITIONS_H