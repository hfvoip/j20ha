/**
 * @file app_handlers.c
 * @brief Application specific action handlers
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

#include <app_bt.h>
#include "bt_config.h"

#if (LED_MANAGER_ENABLED == 1)
    /* Defines durations for LED on/off internals, in milliseconds */
    #define ON_PERIOD 200
    #define OFF_PERIOD 200
    #define OFF_2_SECONDS 2000

    /* Configuration of LED blink patterns for each possible number of active connections */
    #if (APP_MAX_NB_CON > 1)
        #define PATTERN_1_CONN_NUM_ELEMENTS 2
uint16_t LED_pattern_1_conn[PATTERN_1_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 1) */

    #if (APP_MAX_NB_CON > 2)
        #define PATTERN_2_CONN_NUM_ELEMENTS 4
uint16_t LED_pattern_2_conn[PATTERN_2_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 2) */

    #if (APP_MAX_NB_CON > 3)
        #define PATTERN_3_CONN_NUM_ELEMENTS 6
uint16_t LED_pattern_3_conn[PATTERN_3_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 3) */

    #if (APP_MAX_NB_CON > 4)
        #define PATTERN_4_CONN_NUM_ELEMENTS 8
uint16_t LED_pattern_4_conn[PATTERN_4_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 4) */

    #if (APP_MAX_NB_CON > 5)
        #define PATTERN_5_CONN_NUM_ELEMENTS 10
uint16_t LED_pattern_5_conn[PATTERN_5_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 5) */

    #if (APP_MAX_NB_CON > 6)
        #define PATTERN_6_CONN_NUM_ELEMENTS 12
uint16_t LED_pattern_6_conn[PATTERN_6_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 6) */

    #if (APP_MAX_NB_CON > 7)
        #define PATTERN_7_CONN_NUM_ELEMENTS 14
uint16_t LED_pattern_7_conn[PATTERN_7_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 7) */

    #if (APP_MAX_NB_CON > 8)
        #define PATTERN_8_CONN_NUM_ELEMENTS 16
uint16_t LED_pattern_8_conn[PATTERN_8_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 8) */

    #if (APP_MAX_NB_CON > 9)
        #define PATTERN_9_CONN_NUM_ELEMENTS 18
uint16_t LED_pattern_9_conn[PATTERN_9_CONN_NUM_ELEMENTS] =
{
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_PERIOD,
    ON_PERIOD,
    OFF_2_SECONDS
};
    #endif /* #if (APP_MAX_NB_CON > 9) */
#endif /* (LED_MANAGER_ENABLED == 1) */

extern gapm_le_adv_cb_actv_t app_le_adv_cbs;
extern gapc_connection_req_cb_t app_conn_req_cbs;
extern gapc_security_cb_t app_sec_cbs;
extern gapc_connection_info_cb_t app_conn_info_cbs;
extern gapc_le_config_cb_t app_le_config_cbs;

gapm_le_adv_create_param_t advParam =
{
    .prop                  = GAPM_ADV_PROP_UNDIR_CONN_MASK,
    .disc_mode             = GAPM_ADV_MODE_GEN_DISC ,
    .max_tx_pwr            = DEF_TX_POWER,
    .filter_pol            = ADV_ALLOW_SCAN_ANY_CON_ANY,
    .prim_cfg.adv_intv_min = APP_ADV_INT_MIN,
    .prim_cfg.adv_intv_max = APP_ADV_INT_MAX,
    .prim_cfg.ch_map       = GAPM_DEFAULT_ADV_CHMAP,
#if ADV_EXTENSION == 1
    .prim_cfg.phy          = GAPM_PHY_TYPE_LE_CODED,
#else
    .prim_cfg.phy          = GAPM_PHY_TYPE_LE_1M
#endif
};

const gapc_le_preferred_periph_param_t periphPrefParam =
{
    .con_intv_min  = APP_PREF_SLV_MIN_CON_INTERVAL,
    .con_intv_max  = APP_PREF_SLV_MAX_CON_INTERVAL,
    .latency       = APP_PREF_SLV_LATENCY,
    .conn_timeout  = APP_PREF_SLV_SUP_TIMEOUT
};

