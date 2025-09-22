/**
 * @file app_customss.c
 * @brief Application-specific Bluetooth custom service server source file
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

#include <app_customss.h>
#include <swmTrace_api.h>
#include <string.h>
#include <stdio.h>
#include <app_bt.h>
#include "mcu_parser.h"

extern gatt_srv_cb_t app_customss_cbs;

static void AppCustomSS_EventSentCb(uint8_t conidx, uint8_t user_lid,
                                        uint16_t metainfo, uint16_t status);

static void AppCustomSS_ReadGetCb(uint8_t conidx, uint8_t user_lid, uint16_t token,
                                    uint16_t hdl, uint16_t offset, uint16_t max_length);

static void AppCustomSS_EventGetCb(uint8_t conidx, uint8_t user_lid, uint16_t token,
                                    uint16_t metainfo, uint16_t hdl, uint16_t max_length);

static void AppCustomSS_InfoGetCb(uint8_t conidx, uint8_t user_lid,
                                    uint16_t token, uint16_t hdl);

static void AppCustomSS_ValueSetCb(uint8_t conidx, uint8_t user_lid, uint16_t token,
                                    uint16_t hdl, uint16_t offset, co_buf_t* p_data);

static void print_large_buffer(const uint8_t *buffer, uint16_t length);

static app_env_tag_cs_t app_env_cs;

static const cs_att_db_desc_t att_db_cs_svc0[] =
{
    /**** Service 0 ****/
    CS_SERVICE_UUID_16(CS_SERVICE0, CS_ATT_SERVICE_128),

    /* To the BLE transfer */
    CS_CHAR_UUID_128(CS_TX_VALUE_CHAR0,
            CS_TX_VALUE_VAL0,
            CS_CHAR_TX_UUID,
            ATT_UUID(128) | PROP(RD) | PROP(WR) /*| PROP(WC)*/,
            sizeof(app_env_cs.to_air_buffer),
            app_env_cs.to_air_buffer, AppCustomSS_RxCharCallback),
    CS_CHAR_CCC(CS_TX_VALUE_CCC0,
            app_env_cs.to_air_cccd_value,
            NULL),
    CS_CHAR_USER_DESC(CS_TX_VALUE_USR_DSCP0,
            sizeof(CS_TX_CHAR_NAME) - 1,
            CS_TX_CHAR_NAME,
            NULL),

    /* From the BLE transfer */
    CS_CHAR_UUID_128(CS_RX_VALUE_CHAR0,
            CS_RX_VALUE_VAL0,
            CS_CHAR_RX_UUID,
            ATT_UUID(128) | PROP(RD) | PROP(WR),   //不用notify
            sizeof(app_env_cs.from_air_buffer),
            app_env_cs.from_air_buffer,
            NULL),
    CS_CHAR_CCC(CS_RX_VALUE_CCC0,
            app_env_cs.from_air_cccd_value,
            NULL),
    CS_CHAR_USER_DESC(CS_RX_VALUE_USR_DSCP0,
            sizeof(CS_RX_CHAR_NAME) - 1,
            CS_RX_CHAR_NAME,
            NULL),

    /* To the BLE long transfer */
    CS_CHAR_UUID_128(CS_TX_LONG_VALUE_CHAR0,
            CS_TX_LONG_VALUE_VAL0,
            CS_CHAR_LONG_TX_UUID,
            ATT_UUID(128) | PROP(RD) | PROP(WR) /*| PROP(WC)*/,
            sizeof(app_env_cs.to_air_buffer_long),
            app_env_cs.to_air_buffer_long,
            NULL),
    CS_CHAR_CCC(CS_TX_LONG_VALUE_CCC0,
            app_env_cs.to_air_cccd_value_long,
            NULL),
    CS_CHAR_USER_DESC(CS_TX_LONG_VALUE_USR_DSCP0,
            sizeof(CS_TX_CHAR_LONG_NAME) - 1,
            CS_TX_CHAR_LONG_NAME,
            NULL),

    /* From the BLE long transfer */
    CS_CHAR_UUID_128(CS_RX_LONG_VALUE_CHAR0,                /* attidx_char */
            CS_RX_LONG_VALUE_VAL0,                          /* attidx_val */
            CS_CHAR_LONG_RX_UUID,                           /* uuid */
#ifdef RX_VALUE_LONG_INDICATION
            ATT_UUID(128) | PROP(RD) | PROP(I),             /* perm, use indication */
#else
            ATT_UUID(128) | PROP(RD) | PROP(N),             /* perm, use notification */
#endif
            sizeof(app_env_cs.from_air_buffer_long),        /* length */
            app_env_cs.from_air_buffer_long,                /* data */
            AppCustomSS_RXLongCharCallback),                /* callback */
    /* Client Characteristic Configuration descriptor */
    CS_CHAR_CCC(CS_RX_LONG_VALUE_CCC0,                      /* attidx */
            app_env_cs.from_air_cccd_value_long,            /* data */
            NULL),                                          /* callback */
    /* Characteristic User Description descriptor */
    CS_CHAR_USER_DESC(CS_RX_LONG_VALUE_USR_DSCP0,           /* attidx */
            sizeof(CS_RX_CHAR_LONG_NAME)- 1,                /* length */
            CS_RX_CHAR_LONG_NAME,                           /* data */
            NULL),                                          /* callback */
};

