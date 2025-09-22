/**
 * @file app_bass.h
 * @brief Bluetooth battery service server header
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

#ifndef APP_BASS_H_
#define APP_BASS_H_

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

#include "rwip_config.h"                // SW Configuration
#include <stdint.h>
#include <bt_protocol_support.h>
#include <common_gap.h>
#include "prf_types.h"                  // Profile Common Types Definitions

/*
 * DEFINES
 ****************************************************************************************
 */


/*
 * GLOBAL VARIABLES DECLARATION
 ****************************************************************************************
 */

typedef struct
{
    /* Number of battery instances [1,BASS_NB_BAS_INSTANCES_MAX] */
    uint8_t bas_nb;

    /* The current value of CCCD of battery value that has been set by
     * the client device */
    uint8_t batt_ntf_cfg[APP_MAX_NB_CON];

    /* The flag that indicates that service has been enabled */
    bool enabled[APP_MAX_NB_CON];

    co_timer_periodic_t notif_timer;
    co_timer_periodic_t lvl_monitor_timer;

    uint32_t battLevelNotificationTimeout;
    uint32_t battLevelMonitoringTimeout;

    uint8_t lastBattLevel[BASS_NB_BAS_INSTANCES_MAX];

    /* Pointer to an application function that returns the battery level */
    uint8_t (*readBattLevelCallback)(void *p_bas_nb);

    /* Profile added status*/
    bool is_profile_added;
} bass_env_t;

/*
 * GLOBAL FUNCTIONS DECLARATION
 ****************************************************************************************
 */
void AppBASS_Initialize(uint8_t bas_nb,
                            uint8_t (*readBattLevelCallback)(void *p_bas_nb),
                            uint32_t notif_timeout,
                            uint32_t lvl_monitor_timeout);

void AppBASS_BattLvlUpd_CmpCb(uint16_t status);

void AppBASS_BondData_UpdCb(uint8_t conidx, uint8_t ntf_ind_cfg);

uint16_t AppBASS_AddProfile(void);

void AppBASS_Disable(uint8_t conidx);

uint16_t AppBASS_Enable(uint8_t conidx, uint8_t notif_config, const uint8_t* p_prev_batt_lvl);

void AppBASS_BattLvl_Update(uint8_t svc_instance, uint8_t new_batt_level);

void AppBASS_BattLvl_Notif_Timeout(co_timer_periodic_t* p_timer);

void AppBASS_BattLvl_Monitor_Timeout(co_timer_periodic_t* p_timer);
/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif //APP_BASS_H