gapc_pairing_t pairingInfo =
{
    .iocap = GAP_IO_CAP_NO_INPUT_NO_OUTPUT,
    .oob   = GAP_OOB_AUTH_DATA_NOT_PRESENT,

#if SECURE_CONNECTION
    .auth = GAP_AUTH_REQ_SEC_CON_BOND,
#else
    .auth = GAP_AUTH_REQ_NO_MITM_BOND,
#endif /* SECURE_CONNECTION */

    .key_size  = KEY_LEN,
    .ikey_dist = (GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
    .rkey_dist = (GAP_KDIST_ENCKEY | GAP_KDIST_IDKEY | GAP_KDIST_SIGNKEY),
};

activity_status_t advActivityStatus;

uint16_t gError;

static co_timer_t batt_rd_timer;

cust_svc_desc_t app_cust_svc_db[APP_NUM_CUST_SVC];

/* --------------------------------------------------------------------------------------------------
 * Error Handling
 * ------------------------------------------------------------------------------------------------*/
#if 0
void App_Err_Ind(uint16_t error)
{
    gError = error;
    // If code execution reached here, app was unsuccessful.
    // The failure code is available in gError.
    while (1)
    {
        // Refresh the watchdog timers
        SYS_WATCHDOG_REFRESH();
    }
}
#endif /* if 0 */

/* --------------------------------------------------------------------------------------------------
 * Advertising activity management
 * ------------------------------------------------------------------------------------------------*/
void App_AdvStopped(uint32_t metainfo, uint8_t actv_idx, uint16_t reason)
{
    /* The advertising activity is stopped upon receiving
     * connection request. Restart advertising if not connected to
     * maximum number of peers configured for this application
     */

    if(CommonGAP_ConnectionCountGet() < APP_MAX_NB_CON)
    {
        swmLogInfo("    Advertisement Activity Stopped. Restarting advertising...\r\n");

        if(advActivityStatus.state == ACTIVITY_STATE_NOT_STARTED)
        {
            CommonGAP_StartAdvActv(actv_idx, 0, 0);
        }
    }
    else if(CommonGAP_ConnectionCountGet() == APP_MAX_NB_CON)
    {
#if (LED_MANAGER_ENABLED == 1)
        /* When the number of active connections is the maximum value, keep the LED on */
        LED_TurnOn(CONNECTION_STATE_GPIO_INDEX);
#endif /* (LED_MANAGER_ENABLED == 1) */
    }
    else
    {
        /* Do nothing */
    }
}

void App_AdvProcCmp(uint32_t metainfo, uint8_t proc_id, uint8_t actv_idx, uint16_t status)
{
    if(proc_id == GAPM_ACTV_START)
    {
#if (LED_MANAGER_ENABLED == 1)
        /* Once the advertising activity has started, blink an LED to indicate the number of connections */
        App_LEDBlinkNumConnections();
#endif /* (LED_MANAGER_ENABLED == 1) */
    }
}

void App_LEDBlinkNumConnections(void)
{
#if (LED_MANAGER_ENABLED == 1)
    /* Function to check the number of active connections and set the LED blink pattern accordingly */

    uint8_t num_connections = CommonGAP_ConnectionCountGet();
    switch(num_connections)
    {

    /* No active connections, blink on/off in 200ms intervals */
    case 0:
    {
        LED_FastBlink(CONNECTION_STATE_GPIO_INDEX, LED_PATTERN_INFINITE_REPETITIONS);
    }
    break;

    /*
     * If the number of connections is greater than 0 and less than APP_MAX_NB_CON,
     * blink for 200ms once per active connection, then stay off for 2 seconds before repeating
     */
    case 1:
    {
    #if (APP_MAX_NB_CON > 1)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_1_conn,
                        PATTERN_1_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 1) */
    }
    break;

    case 2:
    {
    #if (APP_MAX_NB_CON > 2)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_2_conn,
                        PATTERN_2_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 2) */
    }
    break;

    case 3:
    {
    #if (APP_MAX_NB_CON > 3)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_3_conn,
                        PATTERN_3_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 3) */
    }
    break;

    case 4:
    {
    #if (APP_MAX_NB_CON > 4)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_4_conn,
                        PATTERN_4_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 4) */
    }
    break;

    case 5:
    {
    #if (APP_MAX_NB_CON > 5)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_5_conn,
                        PATTERN_5_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 5) */
    }
    break;

    case 6:
    {
    #if (APP_MAX_NB_CON > 6)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_6_conn,
                        PATTERN_6_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 6) */
    }
    break;

    case 7:
    {
    #if (APP_MAX_NB_CON > 7)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_7_conn,
                        PATTERN_7_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 7) */
    }
    break;

    case 8:
    {
    #if (APP_MAX_NB_CON > 8)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_8_conn,
                        PATTERN_8_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 8) */
    }
    break;

    case 9:
    {
    #if (APP_MAX_NB_CON > 9)
        LED_CustomBlink(CONNECTION_STATE_GPIO_INDEX,
                        LED_pattern_9_conn,
                        PATTERN_9_CONN_NUM_ELEMENTS,
                        LED_PATTERN_INFINITE_REPETITIONS);
    #endif /* #if (APP_MAX_NB_CON > 9) */
    }
    break;

    /* The maximum possible value of APP_MAX_NB_CON is 10, so no other cases should be applicable */
    default:
    {
        /* Do nothing */
    }
    break;

    }