static const cs_att_db_desc_t att_db_cs_svc1[] =
{
    /**** Service 1 ****/
    CS_SERVICE_UUID_16(CS_SERVICE1, CS_ATT_SERVICE_128),

    /* From the BLE LED Transfer */
    CS_CHAR_UUID_128(CS_LED_VALUE_CHAR1,
             CS_LED_VALUE_VAL1,
             CS_CHAR_LED_UUID,
             ATT_UUID(128) | PROP(RD) | PROP(WR) /*| PROP(WC)*/,
             sizeof(app_env_cs.led_from_air_buffer),
             app_env_cs.led_from_air_buffer,
             AppCustomSS_LEDCharCallback),
    CS_CHAR_CCC(CS_LED_VALUE_CCC1,
             app_env_cs.led_from_air_cccd_value,
             NULL),
    CS_CHAR_USER_DESC(CS_LED_VALUE_USR_DSCP1,
             sizeof(CS_LED_CHAR_NAME) - 1,
             CS_LED_CHAR_NAME,
             NULL),

    /* To the BLE Button transfer */
    CS_CHAR_UUID_128(CS_BUTTON_VALUE_CHAR1,
             CS_BUTTON_VALUE_VAL1,
             CS_CHAR_BUTTON_UUID,
             ATT_UUID(128) | PROP(RD) | PROP(N),
             sizeof(app_env_cs.button_to_air_buffer),
             app_env_cs.button_to_air_buffer, NULL),
    CS_CHAR_CCC(CS_BUTTON_VALUE_CCC1,
             app_env_cs.button_to_air_cccd_value,
             NULL),
    CS_CHAR_USER_DESC(CS_BUTTON_VALUE_USR_DSCP1,
             sizeof(CS_BUTTON_CHAR_NAME) - 1,
             CS_BUTTON_CHAR_NAME,
             NULL),
};

static gatt_att_desc_t cs_svc0_att_desc[CS_NB0];
static gatt_att_desc_t cs_svc1_att_desc[CS_NB1];

static uint32_t notifyOnTimeout;
static uint8_t val_notif = 0;

static CS_ButtonPressType_t button_press_type = CS_BUTTON_SHORT_PRESS;

/**
 * @brief Initialize custom service environment.
 *
 * @param[in] notif_timeout  notification timeout for periodically updating attributes
 *
 */
void AppCustomSS_Init(uint32_t notif_timeout)
{
    memset(&app_env_cs, 0x00, sizeof(app_env_tag_cs_t));

    /* Maximal length for Characteristic values */
    app_env_cs.pref_mtu = CS_LONG_VALUE_MAX_LENGTH;
    app_env_cs.prio_level = 0x00;
    app_env_cs.user_lid = GATT_INVALID_USER_LID;
    app_env_cs.rx_changed = 0;

#if 1
    // comment notify function
    app_env_cs.button_to_air_cccd_value[0] = GATT_CCC_START_NTF;
    app_env_cs.button_to_air_cccd_value[1] = GATT_CCC_STOP_NTFIND;

    notifyOnTimeout = notif_timeout;

#endif
    /* Prepare the gatt_att_desc_t structures for... */

    /* Service 0 */
    for(uint8_t i = 0; i < CS_NB0; i++)
    {
        cs_svc0_att_desc[i] = att_db_cs_svc0[i].att;
    }

    /* Service 1 */
    for(uint8_t i = 0; i < CS_NB1; i++)
    {
        cs_svc1_att_desc[i] = att_db_cs_svc1[i].att;
    }


    // comment notify function
    co_timer_periodic_config(&app_env_cs.notif_timer, AppCustomSS_NotifOnTimeout);
    co_timer_config(&app_env_cs.button_timer, AppCustomSS_ButtonNotifOnTimeout);

}

/**
 * @brief 			Register Custom service server as a GATT user.
 * 					Register the application specific gatt server calbacks.
 *
 * @param[in] 		None
 *
 * @return 			Status of the CommonGATT_RegisterServer execution (see enum #hl_err)
 */
uint16_t  AppCustomSS_GATTUserAndHandlerRegister(void)
{
	uint16_t retval = GAP_ERR_NO_ERROR;
	
	/* Register as a GATT user */
    retval =  CommonGATT_RegisterServer(app_env_cs.pref_mtu, app_env_cs.prio_level, &app_env_cs.user_lid);

    if(retval == GAP_ERR_NO_ERROR)
    {
    	 /* Register the app-specific callbacks */
    	app_customss_cbs = (gatt_srv_cb_t) {
            .cb_event_sent    = AppCustomSS_EventSentCb,
            .cb_att_read_get  = AppCustomSS_ReadGetCb,
            .cb_att_event_get = AppCustomSS_EventGetCb,
            .cb_att_info_get  = AppCustomSS_InfoGetCb,
            .cb_att_val_set   = AppCustomSS_ValueSetCb,
    	};
    }

    return retval;
}

