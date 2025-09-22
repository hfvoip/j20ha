/**
 * @file app_bass.c
 * @brief Application actions involving Bluetooth battery service
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

/*
 * INCLUDE FILES
 ****************************************************************************************
 */

#include <app_bass.h>                // Battery Application Definitions
#include "rwip_config.h"             // SW configuration
#include "bass.h"                    // Device Information Profile Functions
#include "bass_msg.h"                //
#include "prf_types.h"               // Profile Common Types Definitions
#include "prf.h"
#include "ke_task.h"                 // Kernel

#include "gap.h"
#include "gapc_msg.h"
#include "gapc.h"
#include "gapm_msg.h"
#include "gapm.h"

#include <string.h>
#include "co_utils.h"

#include <swmTrace_api.h>

static bass_env_t app_bass_env;

static bass_cb_t app_batt_cb =
{
    .cb_batt_level_upd_cmp = AppBASS_BattLvlUpd_CmpCb,
    .cb_bond_data_upd      = AppBASS_BondData_UpdCb
};


/*
 * FUNCTION DEFINITIONS
 ****************************************************************************************
 */

/**
 * @brief Completion of battery level update
 *
 * @param[in] status Status of the procedure execution (see enum #hl_err)
 */
void AppBASS_BattLvlUpd_CmpCb(uint16_t status)
{
    return;
}

/**
 * @brief Inform that Bond data updated for the connection.
 *
 * @param[in] conidx        Connection index
 * @param[in] ntf_cfg       Notification Configuration
 */
void AppBASS_BondData_UpdCb(uint8_t conidx, uint8_t ntf_ind_cfg)
{
    app_bass_env.batt_ntf_cfg[conidx] = ntf_ind_cfg;

    return;
}

/**
 * @brief Initialize BASS server service environment and configure message handlers.
 *
 * @param[in] bas_nb                    number of battery instances
 * @param[in] readBattLevelCallback     Pointer to the battery level read function
 * @param[in] notif_timeout             Battery level notification timeout
 * @param[in] lvl_monitor_timeout       Battery level monitoring timeout
 *
 */
void AppBASS_Initialize(uint8_t bas_nb,
                            uint8_t (*readBattLevelCallback)(void *p_bas_nb),
                            uint32_t notif_timeout,
                            uint32_t lvl_monitor_timeout)
{
    memset(&app_bass_env, 0, sizeof(bass_env_t));

    app_bass_env.bas_nb = bas_nb;
    app_bass_env.readBattLevelCallback = readBattLevelCallback;

    app_bass_env.battLevelNotificationTimeout = notif_timeout;
    app_bass_env.battLevelMonitoringTimeout = lvl_monitor_timeout;

    co_timer_periodic_config(&app_bass_env.notif_timer, AppBASS_BattLvl_Notif_Timeout);
    co_timer_periodic_config(&app_bass_env.lvl_monitor_timer, AppBASS_BattLvl_Monitor_Timeout);
}


/**
 * @brief Add the Battery Service Profile
 *
 * @return Status of the prf_add_profile execution (see enum #hl_err)
 */
uint16_t AppBASS_AddProfile(void)
{
	uint16_t handle = 0;
	struct bass_db_cfg db_cfg;

	// Add a BAS instance
    db_cfg.bas_nb = app_bass_env.bas_nb;

    // Sending of notifications is supported
    // update the Battery Level Characteristic Presentation Format
    // for each instance of the battery service

    for (uint8_t i = 0; i < db_cfg.bas_nb; i++)
    {
        db_cfg.features[i] = BAS_BATT_LVL_NTF_SUP;
        db_cfg.batt_level_pres_format[i].description = 0;
        db_cfg.batt_level_pres_format[i].exponent = 0;
        db_cfg.batt_level_pres_format[i].format = 0x4;
        db_cfg.batt_level_pres_format[i].name_space = 1;
        db_cfg.batt_level_pres_format[i].unit = GATT_UNIT_PERCENTAGE;
    }

    uint16_t status = prf_add_profile(TASK_ID_BASS,
                        0x00 /* Security - should be SVC_SEC_LVL(NO_AUTH) */,
                        0x0 /* user Priority*/,
                        (void*)&db_cfg,
                        (void*)&app_batt_cb,
                        &handle);

    if(status == GAP_ERR_NO_ERROR)
    {
        app_bass_env.is_profile_added = true;
    }

    return status;
}

