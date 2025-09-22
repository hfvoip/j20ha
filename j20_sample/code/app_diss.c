/**
 * @file app_diss.c
 * @brief Application specific action handlers for device information service server
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

#include <app_diss.h>                 /* Device Information Service Application Definitions */
#include "rwip_config.h"              /* SW configuration */

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include "app.h"                     /* Application Manager Definitions */
#include "diss.h"                    /* Device Information Profile Functions */
#include "diss_msg.h"
#include "prf_types.h"               /* Profile Common Types Definitions */
#include "prf.h"
#include "ke_task.h"                 /* Kernel */

#include "gap.h"
#include "gapc_msg.h"
#include "gapc.h"
#include "gapm_msg.h"
#include "gapm.h"

#include <string.h>
#include "co_utils.h"

/* Global variable definition */

static diss_env_t app_diss_env;

static diss_cb_t app_diss_cb =
{
    .cb_value_get = AppDISS_DeviceInfoValueReqInd_Cb,
};

/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 * @brief               Handle the indication when peer device requests a value
 *
 * @param[in] token     Procedure token that must be returned in confirmation function
 * @param[in] val_id    Requested value identifier
 *
 * @return None
 */

void AppDISS_DeviceInfoValueReqInd_Cb(uint32_t token, uint8_t val_id)
{
    /* Initialize length */
    uint8_t len = 0;
    /* Pointer to the data */
    uint8_t *data = NULL;
    uint16_t retval = GAP_ERR_NO_ERROR;

    const diss_device_info_t *deviceInfo = app_diss_env.deviceInfo;

    /* Check requested value and set information */
    switch (val_id)
    {
        case DIS_VAL_MANUFACTURER_NAME :
        {
            len = deviceInfo->MANUFACTURER_NAME.len;
            data = deviceInfo->MANUFACTURER_NAME.data;
        } break;

        case DIS_VAL_MODEL_NB_STR :
        {
            len = deviceInfo->MODEL_NB_STR.len;
            data = deviceInfo->MODEL_NB_STR.data;
        } break;

        case DIS_VAL_SYSTEM_ID :
        {
            len = deviceInfo->SYSTEM_ID.len;
            data = deviceInfo->SYSTEM_ID.data;
        } break;

        case DIS_VAL_PNP_ID :
        {
            len = deviceInfo->PNP.len;
            data = deviceInfo->PNP.data;
        } break;

        case DIS_VAL_SERIAL_NB_STR :
        {
            len = deviceInfo->SERIAL_NB_STR.len;
            data = deviceInfo->SERIAL_NB_STR.data;
        } break;

        case DIS_VAL_HARD_REV_STR :
        {
            len = deviceInfo->HARD_REV_STR.len;
            data = deviceInfo->HARD_REV_STR.data;
        } break;

        case DIS_VAL_FIRM_REV_STR :
        {
            len = deviceInfo->FIRM_REV_STR.len;
            data = deviceInfo->FIRM_REV_STR.data;
        } break;

        case DIS_VAL_SW_REV_STR :
        {
            len = deviceInfo->SW_REV_STR.len;
            data = deviceInfo->SW_REV_STR.data;
        } break;

        case DIS_VAL_IEEE :
        {
            len = deviceInfo->IEEE.len;
            data = deviceInfo->IEEE.data;
        } break;

        default:
        {
            /* Invalid value id. IGNORE */
        } break;
    }

    /* Allocate confirmation to send the value */
    retval = diss_value_cfm(token, len, &data[0]);
  //  swmLogInfo("    DIS info sent to requesting peer. status = 0x%x\r\n", retval);

}

/**
 * @brief Add a Device Information Service instance in the DB
 *
 * @param[in] None
 * @param[in] None
 *
 * @return returns the status of prf_add_profile()
 */
uint16_t AppDISS_AddProfile(void)
{
	uint16_t handle = 0;
	struct diss_db_cfg db_cfg;

	db_cfg.features = app_diss_env.features;

	uint16_t status = prf_add_profile(TASK_ID_DISS,
                        0x00 /* Security - should be SVC_SEC_LVL(NO_AUTH) */,
                        0x0 /* user Priority*/,
                        (void*)&db_cfg,
                        (void*)&app_diss_cb,
                        &handle);

	if(status == GAP_ERR_NO_ERROR)
	{
	    app_diss_env.serviceAdded = true;
	}

	return status;
}

/**
 * @brief   Initialize device information server service environment and configure
 *          message handlers
 *
 * @param[in] features The enabled features for the device information
 *                  service. Values define as enum diss_features in \ref diss_task.h
 * @return  0 if the initialization was successful, < 0 otherwise
 */
void AppDISS_Initialize(uint16_t features, const diss_device_info_t *deviceInfo)
{
    app_diss_env.features = features;
    app_diss_env.deviceInfo = deviceInfo;
}