/**
 * @brief Add custom services - Service 0 and Service 1.
 *
 * @param[in] custom service ID to be added
 *
 * @return Status of the CommonGATT_AddService execution (see enum #hl_err)
 */
uint16_t AppCustomSS_AddService(uint8_t cs_svc_id)
{
    uint16_t res = GAP_ERR_NO_ERROR;

    /* Service 0 */
    if(cs_svc_id == CUST_SVC0)
    {
        const uint8_t cs0_uuid[] = CS_SVC_UUID;
        
        CommonGATT_UpdateEnv(att_db_cs_svc0, CS_NB0);

        res = CommonGATT_AddService(app_env_cs.user_lid, SVC_SEC_LVL(NOT_ENC) | SVC_UUID(128),
                                    cs0_uuid, CS_NB0, NULL, &(cs_svc0_att_desc[0]),
                                    CS_NB0);

        co_timer_periodic_start(&app_env_cs.notif_timer, notifyOnTimeout);
    }
    else if(cs_svc_id == CUST_SVC1)
    {
        const uint8_t cs1_uuid[] = CS_BLT_SVC_UUID;
        
        CommonGATT_UpdateEnv(att_db_cs_svc1, CS_NB1);

        res = CommonGATT_AddService(app_env_cs.user_lid, SVC_SEC_LVL(NOT_ENC) | SVC_UUID(128),
                                    cs1_uuid, CS_NB1, NULL, &(cs_svc1_att_desc[0]),
                                    CS_NB1);
    }
    else
    {
        res = GAP_ERR_INVALID_PARAM;
    }

    return res;
}

/**
 * @brief Update local button attribute and set timer to update custom service
 *
 * @param[in] press_type The type of press which has most recently been performed,
 *                       which will overwrite the existing button attribute value
 *
 */
void AppCustomSS_UpdateButtonAttribute(CS_ButtonPressType_t press_type)
{
    /* Update the press type and set a timer to trigger the button notif event */
    button_press_type = press_type;
    co_timer_start(&app_env_cs.button_timer, 0);



    swmLogWarn("button pressed :%d\r\n", press_type);
  	//play dtmf
  //	MCU_DTMF.OnOff = onoff;
 // 	MCU_DTMF.FreqHigh = 400;
  //	MCU_DTMF.FreqLow = 400;
  //	onoff = 1- onoff;
  //	Fill_SmData_Buffer();


}

/**
 * @brief Update attributes periodically (notification)
 *
 * @param[in] p_env     Pointer to be passed when timer expires
 *
 */
void AppCustomSS_NotifOnTimeout(co_timer_periodic_t* p_timer)
{
#if 0
    uint8_t         i;

    /* Update the RX long characteristic value */
    memset(&app_env_cs.from_air_buffer[0], val_notif, CS_VALUE_MAX_LENGTH);

    /* Update RX long characteristic with the inverted version of
     * TX long characteristic */
    for (i = 0; i < CS_LONG_VALUE_MAX_LENGTH; i++)
    {
        app_env_cs.from_air_buffer_long[i] = 0xFF ^ app_env_cs.to_air_buffer_long[i];
    }

    /* Check if notification/indications are set for attributes*/

    if (app_env_cs.from_air_cccd_value_long[0] != GATT_CCC_STOP_NTFIND &&
            app_env_cs.from_air_cccd_value_long[1] == GATT_CCC_STOP_NTFIND)
    {
        gatt_att_t att;
        uint8_t evt_type;
        att.hdl = CommonGATT_GetHandle(CUST_SVC0, CS_RX_LONG_VALUE_VAL0);
        att.length = CS_VALUE_MAX_LENGTH;


        if (app_env_cs.from_air_cccd_value_long[0] == GATT_CCC_START_NTF)
        {
            evt_type = GATT_NOTIFY;
        }
        else if (app_env_cs.from_air_cccd_value_long[0] == GATT_CCC_START_IND)
        {
            evt_type = GATT_INDICATE ;
        }
        else
        {
            swmLogInfo("    Invalid CCC value 0x000%x", app_env_cs.from_air_cccd_value_long[0]);
            return;
        }

        /* Since CS_LONG_VALUE_MAX_LENGTH > L2CAP_LE_MTU_MIN,
         * we must use reliable send */

        /* Send event to each connected peer.*/
        for(i = 0; i < APP_MAX_NB_CON; i++)
        {
            if(CommonGAP_IsConnected(i))
            {
            	gatt_srv_event_reliable_send(i, app_env_cs.user_lid, 0, evt_type, 1, &att);

                swmLogInfo("    Custom service server notifying connected peers (RX_LONG_VALUE): ");
                print_large_buffer(app_env_cs.from_air_buffer_long, CS_LONG_VALUE_MAX_LENGTH);
            }
        }
    }

    if ((app_env_cs.from_air_cccd_value[0] == GATT_CCC_START_NTF &&
                            app_env_cs.from_air_cccd_value[1] == GATT_CCC_STOP_NTFIND))
    {
        for(i = 0; i < APP_MAX_NB_CON; i++)
        {
            if(CommonGAP_IsConnected(i))
            {
                /* Since CS_VALUE_MAX_LENGTH < L2CAP_LE_MTU_MIN, we can use send */
            	co_buf_t    *p_buf = NULL;
                uint16_t res       = GAP_ERR_NO_ERROR;

            	if(co_buf_alloc(&p_buf, GATT_BUFFER_HEADER_LEN, CS_VALUE_MAX_LENGTH, GATT_BUFFER_TAIL_LEN) ==
            						CO_BUF_ERR_INSUFFICIENT_RESOURCE)
            	{
                    swmLogInfo("    Unable to allocate buffer for notification\r\n");
                    return;
            	}

            	co_buf_copy_data_from_mem(p_buf, app_env_cs.from_air_buffer, CS_VALUE_MAX_LENGTH);

            	res = gatt_srv_event_send(i,
                                          app_env_cs.user_lid,
                                          0, GATT_NOTIFY,
                                          CommonGATT_GetHandle(CUST_SVC0, CS_RX_VALUE_VAL0),
                                          p_buf);

                swmLogInfo("    Custom service server notifying peer %u (status=0x%x) peers (RX_VALUE): ", i, res);
                print_large_buffer(app_env_cs.from_air_buffer, CS_VALUE_MAX_LENGTH);

                if(p_buf!= NULL)
                {
                    co_buf_release(p_buf);
                }
            }
        }
        val_notif++;
    }
#endif
}

