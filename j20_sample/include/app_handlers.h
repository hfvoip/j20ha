/**
 * @file app_msg_handler.h
 * @brief Application message handlers header
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

#ifndef APP_MSG_HANDLER_H
#define APP_MSG_HANDLER_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */


/* --------------------------------------------------------------------------------------------------
 * Advertising activity management callbacks
 * ------------------------------------------------------------------------------------------------*/
void App_AdvStopped(uint32_t metainfo, uint8_t actv_idx, uint16_t reason);

void App_AdvProcCmp(uint32_t metainfo, uint8_t proc_id, uint8_t actv_idx, uint16_t status);

void App_LEDBlinkNumConnections(void);

/* --------------------------------------------------------------------------------------------------
 * Connection Establishment/Address Resolution callbacks
 * ------------------------------------------------------------------------------------------------*/
void App_LEConnectionReq(uint8_t conidx, uint32_t metainfo, uint8_t actv_idx, uint8_t role,
                            const gap_bdaddr_t* p_peer_addr,
                            const gapc_le_con_param_t* p_con_params,
                            uint8_t clk_accuracy);

void App_ResolvAddrRes(uint16_t status, const gap_addr_t* p_addr, const gap_sec_key_t* p_irk);

/* --------------------------------------------------------------------------------------------------
 * Connection Security callbacks
 * ------------------------------------------------------------------------------------------------*/
void App_AuthInfo(uint8_t conidx, uint32_t metainfo, uint8_t sec_lvl, bool encrypted);

void App_PairingSucceed(uint8_t conidx, uint32_t metainfo);

void App_PairingFailed(uint8_t conidx, uint32_t metainfo, uint16_t reason);

void App_PairingInfoRequest(uint8_t conidx, uint32_t metainfo, uint8_t exp_info);

void App_NumericCompareReq(uint8_t conidx, uint32_t metainfo, uint32_t value);

void App_EncryptReq(uint8_t conidx, uint32_t metainfo, uint16_t ediv, const gap_le_random_nb_t* p_rand);

void App_PairingReq(uint8_t conidx, uint32_t metainfo, uint8_t auth_level);

void App_PairingKeysReceived(uint8_t conidx, uint32_t metainfo, const gapc_pairing_keys_t* p_keys);

void App_LTKReq(uint8_t conidx, uint32_t metainfo, uint8_t key_size);

/* --------------------------------------------------------------------------------------------------
 * Connection Information callbacks
 * ------------------------------------------------------------------------------------------------*/
void App_ConnDisconnected(uint8_t conidx, uint32_t metainfo, uint16_t reason);

void App_NameReq(uint8_t conidx, uint32_t metainfo, uint16_t token,
                          uint16_t offset, uint16_t max_length);

void App_AppearanceReq(uint8_t conidx, uint32_t metainfo, uint16_t token);

void App_SlavePrefParamReq(uint8_t conidx, uint32_t metainfo, uint16_t token);

/* --------------------------------------------------------------------------------------------------
 * BLE Connection Configuration
 * ------------------------------------------------------------------------------------------------*/
void App_ConnParamUpdateReq(uint8_t conidx, uint32_t metainfo, const gapc_le_con_param_nego_t *p_param);

void App_ConnParamUpdateCmp(uint8_t conidx, uint32_t metainfo, const gapc_le_con_param_t *p_param);

/* --------------------------------------------------------------------------------------------------
 * BLE Bond Management
 * ------------------------------------------------------------------------------------------------*/
void App_ClearBondList(void);

/* --------------------------------------------------------------------------------------------------
 * Button Presses and Indicators
 * ------------------------------------------------------------------------------------------------*/
void App_ButtonShortPress(uint8_t index);

void App_ButtonMediumPress(uint8_t index);

void App_ButtonLongPress(uint8_t index);

void App_ButtonSuperLongPress(uint8_t index);

void App_ButtonDoublePress(uint8_t index);

void App_ButtonTriplePress(uint8_t index);

void App_ButtonShortIndicator(uint8_t index);

void App_ButtonMediumIndicator(uint8_t index);

void App_ButtonLongIndicator(uint8_t index);

void App_ButtonSuperLongIndicator(uint8_t index);

void App_ButtonDoubleIndicator(uint8_t index);

void App_ButtonTripleIndicator(uint8_t index);

/* --------------------------------------------------------------------------------------------------
 * Button and LED Interrupt Handlers
 * ------------------------------------------------------------------------------------------------*/
extern void GPIO0_IRQHandler(void);

extern void TIMER0_IRQHandler(void);

/* --------------------------------------------------------------------------------------------------
 * Setup
 * ------------------------------------------------------------------------------------------------*/
void App_BTConfig(void);

void App_DeviceConfigCmpCb(uint32_t event, uint16_t status);

/* -----------------------------------------------------------------------------------------------
 * Register App-Specific Handlers for GAP Events and Activities
 * ----------------------------------------------------------------------------------------------*/
void App_RegisterHandlers(void);

/* -----------------------------------------------------------------------------------------------
 * Prepare Advertisement and Scan response Data
 * ----------------------------------------------------------------------------------------------*/
void PrepareAdvScanData(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_MSG_HANDLER_H */
