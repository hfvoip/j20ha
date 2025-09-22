/**
 * @file app.h
 * @brief Main application header
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

#ifndef APP_BT_H
#define APP_BT_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */


/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/
/* Device and libraries headers */
#include <hw.h>
#include <swmTrace_api.h>
#include <MRAM_rom.h>

/* Application headers */
#include <app_init.h>
#include <device_init.h>
#include <bt_protocol_support.h>
#include <common_gap.h>
#include <common_gatt.h>
#include <app_handlers.h>
#include <app_bass.h>
#include <app_batt.h>
#include <app_diss.h>
#include <app_customss.h>
#include <button_mgr_user_cfg.h>
#include <LED_mgr_user_cfg.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
/* Advertising extension
 * Set this define to 1 to enable advertisement extension */

#define ADV_EXTENSION                   0

#define APP_BT_DEV_PARAM_SOURCE        APP_PROVIDED /* or APP_PROVIDED FLASH_PROVIDED_or_DFLT */

/* Advertising channel map - 37, 38, 39 */
#define APP_ADV_CHMAP                   GAPM_DEFAULT_ADV_CHMAP

/* Advertising minimum interval - 40ms (64 * 0.625ms) */
#define APP_ADV_INT_MIN                 GAPM_DEFAULT_ADV_INTV_MIN

/* Advertising maximum interval - 40ms (64*0.625ms) */
#define APP_ADV_INT_MAX                 GAPM_DEFAULT_ADV_INTV_MAX

#define GAPM_ADDRESS_TYPE               GAPM_CFG_ADDR_PUBLIC

#define GAPM_PRIVACY_TYPE               GAPM_CFG_CONTROLLER_PRIVACY

#define APP_BD_RENEW_DUR                150 /* in seconds */

#define GAPM_OWN_ADDR_TYPE              GAPM_STATIC_ADDR /* GAPM_GEN_RSLV_ADDR for Host privacy */

/* BT private address of local device */
#define APP_BT_PRIVATE_ADDR            {0x00, 0x02, 0x4C, 0x53, 0x52, 0xDE} /* TODO: This is a test address. Change it! */

#define SECURE_CONNECTION               1  /* set 0 for LEGACY_CONNECTION or 1 for SECURE_CONNECTION */

/* The number of standard profiles and custom services added in this application */
#define APP_NUM_STD_PRF                 2
#define APP_NUM_CUST_SVC              	2

/* RF Oscillator accuracy in ppm */
#define RADIO_CLOCK_ACCURACY            20

#define LOW_POWER_CLOCK_ACCURACY        500     /* ppm */
#define TWOSC                           1200    /* us */

#define MAX_SLEEP_DURATION              0x17700 /* 30s */

#define CONCAT(x, y)                    x##y
#define GPIO_SRC(x)                     CONCAT(GPIO_SRC_GPIO_, x)

/* Set UART peripheral clock */
#define UART_CLK                        8000000

/* Set sensor clock */
#define SENSOR_CLK                      32768

/* Set user clock */
#define USER_CLK                        1000000

/* Enable/disable buck converter
 * Options: VCC_BUCK or VCC_LDO
 */
#define VCC_BUCK_LDO_CTRL               VCC_BUCK

/* GPIO number that is used for easy re-flashing (recovery mode) */
#define RECOVERY_GPIO                   0

/* Default LSAD channel for SetTxPower */
#define LSAD_TXPWR_DEF                  1

/* Timer setting in units of 1ms (kernel timer resolution) */
#define TIMER_SETTING_MS(MS)            MS
#define TIMER_SETTING_S(S)              (S * 1000)

/* Advertising data is composed by device name and company id */
#define APP_DEVICE_NAME                 "rsl20_ble_peripheral"
#define APP_DEVICE_NAME_LEN             sizeof(APP_DEVICE_NAME)-1

/* Manufacturer info (onsemi Company ID) */
#define APP_COMPANY_ID                  { 0x62, 0x3 }
#define APP_COMPANY_ID_LEN              2

#define APP_DEVICE_APPEARANCE           0
#define APP_PREF_SLV_MIN_CON_INTERVAL   8
#define APP_PREF_SLV_MAX_CON_INTERVAL   10
#define APP_PREF_SLV_LATENCY            0
#define APP_PREF_SLV_SUP_TIMEOUT        200

/* Minimum required key size for pairing, if supported by controller */
#define APP_PAIRING_MIN_REQ_KEY_SIZE    7

/* Application-provided IRK */
#define APP_IRK                         { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

/* Application-provided CSRK */
#define APP_CSRK                        { 0x01, 0x23, 0x45, 0x68, 0x78, 0x9a, \
                                          0xbc, 0xde, 0x01, 0x23, 0x45, 0x68, \
                                          0x78, 0x9a, 0xbc, 0xde }