/**
 * @brief Update button attribute on all connected devices upon timer expiry
 *
 * @param[in] p_timer Pointer to be passed when timer expires
 *
 */
void AppCustomSS_ButtonNotifOnTimeout(co_timer_t* p_timer)
{
	//根据button_press_type 决定操作
	J20_Button_Evt();
#if 0
    /* Send out a button notif to update the button attribute for each connected device */
    for(uint8_t i = 0; i < APP_MAX_NB_CON; i++)
    {
        if(CommonGAP_IsConnected(i))
        {
            /* Since CS_VALUE_MAX_LENGTH < L2CAP_LE_MTU_MIN, we can use send */
            co_buf_t    *p_buf = NULL;
            uint16_t res       = GAP_ERR_NO_ERROR;

            if(co_buf_alloc(&p_buf, GATT_BUFFER_HEADER_LEN, CS_LED_BUTTON_MAX_LENGTH, GATT_BUFFER_TAIL_LEN) ==
                                CO_BUF_ERR_INSUFFICIENT_RESOURCE)
            {
                swmLogInfo("    Unable to allocate buffer for notification\r\n");
                return;
            }

            co_buf_copy_data_from_mem(p_buf, &button_press_type, CS_LED_BUTTON_MAX_LENGTH);

            res = gatt_srv_event_send(i,
                                      app_env_cs.user_lid,
                                      0, GATT_NOTIFY,
                                      CommonGATT_GetHandle(CUST_SVC1, CS_BUTTON_VALUE_VAL1),
                                      p_buf);

            swmLogInfo("    Custom service server notifying connected peer %u (status=0x%x) (BUTTON_VALUE): ", i, res);
            print_large_buffer(&button_press_type, CS_LED_BUTTON_MAX_LENGTH);

            if(p_buf!= NULL)
            {
                co_buf_release(p_buf);
            }
        }
    }
#endif
}

/**
 * @brief This function is called when GATT server user has initiated event send to peer
 *        device or if an error occurs.
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] metainfo      Dummy parameter provided by upper layer for command execution
 * @param[in] status        Status of the procedure (see enum #hl_err)
 */
static void AppCustomSS_EventSentCb(uint8_t conidx, uint8_t user_lid,
                                        uint16_t metainfo, uint16_t status)
{
    // TODO
}

/**
 * @brief This function is called when peer want to read local attribute database value.
 *
 *        #gatt_srv_att_read_get_cfm shall be called to provide attribute value
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] token         Procedure token that must be returned in confirmation function
 * @param[in] hdl           Attribute handle
 * @param[in] offset        Value offset
 * @param[in] max_length    Maximum value length to return
 */
static void AppCustomSS_ReadGetCb(uint8_t conidx, uint8_t user_lid, uint16_t token,
                                    uint16_t hdl, uint16_t offset, uint16_t max_length)
{
	// TODO
}

/**
 * @brief This function is called when GATT server user has initiated event send procedure,
 *
 *        #gatt_srv_att_event_get_cfm shall be called to provide attribute value
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] token         Procedure token that must be returned in confirmation function
 * @param[in] metainfo      Dummy parameter provided by upper layer for command execution.
 * @param[in] hdl           Attribute handle
 * @param[in] max_length    Maximum value length to return
 */
static void AppCustomSS_EventGetCb(uint8_t conidx, uint8_t user_lid, uint16_t token,
                                    uint16_t metainfo, uint16_t hdl, uint16_t max_length)
{
	// TODO
}