/**
 * @brief Reset the BASS server service environment
 *
 * @param[in] conidx               Connection index
 *
 */
void AppBASS_Disable(uint8_t conidx)
{
    app_bass_env.enabled[conidx] = false;

    /* Stop timers, if needed */
    // TODO:
}

/**
 * @brief Restore bond data of a known peer device (at connection establishment)
 *
 * @param[in] conidx               Connection index
 * @param[in] notif_config         Notification Configuration
 * @param[in] p_prev_batt_lvl      Old Battery Level used to decide if notification should be triggered
 *                                 Array of BASS_NB_BAS_INSTANCES_MAX size.
 *
 * @return Status of the function execution (see enum #hl_err)
 */
uint16_t AppBASS_Enable(uint8_t conidx, uint8_t notif_config, const uint8_t* p_prev_batt_lvl)
{
    uint16_t status = bass_enable(conidx, notif_config, p_prev_batt_lvl);

    if(status == GAP_ERR_NO_ERROR)
    {
        app_bass_env.enabled[conidx] = true;

        for (uint8_t i = 0; i < app_bass_env.bas_nb; i++)
        {
            AppBASS_BattLvl_Update(i, app_bass_env.readBattLevelCallback(&i));    /* Notify */
        }

        /* Start timers if needed */
        co_timer_periodic_start(&app_bass_env.notif_timer,
                                        app_bass_env.battLevelNotificationTimeout);

        co_timer_periodic_start(&app_bass_env.lvl_monitor_timer,
                                        app_bass_env.battLevelMonitoringTimeout);
    }

    return status;
}

/**
* @brief Update a battery level
*
* Wait for #cb_batt_level_upd_cmp execution before starting a new procedure
*
* @param[in] svc_instance  Battery service instance
* @param[in] new_batt_level    New Battery level
*
*/
void AppBASS_BattLvl_Update(uint8_t svc_instance, uint8_t new_batt_level)
{
    app_bass_env.lastBattLevel[svc_instance] = new_batt_level;

    bass_batt_level_upd(svc_instance, new_batt_level);
}


/**
 * @brief Update a battery level periodically (notification)
 *
 * @param[in] p_env     Pointer to be passed when timer expires
 *
 */
void AppBASS_BattLvl_Notif_Timeout(co_timer_periodic_t* p_timer)
{
    if(app_bass_env.is_profile_added && CommonGAP_ConnectionCountGet() > 0)
    {
        for (uint8_t i = 0; i < app_bass_env.bas_nb; i++)
        {
            uint8_t battLvl = app_bass_env.readBattLevelCallback(&i);
            AppBASS_BattLvl_Update(i, battLvl);
        }
    }
    else
    {
        co_timer_periodic_stop(&app_bass_env.notif_timer);
    }
}


/**
 * @brief Update a battery level when it changes (monitor)
 *
 * @param[in] p_env     Pointer to be passed when timer expires
 *
 */
void AppBASS_BattLvl_Monitor_Timeout(co_timer_periodic_t* p_timer)
{
    if(app_bass_env.is_profile_added && CommonGAP_ConnectionCountGet() > 0)
    {
        for (uint8_t i = 0; i < app_bass_env.bas_nb; i++)
        {
            uint8_t battLvl = app_bass_env.readBattLevelCallback(&i);

            if(battLvl != app_bass_env.lastBattLevel[i])
            {
                AppBASS_BattLvl_Update(i, app_bass_env.readBattLevelCallback(&i));
            }
        }
    }
    else
    {
        co_timer_periodic_stop(&app_bass_env.notif_timer);
    }
}