#define APP_SECURELE_KEYS_FORCED        1

/* Application-provided private key */
#define APP_PRIVATE_KEY                 { 0xEC, 0x89, 0x3C, 0x11, 0xBB, 0x2E, \
                                          0xEB, 0x5C, 0x80, 0x88, 0x63, 0x57, \
                                          0xCC, 0xE2, 0x05, 0x17, 0x20, 0x75, \
                                          0x5A, 0x26, 0x3E, 0x8D, 0xCF, 0x26, \
                                          0x63, 0x1D, 0x26, 0x0B, 0xCE, 0x4D, \
                                          0x9E, 0x07 }

/* Application-provided public key */
#define APP_PUBLIC_KEY                  { 0x56, 0x09, 0x79, 0x1D, 0x5A, 0x5F, \
                                          0x4A, 0x5C, 0xFE, 0x89, 0x56, 0xEC, \
                                          0xE6, 0xF7, 0x92, 0x21, 0xAC, 0x93, \
                                          0x99, 0x10, 0x51, 0x82, 0xF4, 0xDD, \
                                          0x84, 0x07, 0x50, 0x99, 0xE7, 0xC2, \
                                          0xF1, 0xC8, \
                                          0x40, 0x84, 0xB4, 0xA6, 0x08, 0x67, \
                                          0xFD, 0xAC, 0x81, 0x5D, 0xB0, 0x41, \
                                          0x27, 0x75, 0x9B, 0xA7, 0x92, 0x57, \
                                          0x0C, 0x44, 0xB1, 0x57, 0x7C, 0x76, \
                                          0x5B, 0x56, 0xF0, 0xBA, 0x03, 0xF4, \
                                          0xAA, 0x67}


/* Application communicates PHY update request, comment out if is not required */
/* #define APP_PHY_UPDATE_REQ */

/* Based on enum gap_phy */
#define APP_PREFERRED_PHY_RX            GAP_PHY_LE_CODED
#define APP_PREFERRED_PHY_TX            GAP_PHY_LE_CODED

/* Based on enum gapc_phy_option */
#define APP_PREFERRED_CODED_PHY_RATE    GAPC_PHY_OPT_LE_CODED_125K_RATE

/* The GPIO pin to use for TX when using the UART mode */
#define UART_TX_GPIO    				(6)

/* The GPIO pin to use for RX when using the UART mode */
#define UART_RX_GPIO    				(5)

/* The selected baud rate for the application when using UART mode */
#define UART_BAUD       				(921600)

#if (BUTTON_MANAGER_ENABLED == 1)
    /* The GPIO pin to use for the BUTTON_STATE button */
    #define BUTTON_GPIO                     BUTTON_USER_GPIO_0_NUM
    #define BUTTON_GPIO_INDEX               BUTTON_USER_GPIO_0_INDEX
    #define BUTTON_DEBOUNCE_COUNT               25
#else
    #if (RSL20_CID == 101)
        /* The GPIO pin to use for the BUTTON_STATE button */
        #define BUTTON_GPIO                     0
    #else /* !(RSL20_CID == 101) */
        /* The GPIO pin to use for the BUTTON_STATE button */
        #define BUTTON_GPIO                     7
    #endif /* !(RSL20_CID == 101) */
#endif /* (BUTTON_MANAGER_ENABLED == 1) */

#if (LED_MANAGER_ENABLED == 1)
    /* The GPIO pin to use for the LED_STATE LED */
    #define LED_STATE_GPIO                  LED_USER_GPIO_0_NUM
    #define LED_STATE_GPIO_INDEX            LED_USER_GPIO_0_INDEX
    /* The GPIO pin to use for the CONNECTION_STATE LED */
    #define CONNECTION_STATE_GPIO           LED_USER_GPIO_1_NUM
    #define CONNECTION_STATE_GPIO_INDEX     LED_USER_GPIO_1_INDEX
#else
    #if (RSL20_CID == 101)
        /* The GPIO pin to use for the LED_STATE LED */
        #define LED_STATE_GPIO                  8
        /* The GPIO pin to use for the CONNECTION_STATE LED */
        #define CONNECTION_STATE_GPIO           10
    #else /* !(RSL20_CID == 101) */
        /* The GPIO pin to use for the LED_STATE LED */
        #define LED_STATE_GPIO                  11
        /* The GPIO pin to use for the CONNECTION_STATE LED */
        #define CONNECTION_STATE_GPIO           4
    #endif /* !(RSL20_CID == 101) */
#endif /* (LED_MANAGER_ENABLED == 1) */
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_BT_H */