/**
 * @brief This function is called during a write procedure to get information about a
 *        specific attribute handle.
 *
 *        #gatt_srv_att_info_get_cfm shall be called to provide attribute information
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] token         Procedure token that must be returned in confirmation function
 * @param[in] hdl           Attribute handle
 */
static void AppCustomSS_InfoGetCb(uint8_t conidx, uint8_t user_lid, uint16_t token, uint16_t hdl)
{
	// TODO
}

/**
 * @brief This function is called during a write procedure to modify attribute handle.
 *
 *        #gatt_srv_att_val_set_cfm shall be called to accept or reject attribute
 *        update.
 *
 * @param[in] conidx        Connection index
 * @param[in] user_lid      GATT user local identifier
 * @param[in] token         Procedure token that must be returned in confirmation function
 * @param[in] hdl           Attribute handle
 * @param[in] offset        Value offset
 * @param[in] p_data        Pointer to buffer that contains data to write starting from offset
 */
static void AppCustomSS_ValueSetCb(uint8_t conidx, uint8_t user_lid, uint16_t token,
                                    uint16_t hdl, uint16_t offset, co_buf_t* p_data)
{
	// TODO
	app_env_cs.rx_changed = 1;
	 swmLogInfo("    AppCustomSS_ValueSetCb (%d): offset (%d) \r\n", conidx, offset);
	 //Trigger update mcu configs

}

/**
 * @brief This function uses swmTrace_printf for 10 Bytes of data at time.
 *        The remaining byte/s will be printed in string format.
 *
 * @param[in] buffer        data buffer
 * @param[in] length        length of data
 */
static void print_large_buffer(const uint8_t *buffer, uint16_t length)
{
    unsigned int num_of_blocks = (length / 10);
    unsigned int rem_bytes = ((num_of_blocks) ? (length % 10) : length);

    for(int i = 0; i < num_of_blocks; i++)
    {
        int j = (i * 10);
        swmTrace_printf("%02x %02x %02x %02x %02x %02x %02x %02x %02x %02x ", buffer[j],
                buffer[j+1], buffer[j+2], buffer[j+3],
                buffer[j+4], buffer[j+5], buffer[j+6],
                buffer[j+7], buffer[j+8], buffer[j+9]);
    }

    if(rem_bytes)
    {
        int j = (num_of_blocks * 10);
        char temp_string[28] = {0};
        uint8_t index = 0;

        for (int i = j; i < length; i++)
        {
            sprintf(&temp_string[index], "%02x", buffer[i]);
            index += 2;
            /* put space before next data */
            temp_string[index] = 0x20;
            index += 1;
        }

        swmTrace_printf("%s ", temp_string);
    }

    swmTrace_printf("\r\n");
}

/**
* @brief                    User callback data access function for the RX Long
*                           characteristic.
* @param[in]      conidx    connection index
* @param[in]      attidx    attribute index in the user defined database
* @param[in]      handle    attribute handle allocated in the BLE stack
* @param[in]      to        pointer to destination buffer
* @param[in]      from      pointer to source buffer
* @param[in]                length    - length of data to be copied
* @param[in]                operation - GATTC_ReadReqInd or GATTC_WriteReqInd
* @param[in]                hl_status - HL error code
*
* @return Outputs          hl_status otherwise
*/
uint16_t AppCustomSS_RXLongCharCallback(uint8_t conidx, uint16_t attidx, uint16_t handle,
                                            co_buf_t* to, uint8_t* from,
                                            common_gatt_srv_op_t op, uint16_t length,
                                            uint16_t offset, uint16_t hl_status)
{
    if(hl_status == GAP_ERR_NO_ERROR)
    {
        co_buf_copy_data_from_mem(to, (from + offset), length - offset);
        if(op == COMMON_GATT_SRV_READ_GET)
        {
            for (uint8_t i = offset; i < (length - offset); i++)
            {
                app_env_cs.from_air_buffer_long[i] = 0xFF ^ app_env_cs.to_air_buffer_long[i];
            }
        }

    }
    else
    {
        swmLogInfo("    RXLongCharCallback (%d): error(%d) \r\n", conidx, hl_status);
    }

    return hl_status;
}

/**
* @brief                    User callback data access function for the RX characteristic.
* @param[in]      conidx    connection index
* @param[in]      attidx    attribute index in the user defined database
* @param[in]      handle    attribute handle allocated in the BLE stack
* @param[in]      to        pointer to destination buffer
* @param[in]      from      pointer to source buffer
* @param[in]                length    - length of data to be copied
* @param[in]                operation - GATTC_ReadReqInd or GATTC_WriteReqInd
* @param[in]                hl_status - HL error code
*
* @return Outputs          hl_status otherwise
*/
uint16_t AppCustomSS_RxCharCallback(uint8_t conidx, uint16_t attidx, uint16_t handle,
                                        co_buf_t* to, uint8_t* from,
                                        common_gatt_srv_op_t op, uint16_t length,
                                        uint16_t offset, uint16_t hl_status)
{
    if(hl_status == GAP_ERR_NO_ERROR)
    {
        co_buf_copy_data_from_mem(to, (from + offset), length - offset);
    }
    else
    {
        swmLogInfo("    RXCharCallback (%d): error(%d) \r\n", conidx, hl_status);
    }
    if (op==4)
    	swmLogInfo("    RXCharCallback (%d):  length:%d , data:%0x %0x \r\n", conidx, length,from[0],from[1]);
    return hl_status;
}