#endif /* (LED_MANAGER_ENABLED == 1) */
}

/* --------------------------------------------------------------------------------------------------
 * Connection Establishment/Address Resolution
 * ------------------------------------------------------------------------------------------------*/
void App_LEConnectionReq(uint8_t conidx, uint32_t metainfo,
                                uint8_t actv_idx, uint8_t role,
                                const gap_bdaddr_t* p_peer_addr,
                                const gapc_le_con_param_t* p_con_params,
                                uint8_t clk_accuracy)
{
    uint8_t i;

     /* Ask the stack to resolve the address with the IRKs we have in our bond list.
      * In case of success, the stack returns GAPM_ADDR_SOLVED_IND.
      * If not successful (not bonded previously)
      * the stack returns GAPM_CMP_EVT /GAPM_RESOLV_ADDR
      * with status GAP_ERR_NOT_FOUND (see below). If previously bonded,
      * share the bond data as the last param to the call below
      */

    /* Connection request message report peer addr if required */
    swmLogInfo("    Create Connection conidx=%u with : ", conidx);

    for(i = (GAP_BD_ADDR_LEN - 1); i > 0; i--)
    {
        swmTrace_printf("%02x:", p_peer_addr->addr[i]);
    }

    swmTrace_printf("%02x\r\n", p_peer_addr->addr[i]);

    /* Once connected, send a request to the battery server
     * to enable battery service */
    uint8_t old_batt_lvl[BASS_NB_BAS_INSTANCES_MAX] = {0, 0};

    AppBASS_Enable(conidx, PRF_CLI_START_NTF | PRF_CLI_START_IND, old_batt_lvl);

}

/* --------------------------------------------------------------------------------------------------
 * Connection Security
 * ------------------------------------------------------------------------------------------------*/
void App_PairingInfoRequest(uint8_t conidx, uint32_t metainfo, uint8_t exp_info)
{
    uint16_t status = GAP_ERR_NO_ERROR;

    switch(exp_info)
    {
        case GAPC_INFO_TK_OOB:
        case GAPC_INFO_TK_DISPLAYED:
        case GAPC_INFO_TK_ENTERED:
        {
            /* IO Capabilities are set to GAP_IO_CAP_NO_INPUT_NO_OUTPUT in this application.
             * Therefore TK exchange is NOT performed. It is always set to 0 (Just Works algorithm). */

            swmLogInfo("    Peer requesting TK...\r\n");

        }
        break;

        case  GAPC_INFO_IRK:
        {
            gap_sec_key_t irkExch;
            memcpy(irkExch.key, CommonGAP_GetDeviceConfig()->irk.key, GAP_KEY_LEN);
            status = gapc_le_pairing_provide_irk (conidx, &irkExch);

            swmLogInfo("    Provide IRK to requesting peer. status = 0x%x...\r\n", status);

        }
        break;

        case  GAPC_INFO_CSRK:
        {
            gap_sec_key_t csrkExch = APP_CSRK;
            status = gapc_pairing_provide_csrk(conidx, &csrkExch);
            swmLogInfo("    Provide CSRK to requesting peer. status = 0x%x...\r\n", status);
        }
        break;

        default:
        {
            /* Ignore */
        }
        break;
    }
}

