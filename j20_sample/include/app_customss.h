/**
 * @file app_customss.h
 * @brief Application-specific Bluetooth custom service server header file
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

#ifndef APP_CUSTOMSS_H
#define APP_CUSTOMSS_H

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
#include <gatt.h>
#include <common_gatt.h>
#include <common_gap.h>


/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

/* Custom service UUIDs and characteristics */

/* Custom service UUIDs */
#define CS_SVC_UUID                     { 0x24, 0xdc, 0x0e, 0x6e, 0x01, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHAR_TX_UUID                 { 0x24, 0xdc, 0x0e, 0x6e, 0x02, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHAR_RX_UUID                 { 0x24, 0xdc, 0x0e, 0x6e, 0x03, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHAR_LONG_TX_UUID            { 0x24, 0xdc, 0x0e, 0x6e, 0x04, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHAR_LONG_RX_UUID            { 0x24, 0xdc, 0x0e, 0x6e, 0x05, 0x40, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }

#define CS_BLT_SVC_UUID                 { 0x24, 0xdc, 0x0e, 0x6e, 0x01, 0x50, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHAR_LED_UUID                { 0x24, 0xdc, 0x0e, 0x6e, 0x03, 0x50, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }
#define CS_CHAR_BUTTON_UUID             { 0x24, 0xdc, 0x0e, 0x6e, 0x04, 0x50, \
                                          0xca, 0x9e, 0xe5, 0xa9, 0xa3, 0x00, \
                                          0xb5, 0xf3, 0x93, 0xe0 }

#define CS_VALUE_MAX_LENGTH          128 //20
#define CS_LONG_VALUE_MAX_LENGTH     128 //40
#define CS_LED_BUTTON_MAX_LENGTH     1
#define CS_TEMPERATURE_MAX_LENGTH    4

#define CS_TX_CHAR_NAME            "TX_VALUE"
#define CS_RX_CHAR_NAME            "RX_VALUE"
#define CS_TX_CHAR_LONG_NAME       "TX_VALUE_LONG"
#define CS_RX_CHAR_LONG_NAME       "RX_VALUE_LONG"
#define CS_TEMP_CHAR_NAME          "TEMPERATURE_VALUE"
#define CS_LED_CHAR_NAME           "LED_STATE"
#define CS_BUTTON_CHAR_NAME        "BUTTON_STATE"

#define CUSTOMSS_NOTIF_TIMEOUT_S            10

/* Uncomment to use indications in the RX_VALUE_LONG characteristic */
/* #define RX_VALUE_LONG_INDICATION */

/* Custom service ID */
/* Used in calculating attribute number for given custom service */
typedef enum
{
    CUST_SVC0,
    CUST_SVC1,
} cust_svc_id_t;

typedef enum
{
    /* Service 0 */
    CS_SERVICE0,

    /* TX Characteristic in Service 0 */
    CS_TX_VALUE_CHAR0,
    CS_TX_VALUE_VAL0,
    CS_TX_VALUE_CCC0,
    CS_TX_VALUE_USR_DSCP0,

    /* RX Characteristic in Service 0 */
    CS_RX_VALUE_CHAR0,
    CS_RX_VALUE_VAL0,
    CS_RX_VALUE_CCC0,
    CS_RX_VALUE_USR_DSCP0,

    /* TX Long Characteristic in Service 0 */
    CS_TX_LONG_VALUE_CHAR0,
    CS_TX_LONG_VALUE_VAL0,
    CS_TX_LONG_VALUE_CCC0,
    CS_TX_LONG_VALUE_USR_DSCP0,

    /* RX Long Characteristic in Service 0 */
    CS_RX_LONG_VALUE_CHAR0,
    CS_RX_LONG_VALUE_VAL0,
    CS_RX_LONG_VALUE_CCC0,
    CS_RX_LONG_VALUE_USR_DSCP0,

     /* Max number of services and characteristics */
    CS_NB0,
} cs0_att_t;