/**
* @brief                    User callback data access function for the LED characteristic.
* @param[in]      conidx    connection index
* @param[in]      attidx    attribute index in the user defined database
* @param[in]      handle    attribute handle allocated in the BLE stack
* @param[in]      to        pointer to destination buffer
* @param[in]      from      pointer to source buffer
* @param[in]                length    - length of data to be copied
* @param[in]                operation - GATTC_ReadReqInd or GATTC_WriteReqInd
* @param[in]                hl_status - HL error code
*
* @return Outputs          hl_status otherwise
*/
uint16_t AppCustomSS_LEDCharCallback(uint8_t conidx, uint16_t attidx, uint16_t handle,
                                    co_buf_t* to, uint8_t* from,
                                    common_gatt_srv_op_t op, uint16_t length,
                                    uint16_t offset, uint16_t hl_status)
{
    if(hl_status == GAP_ERR_NO_ERROR)
    {
        if (op == COMMON_GATT_SRV_VAL_SET)
        {
            co_buf_copy_data_to_mem(to, (from + offset), length - offset);

            /* Set LED state based on received data */
            if (app_env_cs.led_from_air_buffer[0] == 0)
            {
#if (LED_MANAGER_ENABLED == 1)
                LED_TurnOff(LED_STATE_GPIO_INDEX);
#endif /* (LED_MANAGER_ENABLED == 1) */
                swmLogInfo("    Received LED OFF\r\n");
            }
            else if(app_env_cs.led_from_air_buffer[0] == 1)
            {
#if (LED_MANAGER_ENABLED == 1)
                LED_TurnOn(LED_STATE_GPIO_INDEX);
#endif /* (LED_MANAGER_ENABLED == 1)*/
                swmLogInfo("    Received LED ON\r\n");
            }
            else
            {
                /* Do nothing */
            }
        }
        else if(op == COMMON_GATT_SRV_READ_GET)
        {
            co_buf_copy_data_from_mem(to, (from + offset), length - offset);
        }
        else
        {
            /* Do nothing */
        }
    }
    else
    {
        swmLogInfo("    RXCharCallback (%d): error(%d) \r\n", conidx, hl_status);
    }

    return hl_status;
}

//2025-03-01


int  arr_nsdeep_levels[32] = {
		0x0,0x2,0x4,0x8,
		0x10,0x20,0x40,0x80,
		0x100,0x200,0x400,0x800,
		0x1000,0x2000,0x4000,0x8000,
		0x10000,0x20000,0x40000,0x80000,
		0x100000,0x200000,0x400000,0x800000,
		0x1000000,0x2000000,0x4000000,0x8000000,
		0x10000000,0x20000000,0x40000000,0x7fffffff
};

//  有声和噪音情况下的降噪深度 db (0..18)  EQ_Gain = Q31*pow(10,(Cfg->dB_Gain_float[i])/20);
 //0 db , -1 db,
#if 0
int arr_nsdeep_dbs[32] ={
		0x7fffffff, 0x70000000,0x65000000,0x5a000000,
		0x50000000, 0x48000000,0x40000000,0x39000000,
		0x33000000, 0x2d000000,0x28000000,0x24000000,
		0x21000000, 0x1e000000,0x1b000000,0x18000000,
		0x15000000, 0x12000000,0x10000000,0x0e000000,
		0x0d000000, 0x0b000000,0x0a000000,0x09000000,
		0x08000000, 0x07000000,0x06000000,0x05b00000,
		0x05100000, 0x04800000,0x04000000,0x03900000};
#endif


