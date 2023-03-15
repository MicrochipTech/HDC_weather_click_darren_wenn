/*******************************************************************************
  BME280 Driver Local Data Structures

  Company:
    Microchip Technology Inc.

  File Name:
    drv_bme280_local.h

  Summary:
    BME280 Driver Local Data Structures

  Description:
    Driver Local Data Structures
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

#ifndef _DRV_BME280_LOCAL_H
#define _DRV_BME280_LOCAL_H


// *****************************************************************************
// *****************************************************************************
// Section: Included Files
// *****************************************************************************
// *****************************************************************************
#include "configuration.h"

// *****************************************************************************
// *****************************************************************************
// Section: Data Type Definitions
// *****************************************************************************
// *****************************************************************************

#define DRV_BME280_CHIP_ID_ADDR                                     0xD0
#define DRV_BME280_CHIP_ID                                          0x60
        
/* I2C Address definition */
#define DRV_BME280_I2C_ADDRESS                                      0x76

#define DRV_BME280_SLEEP_MODE                                       0x00
#define DRV_BME280_FORCED_MODE                                      0x01
#define DRV_BME280_NORMAL_MODE                                      0x03
#define DRV_BME280_SOFT_RESET                                       0xB6
#define DRV_BME280_OVERSAMPLING_1X                                  0x01              
        
/* Temperature Calibration Register Addresses */
#define DRV_BME280_CALIB_TEMP_DIG_T1_LSB_REG                        0x88
#define DRV_BME280_CALIB_TEMP_DIG_T1_MSB_REG                        0x89
#define DRV_BME280_CALIB_TEMP_DIG_T2_LSB_REG                        0x8A
#define DRV_BME280_CALIB_TEMP_DIG_T2_MSB_REG                        0x8B
#define DRV_BME280_CALIB_TEMP_DIG_T3_LSB_REG                        0x8C
#define DRV_BME280_CALIB_TEMP_DIG_T3_MSB_REG                        0x8D
/* Pressure Calibration Register Addresses */
#define DRV_BME280_CALIB_PRESS_DIG_P1_LSB_REG                       0x8E
#define DRV_BME280_CALIB_PRESS_DIG_P1_MSB_REG                       0x8F
#define DRV_BME280_CALIB_PRESS_DIG_P2_LSB_REG                       0x90
#define DRV_BME280_CALIB_PRESS_DIG_P2_MSB_REG                       0x91
#define DRV_BME280_CALIB_PRESS_DIG_P3_LSB_REG                       0x92
#define DRV_BME280_CALIB_PRESS_DIG_P3_MSB_REG                       0x93
#define DRV_BME280_CALIB_PRESS_DIG_P4_LSB_REG                       0x94
#define DRV_BME280_CALIB_PRESS_DIG_P4_MSB_REG                       0x95
#define DRV_BME280_CALIB_PRESS_DIG_P5_LSB_REG                       0x96
#define DRV_BME280_CALIB_PRESS_DIG_P5_MSB_REG                       0x97
#define DRV_BME280_CALIB_PRESS_DIG_P6_LSB_REG                       0x98
#define DRV_BME280_CALIB_PRESS_DIG_P6_MSB_REG                       0x99
#define DRV_BME280_CALIB_PRESS_DIG_P7_LSB_REG                       0x9A
#define DRV_BME280_CALIB_PRESS_DIG_P7_MSB_REG                       0x9B
#define DRV_BME280_CALIB_PRESS_DIG_P8_LSB_REG                       0x9C
#define DRV_BME280_CALIB_PRESS_DIG_P8_MSB_REG                       0x9D
#define DRV_BME280_CALIB_PRESS_DIG_P9_LSB_REG                       0x9E
#define DRV_BME280_CALIB_PRESS_DIG_P9_MSB_REG                       0x9F
/* Humidity Calibration Register Addresses */    
#define DRV_BME280_CALIB_HUM_DIG_H1_REG                             0xA1
#define DRV_BME280_CALIB_HUM_DIG_H2_LSB_REG                         0xE1
#define DRV_BME280_CALIB_HUM_DIG_H2_MSB_REG                         0xE2
#define DRV_BME280_CALIB_HUM_DIG_H3_REG                             0xE3
#define DRV_BME280_CALIB_HUM_DIG_H4_MSB_REG                         0xE4
#define DRV_BME280_CALIB_HUM_DIG_H4_LSB_REG                         0xE5
#define DRV_BME280_CALIB_HUM_DIG_H5_MSB_REG                         0xE6
#define DRV_BME280_CALIB_HUM_DIG_H6_REG                             0xE7
/* Register Memory Map */    
#define DRV_BME280_REG_CHIP_ID                                      0xD0  
#define DRV_BME280_REG_RESET                                        0xE0  
#define DRV_BME280_REG_STATUS                                       0xF3  
#define DRV_BME280_REG_CTRL_MEAS                                    0xF4  
#define DRV_BME280_REG_CTRL_HUMIDITY                                0xF2  
#define DRV_BME280_REG_CONFIG                                       0xF5  
#define DRV_BME280_REG_PRESSURE_MSB                                 0xF7  
#define DRV_BME280_REG_PRESSURE_LSB                                 0xF8  
#define DRV_BME280_REG_PRESSURE_XLSB                                0xF9  
#define DRV_BME280_REG_TEMPERATURE_MSB                              0xFA  
#define DRV_BME280_REG_TEMPERATURE_LSB                              0xFB  
#define DRV_BME280_REG_TEMPERATURE_XLSB                             0xFC  
#define DRV_BME280_REG_HUMIDITY_MSB                                 0xFD  
#define DRV_BME280_REG_HUMIDITY_LSB                                 0xFE   