void App_NumericCompareReq(uint8_t conidx, uint32_t metainfo, uint32_t value)
{
    bool accept = true;

    swmLogInfo("    PIN %u from conidx = %u...\r\n", value, conidx);

    gapc_pairing_numeric_compare_rsp(conidx, accept);
}

void App_PairingReq(uint8_t conidx, uint32_t metainfo, uint8_t auth_level)
{
    bool accept = BondList_Size() < BONDLIST_MAX_SIZE;

    if(!accept)
    {
        swmLogWarn("    Cannot save pairing info. Memory full!\r\n");
    }

#if SECURE_CONNECTION
    uint8_t security_required = GAP_SEC1_NOAUTH_PAIR_ENC;
#else
    uint8_t security_required = GAP_NO_SEC;
#endif

    gapm_le_configure_security_level(security_required);
    gapc_le_pairing_accept(conidx, accept, &pairingInfo, 0);

    swmLogInfo("    Pairing request from peer conidx=%u, %s\r\n",
                    conidx, (accept ? "ACCEPTED" : "REJECTED"));
}

/* --------------------------------------------------------------------------------------------------
 * Connection Information
 * ------------------------------------------------------------------------------------------------*/
void App_ConnDisconnected(uint8_t conidx, uint32_t metainfo, uint16_t reason)
{
    swmLogInfo("    Disconnect Indication reason = 0x%x... \r\n", reason);

    /*
     * Whenever the device's active connection count is less than APP_MAX_NB_CON, it will be
     * advertising, and when the active connection count reaches APP_MAX_NB_CON, it will stop
     * advertising; thus, advertising must be restarted when the number of connections decrements from
     * APP_MAX_NB_CON to (APP_MAX_NB_CON - 1)
     */
    if(CommonGAP_ConnectionCountGet() == (APP_MAX_NB_CON - 1))
    {
        swmLogInfo("    Restarting advertising...\r\n");
        CommonGAP_StartAdvActv(advActivityStatus.actv_idx, 0, 0);
    }
    else
    {
#if (LED_MANAGER_ENABLED == 1)
        /* Update LED blink pattern based on new number of connections */
        App_LEDBlinkNumConnections();
#endif /* (LED_MANAGER_ENABLED == 1) */
    }
}

void App_NameReq(uint8_t conidx, uint32_t metainfo, uint16_t token, uint16_t offset, uint16_t max_length)
{
    const char* app_device_name = APP_DEVICE_NAME;
    uint16_t name_len = APP_DEVICE_NAME_LEN;
    uint16_t status = ((offset < name_len) ? GAP_ERR_NO_ERROR : ATT_ERR_INVALID_OFFSET);

    uint16_t remain_len = name_len - offset;
    uint16_t cfm_status = GAP_ERR_NO_ERROR;

    /* Length of the device name to be transmitted */
    uint8_t send_len = (remain_len < max_length)? remain_len : max_length;

    cfm_status = gapc_le_get_name_cfm(conidx, token,
                                        status, name_len, send_len,
                                        (const uint8_t*) &(app_device_name[offset]));

    swmLogInfo("    Provide device name to requesting peer. status = 0x%x...\r\n", cfm_status);
}

void App_AppearanceReq(uint8_t conidx, uint32_t metainfo, uint16_t token)
{
    uint16_t cfm_status = gapc_le_get_appearance_cfm(conidx, token,
                                                        GAP_ERR_NO_ERROR,
                                                        APP_DEVICE_APPEARANCE);

    swmLogInfo("    Provide appearance to requesting peer. status = 0x%x...\r\n", cfm_status);;
}

void App_SlavePrefParamReq(uint8_t conidx, uint32_t metainfo, uint16_t token)
{
    uint16_t cfm_status = gapc_le_get_preferred_periph_params_cfm(conidx, token,
                                                                    GAP_ERR_NO_ERROR,
                                                                    periphPrefParam);

    swmLogInfo("    Provide preferred connection parameters to requesting peer. status = 0x%x\r\n", cfm_status);

}

/* --------------------------------------------------------------------------------------------------
 * BLE Connection Configuration
 * ------------------------------------------------------------------------------------------------*/
