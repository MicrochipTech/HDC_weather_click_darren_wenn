/*******************************************************************************
  BME280 Driver Definitions Header File

  Company:
    Microchip Technology Inc.

  File Name:
    drv_bme280_definitions.h

  Summary:
    BME280 Driver Definitions Header File

  Description:
    This file provides implementation-specific definitions for the BME280
    driver's system interface.
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

#ifndef DRV_BME280_DEFINITIONS_H
#define DRV_BME280_DEFINITIONS_H

// *****************************************************************************
// *****************************************************************************
// Section: File includes
// *****************************************************************************
// *****************************************************************************

#include <device.h>

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
       
typedef enum
{
    DRV_BME280_ERROR_NONE = 0,
    DRV_BME280_ERROR_NACK,
    DRV_BME280_ERROR_BUS,
} DRV_BME280_ERROR;

typedef enum
{
    DRV_BME280_EVENT_READ_TEMP_DONE = 0,
    DRV_BME280_EVENT_WRITE_DONE,
    DRV_BME280_EVENT_READ_DONE,
    DRV_BME280_EVENT_ERROR,
} DRV_BME280_EVENT;

typedef struct
{
    uint32_t clockSpeed;
} DRV_BME280_TRANSFER_SETUP;

typedef struct
{
    uint8_t                         sensorAddr;
    DRV_BME280_TRANSFER_SETUP       transferParams;
} DRV_BME280_CONFIG_PARAMS;

typedef void (* DRV_BME280_PLIB_CALLBACK)( uintptr_t );

typedef bool (* DRV_BME280_PLIB_WRITE_READ)(uint16_t , uint8_t* , uint32_t , uint8_t* , uint32_t);

typedef bool (* DRV_BME280_PLIB_WRITE)(uint16_t , uint8_t* , uint32_t );

typedef bool (* DRV_BME280_PLIB_READ)(uint16_t , uint8_t* , uint32_t);

typedef bool (* DRV_BME280_PLIB_IS_BUSY)(void);

typedef DRV_BME280_ERROR (* DRV_BME280_PLIB_ERROR_GET)(void);

typedef void (* DRV_BME280_PLIB_CALLBACK_REGISTER)(DRV_BME280_PLIB_CALLBACK, uintptr_t);

typedef bool (* DRV_BME280_PLIB_TRANSFER_SETUP)(DRV_BME280_TRANSFER_SETUP*, uint32_t);


// *****************************************************************************
/* BME280 Driver PLIB Interface Data

  Summary:
    Defines the data required to initialize the BME280 driver PLIB Interface.

  Description:
    This data type defines the data required to initialize the BME280 driver
    PLIB Interface.

  Remarks:
    None.
*/

typedef struct
{
    /* BME280 PLIB writeRead API */
    DRV_BME280_PLIB_WRITE_READ              writeRead;

    /* BME280 PLIB write API */
    DRV_BME280_PLIB_WRITE                   write;

    /* BME280 PLIB read API */
    DRV_BME280_PLIB_READ                    read;

    /* BME280 PLIB Error get API */
    DRV_BME280_PLIB_ERROR_GET               errorGet;

    /* BME280 PLIB callback register API */
    DRV_BME280_PLIB_CALLBACK_REGISTER       callbackRegister;

    /* BME280 PLIB Transfer setup API*/
    DRV_BME280_PLIB_TRANSFER_SETUP          transferSetup;    
} DRV_BME280_PLIB_INTERFACE;

// *****************************************************************************
/* BME280 Driver Initialization Data

  Summary:
    Defines the data required to initialize the BME280 driver

  Description:
    This data type defines the data required to initialize or the BME280 driver.

  Remarks:
    None.
*/

typedef struct
{
    /* Identifies the PLIB API set to be used by the driver to access the
     * peripheral. */
    const DRV_BME280_PLIB_INTERFACE*    plibInterface;

    /* Config parameters */
    DRV_BME280_CONFIG_PARAMS            configParams;
    
    /* client object pool */
    const uintptr_t                     clientObjPool;

    /* Number of clients */
    size_t                              maxClients;
} DRV_BME280_INIT;


//DOM-IGNORE-BEGIN
#ifdef __cplusplus
}
#endif
//DOM-IGNORE-END

#endif // #ifndef DRV_BME280_DEFINITIONS_H

/*******************************************************************************
 End of File
*/