#define DRV_BME280_REG_DATA_ADDR                                    0xF7
#define DRV_BME280_REG_DATA_LEN                                     8
#define DRV_BME280_MODE_NORMAL                                      0x03

/* oversampling bits */
#define DRV_BME280_MODE_SAMPLING_P                                  0x04
#define DRV_BME280_MODE_SAMPLING_T                                  0x20
#define DRV_BME280_MODE_SAMPLING_H                                  0x01

/* Macro to combine two 8 bit data's to form a 16 bit data */
#define DRV_BME280_CONCAT_BYTES(msb, lsb) (((uint16_t)msb << 8) | (uint16_t)lsb)


/* Write buffer size, for the BME280 only address+2 bytes may be written
 * however we create a buffer large enough for the calibration data to
 * be read at initialization
 */
#define DRV_BME280_READ_BUFFER_SIZE     32
#define DRV_BME280_WRITE_BUFFER_SIZE    2

/* Definition of the BME280 compensation data structure */
typedef struct
{
    /* Temperature */
    uint16_t    dig_T1;
    int16_t     dig_T2;
    int16_t     dig_T3;
    
    /* Pressure */
    uint16_t    dig_P1;
    int16_t     dig_P2;
    int16_t     dig_P3;
    int16_t     dig_P4;
    int16_t     dig_P5;
    int16_t     dig_P6;
    int16_t     dig_P7;
    int16_t     dig_P8;
    int16_t     dig_P9;
    
    /* Humidity */
    uint8_t     dig_H1;
    int16_t     dig_H2;
    uint8_t     dig_H3;
    int16_t     dig_H4;
    int16_t     dig_H5;
    int8_t      dig_H6;  
    
    int32_t     t_fine;
} DRV_BME280_COMPENSATION_DATA;

/* Definition of BME280 uncompensated temperature, pressure and humidity */
typedef struct
{
    uint32_t            pressure;
    uint32_t            temperature;
    uint32_t            humidity;
} DRV_BME280_UNCOMP_DATA;

/* Definition of BME280 compensated temperature, pressure and humidity */
typedef struct
{
    uint32_t            pressure;
    int32_t             temperature;
    uint32_t            humidity;
} DRV_BME280_COMP_DATA;

/* Device states */
typedef enum
{
    DRV_BME280_TASK_INIT,
    DRV_BME280_TASK_READ_ID,
    DRV_BME280_TASK_READ_CALIBT,
    DRV_BME280_TASK_READ_CALIBP,
    DRV_BME280_TASK_READ_CALIBH1,
    DRV_BME280_TASK_READ_CALIBH2,
    DRV_BME280_TASK_POWERMODE_SET,
    DRV_BME280_SET_OVERSAMPLING1,
    DRV_BME280_TASK_READ,
    DRV_BME280_TASK_IDLE,
    DRV_BME280_TASK_ERROR         
} DRV_BME280_TASK_STATES;

// *****************************************************************************
/* DRV_BME280 Active Command

  Summary:
    Enumeration listing of the active command.

  Description:
    This enumeration defines the currently active command

  Remarks:
    None
*/

typedef enum
{
    /* Write command*/
    DRV_BME280_CMD_WRITE,

    /* Wait for EEPROM internal write complete command */
    DRV_BME280_CMD_WAIT_WRITE_COMPLETE,

    /* Read command */
    DRV_BME280_CMD_READ,

} DRV_BME280_CMD;


// *****************************************************************************
/* BME280 Client Instance Object

  Summary:
    Object used to keep any data required by a client of the BME280 driver.

  Description:
    None.

  Remarks:
    None.
*/
typedef struct
{
    bool                        inUse;
    DRV_BME280_APP_CALLBACK     callback;
    uintptr_t                   context;
    uint8_t                     drvIndex;
    DRV_BME280_CONFIG_PARAMS    configParams;
} DRV_BME280_CLIENT_OBJ;

// *****************************************************************************
/* BME280 Driver Instance Object

  Summary:
    Object used to keep any data required for an instance of the BME280 driver.

  Description:
    None.

  Remarks:
    None.
*/

typedef struct
{
    /* The status of the driver */
    SYS_STATUS                          status;
    
    /* Flag to indicate this object is in use  */
    bool                                inUse;

    /* Indicates the number of clients that have opened this driver */
    size_t                              nClients;

    /* Maximum number of clients */
    size_t                              nClientsMax;

    /* PLIB API list that will be used by the driver to access the hardware */
    const DRV_BME280_PLIB_INTERFACE*    plibInterface;
    
    /* the pool of clients and the current active client */
    DRV_BME280_CLIENT_OBJ*              clientObjPool;
    DRV_BME280_CLIENT_OBJ*              activeClient;
    
    /* read buffer, primarily used for Humidity, Temperature or
     * Pressure readings but also used for initial read of calibration
     * data
    */
    uint8_t                             readBuffer[DRV_BME280_READ_BUFFER_SIZE];
    /* write buffer to start readout requests */
    uint8_t                             writeBuffer[DRV_BME280_WRITE_BUFFER_SIZE];
    bool                                wrInProgress;

    /* config parameters for address and clock speed */
    DRV_BME280_CONFIG_PARAMS            configParams;
    
    /* Event for the event handler */
    DRV_BME280_EVENT                    event;  
    
    /* Current state of the driver */
    volatile DRV_BME280_TASK_STATES     taskState;
    
    /* compensation data */
    DRV_BME280_COMPENSATION_DATA        calibData;
    
    /* Device ID (should be 0x60) */
    uint8_t                             deviceID;
    
    /* uncompensated data */
    DRV_BME280_UNCOMP_DATA              uncompData;
    
    /* compensated data */
    DRV_BME280_COMP_DATA                compData;
    
} DRV_BME280_OBJ;


#endif //#ifndef _DRV_BME280C_LOCAL_H