void  Update_SMData_RX(uint8_t* valptr, uint16_t lenData) {
	if (valptr[0] == 0xAA) {
		Update_ShortSMData_RX(valptr,lenData);
		return ;
	}


	 uint8_t  result_xor = valptr[1]  ^ valptr[2];
	 result_xor = result_xor ^valptr[3];
	 result_xor = result_xor ^ 0xDE;
	 //we use XOR to make sure correct data format,这两行应该加的，当时因为和web 端没测试好，先注释掉吧
	 //2023-07-15 加上这行
	// if (valptr[4] != result_xor)
	//	 return ;
	 //我们的新版每次都是发送100个字节的，而老版本都比较短，用lenData 也可以起到一点作用
	// if  (lenData <30)  return ;

	 uint8_t mem_idx = valptr[0];
	// MCU_MULTI_GAIN.ADC_GAIN = valptr[1];


	 MCU_VOLUME.Volume = 0 - valptr[1];


	// MCU_MULTI_GAIN.DAC_GAIN = 0;

	 uint32_t  arr_dmic_gains[] ={0xfff,0xcad,0xa12,0x800,0x65a,0x50c,0x402,0x32f,0x287,0x202};
	 //这里是更新DMIC的增益
	 if ((valptr[2] <=9) && (valptr[2]>=0)) {
		 AUDIO->DMIC0_GAIN = arr_dmic_gains[valptr[2]];
		 AUDIO->DMIC1_GAIN =  arr_dmic_gains[valptr[2]];

	 }
	 if ((valptr[89] <=9) && (valptr[89]>=0)) {
				 AUDIO->OD_GAIN = arr_dmic_gains[valptr[89]];

     }



	 uint8_t wdrc_mask =valptr[3];



	 int base_offset = 20;
	//WDRC  这里6*8=48

	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		MCU_WDRC.exp_cr[band_idx] = 0.1f * valptr[base_offset++];

	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		MCU_WDRC.exp_end_knee[band_idx] = 1.0f * valptr[base_offset++];

	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		MCU_WDRC.tkgain[band_idx] = 1.0f * valptr[base_offset++];

	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		MCU_WDRC.tk[band_idx] = 1.0f * valptr[base_offset++];

	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		MCU_WDRC.cr[band_idx] = 0.1f * valptr[base_offset++];

	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		MCU_WDRC.bolt[band_idx] = 1.0f * valptr[base_offset++];




	//EQ 以后再说
	base_offset = 80;
	for (uint8_t band_idx=1;band_idx <=8;band_idx++)
			MCU_EQ.dB_Gain_float[band_idx] = 0.0f - valptr[base_offset++] ;

	//不要再这里进行浮点运算，我们放到while()


	 //noise deep level
	MCU_AI_NS.NS_LEVEL = 0.1f*valptr[93];


	if  (valptr[94] == 0)  valptr[94]= 6;


	MCU_DPEQ.Energy_Time = valptr[94];
	MCU_DPEQ.Threshold_High = 0-valptr[95];

	MCU_DPEQ.Threshold_Low = 0-valptr[96];
	if (valptr[96] ==0)
		MCU_DPEQ.Threshold_Low = -1024;


	MCU_AGCO.Threshold = 0- valptr[97];

	 //98: Mark_Gain
	//MCU_VOLUME.Mark_Gain =  valptr[98] ;

	uint8_t  ns_strength_vox  = valptr[100];
	//这里要改为32 个band

	for (uint8_t band_idx=0;band_idx<32;band_idx++)
		MCU_NS_WIENER.normal_max_depth_dB[band_idx] = 0- ns_strength_vox;


	uint8_t  ns_strength_novox  = valptr[101];
	//这里要改为32 个band
	for (uint8_t band_idx=0;band_idx<32;band_idx++)
		MCU_NS_WIENER.low_noise_max_depth_dB[band_idx] = 0-ns_strength_novox;


	uint8_t nox_level  =valptr[102];
	if (nox_level >31)  nox_level =31;
	MCU_NS_WIENER.nc_common_param[6] = arr_nsdeep_levels[nox_level] ;



	uint8_t lownoise_level  = valptr[103] ;
	if (lownoise_level>31) lownoise_level =31;
	MCU_NS_WIENER.nc_common_param[14] = arr_nsdeep_levels[lownoise_level];




}


void  Update_ShortSMData_RX(uint8_t* valptr, uint16_t lenData) {


}

