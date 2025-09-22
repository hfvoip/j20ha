/**
 * @file app_diss.h
 * @brief Bluetooth device information service server header
 *
 *
 * @copyright @parblock
 * Copyright (c) 2023 Semiconductor Components Industries, LLC (d/b/a
 * onsemi), All Rights Reserved
 *
 * This code is the property of onsemi and may not be redistributed
 * in any form without prior written permission from onsemi.
 * The terms of use and warranty for this code are covered by contractual
 * agreements between onsemi and the licensee.
 *
 * This is Reusable Code.
 * @endparblock
 */

#ifndef APP_DISS_H_
#define APP_DISS_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "rwip_config.h"     // SW Configuration
#include "diss_msg.h"
#include <stdint.h>
#include "co_version.h"

/* ----------------------------------------------------------------------------
 * Global variables and types
 * --------------------------------------------------------------------------*/

/* Global DISS Specific Info Structure */
typedef struct diss_device_info_field
{
    uint8_t len;
    uint8_t *data;
}diss_device_info_field_t;

/* Global DISS Info Compilation Structure */
typedef struct diss_device_info
{
    diss_device_info_field_t MANUFACTURER_NAME;
    diss_device_info_field_t MODEL_NB_STR;
    diss_device_info_field_t SERIAL_NB_STR;
    diss_device_info_field_t FIRM_REV_STR;
    diss_device_info_field_t SYSTEM_ID;
    diss_device_info_field_t HARD_REV_STR;
    diss_device_info_field_t SW_REV_STR;
    diss_device_info_field_t IEEE;
    diss_device_info_field_t PNP;
}diss_device_info_t;

/**
 * DISS state structure
 */
typedef struct
{
    /** DISS features that will be enabled. See \ref diss_msg.h */
    uint16_t features;

    /** The flag that indicates that service has been added */
    bool serviceAdded;

    /** DISS Device Information compilation structure arm*/
    const diss_device_info_t *deviceInfo;
} diss_env_t;

/* --------------------------------------------------------------------------
 *  Device Information used for Device Information Server Service (DISS)
 * ----------------------------------------------------------------------- */

/* Manufacturer Name Value */
#define APP_DIS_MANUFACTURER_NAME       ("onsemi")
#define APP_DIS_MANUFACTURER_NAME_LEN   (sizeof(APP_DIS_MANUFACTURER_NAME) - 1)

/* Model Number String Value */
#define APP_DIS_MODEL_NB_STR            ("RSL20")
#define APP_DIS_MODEL_NB_STR_LEN        (sizeof(APP_DIS_MODEL_NB_STR) - 1)

/* Serial Number */
#define APP_DIS_SERIAL_NB_STR           ("1.0.0.0-LE")
#define APP_DIS_SERIAL_NB_STR_LEN       (sizeof(APP_DIS_SERIAL_NB_STR) - 1)

/* Firmware Revision */
#define APP_DIS_FIRM_REV_STR            ("12.0.18")
#define APP_DIS_FIRM_REV_STR_LEN        (sizeof(APP_DIS_FIRM_REV_STR) - 1)

/* System ID Value - LSB -> MSB */
#define APP_DIS_SYSTEM_ID               ("\x12\x34\x56\xFF\xFE\x9A\xBC\xDE")
#define APP_DIS_SYSTEM_ID_LEN           (sizeof(APP_DIS_SYSTEM_ID) - 1)

/* Hardware Revision String */
#define APP_DIS_HARD_REV_STR            ("1.0.0")
#define APP_DIS_HARD_REV_STR_LEN        (sizeof(APP_DIS_HARD_REV_STR) - 1)

/* Software Revision String */
#define APP_DIS_SW_REV_STR              ("1.0")
#define APP_DIS_SW_REV_STR_LEN          (sizeof(APP_DIS_SW_REV_STR) - 1)

/* IEEE */
#define APP_DIS_IEEE                    ("\xFF\xEE\xDD\xCC\xBB\xAA")
#define APP_DIS_IEEE_LEN                (sizeof(APP_DIS_IEEE) - 1)

/**
 * PNP ID Value - LSB -> MSB
 *      Vendor ID Source : 0x02 (USB Implementer's Forum assigned Vendor ID value)
 *      Vendor ID : 0x1057      (onsemi)
 *      Product ID : 0x0040
 *      Product Version : 0x0300
 */

#define APP_DIS_PNP_ID               ("\x02\x57\x10\x40\x00\x00\x03")
#define APP_DIS_PNP_ID_LEN           (7)
#define APP_DIS_FEATURES             (DIS_ALL_FEAT_SUP)


/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */

void AppDISS_DeviceInfoValueReqInd_Cb(uint32_t token, uint8_t val_id);

void AppDISS_Initialize(uint16_t features, const diss_device_info_t *deviceInfo);

uint16_t AppDISS_AddProfile(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif //APP_DISS_H_
