/**
 * @file app_batt.c
 * @brief Handlers for measuring/reading battery level - for the BASS
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
#include <app_batt.h>
#include <hw.h>
#include <swmTrace_api.h>
#include <app_bass.h>

/* Global variable definitions */

/* Struct containing values needed for battery level calculation */
static struct app_batt_read_t app_batt_read;

/**
 * @brief  Calculate the current battery level by mapping measured (accumulated)
 *         voltage from [MIN_VOLTAGE_MV, MAX_VOLTAGE_MV] to [0, 100]
 *
 * @param[in] p_bas_nb     pointer to the number of battery service instances.
 */
uint8_t AppBatt_ReadBattLevel(void *p_bas_nb)
{
    uint8_t batt_lvl_percent;
    uint32_t batt_lvl_mV;

    if (app_batt_read.read_cnt == LSAD_READS_NUM)
    {
        /* Average reads */
        batt_lvl_mV = (uint32_t) (app_batt_read.lsad_sum_mV / LSAD_READS_NUM);

        /* If batt_lvl_mV is less than MIN_VOLTAGE_MV then set bat_lvl_percent to 0, since
         * the value would overflow otherwise */
        if (batt_lvl_mV < MIN_VOLTAGE_MV)
        {
            batt_lvl_percent = 0;
        }
        else
        {
            /* Calculating bat_lvl_percent using average voltage measured. The
             * voltage is scaled from [MIN_VOLTAGE_MV, MAX_VOLTAGE_MV] to [0, 100] */
            batt_lvl_percent = (uint8_t)(((batt_lvl_mV - MIN_VOLTAGE_MV) * 100) /
                                        (MAX_VOLTAGE_MV - MIN_VOLTAGE_MV));

            /* If measured voltage is less than MIN_VOLTAGE_MV set bat_lvl_percent to 100 */
            batt_lvl_percent = (batt_lvl_percent <= 100) ? batt_lvl_percent : 100;
        }

        app_batt_read.prev_batt_lvl_percent = batt_lvl_percent;

        /* Set to 0 before next set of LSAD readings are taken in AppBatt_BattLevelReadHandler */
        app_batt_read.lsad_sum_mV = 0;

        /* Set to 0 to begin next set of LSAD readings in AppBatt_BattLevelReadHandler */
        app_batt_read.read_cnt = 0;

    }
    else
    {
        /* Return the last battery level calculated if a set of LSAD readings has not been
         * completed since the last time the function was called */
        batt_lvl_percent = app_batt_read.prev_batt_lvl_percent;
    }

    return batt_lvl_percent;
}

/**
 * @brief  Periodically reads and accumulates the battery level.
 *
 * @param[in] p_env     pointer to the data passed by the timer.
 */
void AppBatt_BattLevelReadHandler(co_timer_t* p_timer)
{
    uint32_t voltage;

    /* Set timer to call AppBatt_BattLevelReadHandler to take another LSAD reading if needed */
    co_timer_start(&app_batt_read.read_timer, TIMER_SETTING_MS(LSAD_READ_INTERVAL_MS));

    if (app_batt_read.read_cnt < LSAD_READS_NUM)
    {
        voltage = ((LSAD_VOLTAGE_RANGE_MV * LSAD_VBAT_FACTOR *
                        LSAD->ABS_DATA_SAT[LSAD_VBAT_CHANNEL]) / (LSAD_MAX_SIZE));

        app_batt_read.lsad_sum_mV += voltage;

        app_batt_read.read_cnt++;
    }

}

/**
 * @brief Initializes LSAD and the app_batt_read structure.
 *
 * @param[in] None
 * @param[in] None
 */

void AppBatt_ReadBattLevelInit(void)
{
    uint32_t i = 0;

    /* Configure ADC */
    /* Full VBAT range, normal mode, run LSAD @ 625Hz/channel.
     * SLOWCLK = 1 MHz */
    LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_1600;

    /* Clear all interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    /* Turn off automatic compensation */
    for (i = 0; i < LSAD_NUM_CHANNELS; i++)
    {
        LSAD->INPUT_SEL[i] =  LSAD_POS_INPUT_VBAT_DIV3 | LSAD_NEG_INPUT_VBAT_DIV3;
    }

    /* Load LSAD trim settings from NVR */
    Sys_Trim_Load_LSADTrim((uint32_t *)(COPIER_SELECT_NVR | LSAD_TRIMS_OFFSET));

    app_batt_read.lsad_sum_mV = 0;
    app_batt_read.prev_batt_lvl_percent = 0;
    app_batt_read.read_cnt = 0;

    co_timer_config(&app_batt_read.read_timer, AppBatt_BattLevelReadHandler);
}

/**
 * @brief Initializes LSAD input channels.
 *
 * @param[in] None
 * @param[in] None
 */

void LSAD_ChannelInit(void)
{
    AppBatt_ReadBattLevelInit();

    Sys_LSAD_InputConfig(LSAD_VBAT_CHANNEL,
                             LSAD_POS_INPUT_VBAT_DIV3 | LSAD_NEG_INPUT_VREF,
                             -1, -1);
}