void Readfrom_SmData_Buffer(uint8_t*  valptr) {

	valptr[0] = 0; //mem id 0 cs_env[0].arr_params[0];

	valptr[1] = (uint8_t) (0 -MCU_VOLUME.Volume);

	//这行要去掉
	//valptr[2] = (uint8_t)MCU_MULTI_GAIN.DAC_GAIN;

	valptr[3] = 0x0;
	if(SM_Ptr->Control&MASK16(WDRC)) valptr[3] |= 0x10;


	if(SM_Ptr->Control&MASK16(EQ)) valptr[3] |= 0x8;
	if(SM_Ptr->Control&MASK16(AFC)) valptr[3] |= 0x4;
	if(SM_Ptr->Control&MASK16(NC)) valptr[3] |= 0x2;


	if(valptr[3] == 0x0) {
		if(SM_Ptr->Control&MASK16(NC)) valptr[3] = 0x2;
		if(SM_Ptr->Control&MASK16(POST_BQ)) valptr[3] = 0x1;

	}


	 memcpy((uint8_t*)&valptr[10],"j20demo",10);



	//valptr[3..19]  空下来
	int base_offset = 20;
	//WDRC  这里6*8=48
	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		valptr[base_offset++] = (uint8_t)(MCU_WDRC.exp_cr[band_idx]*10);
	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		valptr[base_offset++] = (uint8_t)(MCU_WDRC.exp_end_knee[band_idx]*1);
	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		valptr[base_offset++] = (uint8_t)(MCU_WDRC.tkgain[band_idx]*1);
	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		valptr[base_offset++] = (uint8_t)(MCU_WDRC.tk[band_idx]*1);
	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		valptr[base_offset++] = (uint8_t)(MCU_WDRC.cr[band_idx]*10);
	for (uint8_t band_idx=0;band_idx <8;band_idx++)
		valptr[base_offset++] = (uint8_t)(MCU_WDRC.bolt[band_idx]*1);

	//valptr[68..79] 空下来

	//70,71 用于记录电压为0时候的adc 值




	//EQ 以后再说
	base_offset = 80;
	for (uint8_t band_idx=1;band_idx <=8;band_idx++)
			valptr[base_offset++] = (uint8_t)(0-MCU_EQ.dB_Gain_float[band_idx]);


    //noise deep level

   valptr[93] = (uint8_t)(MCU_AI_NS.NS_LEVEL *10);

   //DPEQ
   valptr[94] =(uint8_t)(MCU_DPEQ.Energy_Time);
   valptr[95] = (uint8_t)(MCU_DPEQ.Threshold_High);
   valptr[96] = (uint8_t)(MCU_DPEQ.Threshold_Low);

   //AGCO
   valptr[97] = (uint8_t)(0- MCU_AGCO.Threshold);
   //98: Mark_Gain
  // valptr[98] = (uint8_t)MCU_VOLUME.Mark_Gain;

   uint8_t ns_strength_vox  = 0-MCU_NS_WIENER.normal_max_depth_dB[0];
   valptr[100] =ns_strength_vox;

   uint8_t ns_strength_novox  = 0- MCU_NS_WIENER.low_noise_max_depth_dB[0];
   valptr[101] = ns_strength_novox;


	uint8_t nox_level  = 31;

  	int32_t tmp_nox_level = MCU_NS_WIENER.nc_common_param[6] ;
  	for (int i=0;i<=31;i++) {
  	   		if (tmp_nox_level == arr_nsdeep_levels[i]) {
  	   		nox_level = i;
  	   			break;
  	   		}
  	   	}

  	valptr[102] = nox_level;

   uint8_t lownoise_level  =31 ;


   	int32_t  tmp_lownoise_level = MCU_NS_WIENER.nc_common_param[14]  ;
   	for (int i=0;i<=31;i++) {
   		if (tmp_lownoise_level == arr_nsdeep_levels[i]) {
   			lownoise_level = i;
   			break;
   		}
   	}

   	valptr[103] = lownoise_level;



   base_offset =110;
   for (uint8_t band_idx=0;band_idx <8;band_idx++)
   		valptr[base_offset++] = (uint8_t)(MCU_WDRC.Attack_time[band_idx]*1);
   //0ms 8ms 16 ms ...
   for (uint8_t band_idx=0;band_idx <8;band_idx++)
   		valptr[base_offset++] = (uint8_t)(MCU_WDRC.Release_time[band_idx]/8.0);

}

void J20_UPDATE_DSP() {
	//Update_SMData_RX(app_env_cs.from_air_buffer,CS_VALUE_MAX_LENGTH);
	if (app_env_cs.rx_changed ==1) {
		Update_SMData_RX(app_env_cs.from_air_buffer,CS_VALUE_MAX_LENGTH);
		Fill_SmData_Buffer();

	    uint8_t wdrc_mask = app_env_cs.from_air_buffer[3];
	    if (wdrc_mask ==0)  SM_Ptr->Control = MASK16(LOOPBACK);
	    if (wdrc_mask ==2)  SM_Ptr->Control = MASK16(PRE_BQ)|MASK16(POST_BQ)|MASK16(NC);
	   	 if (wdrc_mask ==1)  SM_Ptr->Control = MASK16(PRE_BQ)|MASK16(POST_BQ);


	   	 if (wdrc_mask  & 0x10)  SM_Ptr->Control |= MASK16(WDRC);
	   		 else
	   			 SM_Ptr->Control &= ~MASK16(WDRC);

	   		 if (wdrc_mask  & 0x8)  SM_Ptr->Control |= MASK16(EQ);
	   		 else
	   			 SM_Ptr->Control &= ~MASK16(EQ);


	   		 if (wdrc_mask  & 0x4)  SM_Ptr->Control |= MASK16(AFC);
	   		 else
	   			 SM_Ptr->Control &= ~MASK16(AFC);

	   		 if (wdrc_mask  & 0x2)  SM_Ptr->Control |= MASK16(NC);
	   		 else
	   			 SM_Ptr->Control &= ~MASK16(NC);


	   		J20_UpdateDSP(security_key,64);

		app_env_cs.rx_changed = 0;
	} else {
		//是不是第一次初始化？
		if (app_env_cs.from_air_buffer[CS_VALUE_MAX_LENGTH-1] == 0) {
			Readfrom_SmData_Buffer(app_env_cs.from_air_buffer);
			app_env_cs.from_air_buffer[CS_VALUE_MAX_LENGTH-1] = 0x20;

		}



	}
}
void J20_Button_Evt() {

	if (button_press_type == CS_BUTTON_SHORT_PRESS) {
		//短按音量
	}
	if (button_press_type == CS_BUTTON_LONG_PRESS) {
		//长按，切换模式
	}
	if (button_press_type == CS_BUTTON_SUPERLONG_PRESS) {
		//超长按,关机


	}

}