void App_ConnParamUpdateReq(uint8_t conidx, uint32_t metainfo, const gapc_le_con_param_nego_t *p_param)
{
    uint16_t status = gapc_le_update_params_cfm(conidx, true, 0xFFFF, 0xFFFF);
    swmLogInfo("    Connection parameter update requested... status = 0x%x\r\n", status);
}

/* --------------------------------------------------------------------------------------------------
 * BLE Bond Management
 * ------------------------------------------------------------------------------------------------*/
void App_ClearBondList(void)
{
    /* Attempt to clear the bond list and return whether it was successful or not */
    if(BondList_RemoveAll())
    {
        /* Operation was successful */
        swmLogInfo("Bond list cleared successfully\r\n");
    }
    else
    {
        /* Operation was not successful */
        swmLogInfo("Failed to clear the bond list\r\n");
    }
}

/* --------------------------------------------------------------------------------------------------
 * Button Presses and Indicators
 * ------------------------------------------------------------------------------------------------*/
void App_ButtonShortPress(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Update custom attribute on any connected devices */
    AppCustomSS_UpdateButtonAttribute(CS_BUTTON_SHORT_PRESS);

#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonMediumPress(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Update custom attribute on any connected devices */
    AppCustomSS_UpdateButtonAttribute(CS_BUTTON_MEDIUM_PRESS);
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonLongPress(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Update custom attribute on any connected devices */
    AppCustomSS_UpdateButtonAttribute(CS_BUTTON_LONG_PRESS);
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonSuperLongPress(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Update custom attribute on any connected devices */
    AppCustomSS_UpdateButtonAttribute(CS_BUTTON_SUPERLONG_PRESS);

    /* Clear the bond list from MRAM */
    App_ClearBondList();
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonDoublePress(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Update custom attribute on any connected devices */
    AppCustomSS_UpdateButtonAttribute(CS_BUTTON_DOUBLE_PRESS);
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonTriplePress(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Update custom attribute on any connected devices */
    AppCustomSS_UpdateButtonAttribute(CS_BUTTON_TRIPLE_PRESS);
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonShortIndicator(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Inform the user that they're currently performing a short press */
    swmLogInfo("Short press elapsed\r\n");
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonMediumIndicator(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Inform the user that they're currently performing a medium press */
    swmLogInfo("Medium press elapsed\r\n");
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonLongIndicator(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Inform the user that they're currently performing a long press */
    swmLogInfo("Long press elapsed\r\n");
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonSuperLongIndicator(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Inform the user that they've performed a super long press, which will clear the bond list */
    swmLogInfo("Super long press elapsed - Attempting to clear the bond list\r\n");
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonDoubleIndicator(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Inform the user that they're currently performing a double press */
    swmLogInfo("Double press elapsed\r\n");
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void App_ButtonTripleIndicator(uint8_t index)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Inform the user that they're currently performing a triple press */
    swmLogInfo("Triple press elapsed\r\n");
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

/* --------------------------------------------------------------------------------------------------
 * Button and LED Interrupt Handlers
 * ------------------------------------------------------------------------------------------------*/
void GPIO0_IRQHandler(void)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Call the button GPIO interrupt event handler */
    Button_GPIOEventHandler(BUTTON_GPIO_INDEX);
#endif /* (BUTTON_MANAGER_ENABLED == 1) */
}

void TIMER0_IRQHandler(void)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Call periodic event handlers */
    Button_PeriodicEventHandler();
#endif /* (BUTTON_MANAGER_ENABLED == 1) */

#if (LED_MANAGER_ENABLED == 1)
    LED_PeriodicEventHandler();
#endif /* (LED_MANAGER_ENABLED == 1) */
}

/* --------------------------------------------------------------------------------------------------
 * Setup
 * ------------------------------------------------------------------------------------------------*/
void App_BTConfig(void)
{
    uint16_t result;

    /* Prepare the device configuration structure */
    gapm_config_t devConfig =
    {
        .role                       = GAP_ROLE_LE_ALL,

        .pairing_mode               = GAPM_PAIRING_LEGACY
                                     #if (SECURE_CONNECTION)
                                     | GAPM_PAIRING_SEC_CON
                                     #endif /* (SECURE_CONNECTION) */
                                     ,
        .pairing_min_req_key_size   = APP_PAIRING_MIN_REQ_KEY_SIZE,
        .renew_dur                  = APP_BD_RENEW_DUR,
        .private_identity.addr      = APP_BT_PRIVATE_ADDR,
        .irk.key                    = APP_IRK,
        .privacy_cfg                = (GAPM_PRIVACY_TYPE | GAPM_ADDRESS_TYPE),
        .gap_start_hdl              = GAPM_DEFAULT_GAP_START_HDL,
        .gatt_start_hdl             = GAPM_DEFAULT_GATT_START_HDL,
        .att_cfg                    = GAPM_DEFAULT_ATT_CFG,
        .sugg_max_tx_octets         = GAPM_DEFAULT_TX_OCT_MAX,
        .sugg_max_tx_time           = GAPM_DEFAULT_TX_TIME_MAX,
        .tx_pref_phy                = GAP_PHY_ANY,
        .rx_pref_phy                = GAP_PHY_ANY,
        .tx_path_comp               = 0,
        .rx_path_comp               = 0,
        .class_of_device            = 0,
        .dflt_link_policy           = 0,
    };

    swmLogInfo("    Setting BT device configuration...\r\n");
    
    /* Check privacy_cfg bit 0 to identify address type, public if not set */
    if(devConfig.privacy_cfg & GAPM_CFG_ADDR_PRIVATE)
    {
        swmLogInfo("    devConfig address set to static private random\r\n");
    }
    else
    {
        /* Read Device BLE Public Address */
        uint8_t ble_dev_addr_len = GAP_BD_ADDR_LEN;
        uint8_t ble_dev_addr_buf[GAP_BD_ADDR_LEN] = {0};

        /* Make sure proper BLE public address has been read and saved into ble_public_addr
         * using Device_BLE_Public_Address_Read() before calling Device_BLE_Param_Get() */
        Device_BT_Param_Get(PARAM_ID_BD_ADDRESS, &ble_dev_addr_len, ble_dev_addr_buf);

        swmLogInfo("    Device BT public address read: %02x:%02x:%02x:%02x:%02x:%02x\r\n",
                       ble_dev_addr_buf[5], ble_dev_addr_buf[4], ble_dev_addr_buf[3],
                       ble_dev_addr_buf[2], ble_dev_addr_buf[1], ble_dev_addr_buf[0]);

        swmLogInfo("    devConfig address set to public\r\n");
    }

    co_timer_config(&batt_rd_timer, AppBatt_BattLevelReadHandler);

    /* Set device configuration */
    result = CommonGAP_SetDeviceConfig(&devConfig, App_DeviceConfigCmpCb);
    
    /* Initialize the GAP environment */
    CommonGAP_Initialize();

    /* Initialize and set up the GATT environment */
    CommonGATT_Initialize();
    CommonGATT_SetEnvData(NULL, app_cust_svc_db, APP_NUM_CUST_SVC);
    
    if(result != GAP_ERR_NO_ERROR)
    {
        /* Error Handling */
    }
}

void App_DeviceConfigCmpCb(uint32_t event, uint16_t status)
{
    /* Device Configuration is complete */
    if(status != GAP_ERR_NO_ERROR)
    {
        /* Error Handling */
    }

    uint16_t result;

    CommonGATT_SetEnvData(NULL, app_cust_svc_db, APP_NUM_CUST_SVC);

    result = AppBASS_AddProfile();

    /* If BASS was added successfully, add the DISS */
    if(result == GAP_ERR_NO_ERROR)
    {
        /* Set timer to call AppBatt_BattLevelReadHandler after BEGIN_TIMER_S seconds */
        co_timer_start(&batt_rd_timer, TIMER_SETTING_MS(BEGIN_TIMER_S));

        swmLogInfo("    BLE profile BASS added successfully... \r\n");

        CommonGAP_IncrProfileAddedCount();

        result = AppDISS_AddProfile();
    }

    /* If DISS was added successfully, register a GATT user for Custom Services  */
    if(result == GAP_ERR_NO_ERROR)
    {

        swmLogInfo("    BLE profile DISS added successfully... \r\n");

        CommonGAP_IncrProfileAddedCount();

        result = AppCustomSS_GATTUserAndHandlerRegister();
    }

    /* GATT User registration was successful, now add Custom Service 0 */
    if(result == GAP_ERR_NO_ERROR)
    {
        result = AppCustomSS_AddService(CUST_SVC0);
    }

    /* Custom Service 0 added successfully, add Custom Service 1 */
    if(result == GAP_ERR_NO_ERROR)
    {
        swmLogInfo("    BLE Custom Service 0 added successfully... \r\n");
        result = AppCustomSS_AddService(CUST_SVC1);
    }

    /* Custom Service 1 added successfully */
    if(result == GAP_ERR_NO_ERROR)
    {
        swmLogInfo("    BLE Custom Service 1 added successfully... \r\n");
    }

    /* Request the stack to create an advertising activity. */

    swmLogInfo("    Device Configured. Creating Advertising activity...\r\n");

    result = CommonGAP_CreateAdvActv(&advActivityStatus, GAPM_OWN_ADDR_TYPE, &advParam);

    if(result != GAP_ERR_NO_ERROR)
    {
        /* Error Handling */
    }
}

/* -----------------------------------------------------------------------------------------------
 * Prepare Advertisement and Scan response Data
 * ----------------------------------------------------------------------------------------------*/
void PrepareAdvScanData(void)
{
    uint8_t companyID[] = APP_COMPANY_ID;
    uint8_t devName[]   = APP_DEVICE_NAME;

    /* Assemble advertising data as device name + company ID and
     * copy into app_adv_data */
    CommonGAP_AddAdvData(APP_DEVICE_NAME_LEN + 1, GAP_AD_TYPE_COMPLETE_NAME, devName, ADV_DATA);
    CommonGAP_AddAdvData(APP_COMPANY_ID_LEN + 1, GAP_AD_TYPE_MANU_SPECIFIC_DATA, companyID, ADV_DATA);

    /* Set scan response data as company ID */
    CommonGAP_AddAdvData(APP_COMPANY_ID_LEN + 1, GAP_AD_TYPE_MANU_SPECIFIC_DATA, companyID, SCAN_RSP_DATA);
}

/* -----------------------------------------------------------------------------------------------
 * Register App-Specific Handlers for GAP Events and Activities
 * ----------------------------------------------------------------------------------------------*/
void App_RegisterHandlers(void)
{
    /* Activity handler callbacks */
    app_le_adv_cbs = (gapm_le_adv_cb_actv_t){
        .hdr.actv.proc_cmp              = App_AdvProcCmp,
        .hdr.actv.stopped               = App_AdvStopped,
        .hdr.addr_updated               = NULL,
        .created                        = NULL,
        .scan_req_received              = NULL,
        .ext_adv_stopped                = NULL,
    };

    /* GAP event handler callbacks */
    app_conn_req_cbs = (gapc_connection_req_cb_t){
        .le_connection_req               = App_LEConnectionReq,
        .bt_connection_req               = NULL,
        .bt_periph_connection_estab      = NULL,
    };

    app_sec_cbs = (gapc_security_cb_t){
        .le_encrypt_req                  = NULL,
        .bt_encrypt_req                  = NULL,
        .auth_info                       = NULL,
        .pairing_succeed                 = NULL,
        .pairing_failed                  = NULL,
        .info_req                        = App_PairingInfoRequest,
        .auth_req                        = NULL,
        .pairing_req                     = App_PairingReq,
        .peer_iocap                      = NULL,
        .numeric_compare_req             = App_NumericCompareReq,
        .display_passkey                 = NULL,
        .key_pressed                     = NULL,
        .ltk_req                         = NULL,
        .key_received                    = NULL,
        .repeated_attempt                = NULL,
    };

    app_conn_info_cbs = (gapc_connection_info_cb_t){
        .disconnected                   = App_ConnDisconnected,
        .bond_data_updated              = NULL,
        .auth_payload_timeout           = NULL,
        .no_more_att_bearer             = NULL,
        .cli_hash_info                  = NULL,
        .name_get                       = App_NameReq,
        .appearance_get                 = App_AppearanceReq,
        .slave_pref_param_get           = App_SlavePrefParamReq,
        .name_set                       = NULL,
        .appearance_set                 = NULL,
    };

    app_le_config_cbs = (gapc_le_config_cb_t) {
        .param_update_req               = App_ConnParamUpdateReq,
        .param_updated                  = NULL,
        .packet_size_updated            = NULL,
        .phy_updated                    = NULL,
    };
}