typedef enum
{
    /* Service 1 */
    CS_SERVICE1,

    /* LED Characteristic in Service 1 */
    CS_LED_VALUE_CHAR1,
    CS_LED_VALUE_VAL1,
    CS_LED_VALUE_CCC1,
    CS_LED_VALUE_USR_DSCP1,

    /* Button Characteristic in Service 1 */
    CS_BUTTON_VALUE_CHAR1,
    CS_BUTTON_VALUE_VAL1,
    CS_BUTTON_VALUE_CCC1,
    CS_BUTTON_VALUE_USR_DSCP1,

    /* Max number of services and characteristics */
    CS_NB1,
} cs1_att_t;

typedef enum
{
    CS_BUTTON_SHORT_PRESS     = 0,
    CS_BUTTON_MEDIUM_PRESS    = 1,
    CS_BUTTON_LONG_PRESS      = 2,
    CS_BUTTON_SUPERLONG_PRESS = 3,
    CS_BUTTON_DOUBLE_PRESS    = 4,
    CS_BUTTON_TRIPLE_PRESS    = 5
} CS_ButtonPressType_t;

typedef struct
{
    /* Preferred MTU for attribute exchange */
    uint16_t pref_mtu;

    /*User attribute priority level */
    uint8_t  prio_level;

    /* GATT user local identifier for custom service */
    uint8_t  user_lid;

    /* To BLE transfer buffer */
    uint8_t  to_air_buffer[CS_VALUE_MAX_LENGTH];
    uint8_t  to_air_cccd_value[2];

    /* From BLE transfer buffer */
    uint8_t  from_air_buffer[CS_VALUE_MAX_LENGTH];
    uint8_t  from_air_cccd_value[2];

    /* To BLE long transfer buffer */
    uint8_t  to_air_buffer_long[CS_LONG_VALUE_MAX_LENGTH];
    uint8_t  to_air_cccd_value_long[2];

    /* From BLE long transfer buffer */
    uint8_t  from_air_buffer_long[CS_LONG_VALUE_MAX_LENGTH];
    uint8_t  from_air_cccd_value_long[2];

    /* From BLE transfer buffer */
    uint8_t  led_from_air_buffer[CS_LED_BUTTON_MAX_LENGTH];
    uint8_t  led_from_air_cccd_value[2];

    /* To BLE Button transfer buffer */
    uint8_t  button_to_air_buffer[CS_LED_BUTTON_MAX_LENGTH];
    uint8_t  button_to_air_cccd_value[2];

    co_timer_periodic_t notif_timer;
    co_timer_t button_timer;

    uint8_t  rx_changed;
} app_env_tag_cs_t;

/* ----------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/
void AppCustomSS_Init(uint32_t notif_timeout);

uint16_t AppCustomSS_GATTUserAndHandlerRegister(void);

uint16_t AppCustomSS_AddService(uint8_t cs_svc_id);

void AppCustomSS_UpdateButtonAttribute(CS_ButtonPressType_t press_type);

uint16_t AppCustomSS_RXLongCharCallback(uint8_t conidx, uint16_t attidx, uint16_t handle,
                                        co_buf_t* to, uint8_t* from,
                                        common_gatt_srv_op_t op, uint16_t length,
                                        uint16_t offset, uint16_t hl_status);

uint16_t AppCustomSS_RxCharCallback(uint8_t conidx, uint16_t attidx, uint16_t handle,
                                    co_buf_t* to, uint8_t* from,
                                    common_gatt_srv_op_t op, uint16_t length,
                                    uint16_t offset, uint16_t hl_status);

uint16_t AppCustomSS_LEDCharCallback(uint8_t conidx, uint16_t attidx, uint16_t handle,
                                    co_buf_t* to, uint8_t* from,
                                    common_gatt_srv_op_t op, uint16_t length,
                                    uint16_t offset, uint16_t hl_status);

void AppCustomSS_NotifOnTimeout(co_timer_periodic_t* p_timer);

void AppCustomSS_ButtonNotifOnTimeout(co_timer_t* p_timer);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_CUSTOMSS_H */
