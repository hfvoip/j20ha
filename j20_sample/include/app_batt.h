/**
 * @file app_batt.h
 * @brief Application header for battery service action handlers.
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

#ifndef APP_BATT_H
#define APP_BATT_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

#include <stdint.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/

#define CONCAT(x, y)                     x##y
#define BATMON_CH(x)                     CONCAT(BATMON_CH, x)

/* Note:
 *   The ble_peripheral_server sample code is only designed to work with APP_BAS_NB
 *   set to 1. */
#define APP_BAS_NB                       1  /* Number of batteries */
#define BATT_LEVEL_LOW_THRESHOLD_PERCENT 15 /* Battery level low at 15% of 1.1V to 1.4V range */

/* LSAD, VBAT and BATMON alarm configuration */
#define BATMON_ALARM_COUNT_CFG           1U

/* A set of constants that define the available power sources for the system */
#define BATTERY_LI_ION                  (0)
#define BATTERY_ZINC_AIR                (1)
#define REGULATOR_1V8                   (2)
#define REGULATOR_3V3                   (3)

/* The selected power source used. VBAT will be measured relative to this source. */
#define POWER_SOURCE                    (REGULATOR_3V3)

/* Maximum and minimum voltage values used as a reference in order to calculate
 * battery level percent. LSAD is accurate to +/- 5mV */
#if POWER_SOURCE == REGULATOR_3V3
    #define MAX_VOLTAGE_MV              (3300)
    #define MIN_VOLTAGE_MV              (900)
#elif POWER_SOURCE == REGULATOR_1V8
    #define MAX_VOLTAGE_MV              (1800)
    #define MIN_VOLTAGE_MV              (900)
#elif POWER_SOURCE == BATTERY_ZINC_AIR
    #define MAX_VOLTAGE_MV              (1500)
    #define MIN_VOLTAGE_MV              (900)
#elif POWER_SOURCE == BATTERY_LI_ION
    #define MAX_VOLTAGE_MV              (3700)
    #define MIN_VOLTAGE_MV              (2700)
#else /* if POWER_SOURCE == REGULATOR_3V3 */
    #define MAX_VOLTAGE_MV              (4500)
    #define MIN_VOLTAGE_MV              (900)
#endif /* if POWER_SOURCE == REGULATOR_3V3 */

/* VBAT LSAD Channel */
#define LSAD_VBAT_CHANNEL                3

/* Factor that VBAT is divided by */
#define LSAD_VBAT_FACTOR                 3

/* Compensation LSAD Channel */
#define LSAD_COMPENSATION_CHANNEL        7

/** Number of LSAD channels in device */
#define LSAD_NUM_CHANNELS               8

/* Since the LSAD output code is 14 bits, the LSAD output maximum size is 2^14 */
#define LSAD_MAX_SIZE                    0x4000

#define LSAD_VOLTAGE_RANGE_MV            1800

#define LSAD_READS_NUM                   16

/* Sample rate is SLOWCLK/1600 (78.125 Hz per channel), where SLOWCLK = 1MHz
 * so the value must be above 102.4 milliseconds in order to allow enough time
 * for all 8 channels to be sampled */
#define LSAD_READ_INTERVAL_MS            150

#define LSAD_CHANNEL_NUM                 8

/** Offset in NVR0 for address of LSAD trims */
#define LSAD_TRIMS_OFFSET               (0x1A0)

/* Interval for timer that calls BattLevelReadHandler the first time */
#define BEGIN_TIMER_S     1

/* Time interval to check for battery level changes and notify if necessary.
 * BASS_NotifyOnBattLevelChange uses this value to set a kernel timer for
 * the BASS_BATT_MONITORING_TIMEOUT event, which checks if the battery level
 * has changed and if yes, sends a battery level update request.
 * Should be greater than LSAD_READ_INTERVAL_MS / 1000 * LSAD_READS_NUM  */
#define BATT_CHANGE_TIMEOUT_S            5

/* Time interval to notify battery level. BASS_NotifyOnTimeout uses this value
 * to set a kernel timer for the BASS_BATT_LEVEL_NTF_TIMEOUT event, which
 * sends a battery level update request regardless of whether the previous
 * battery level was the same.
 * Should be greater than BATT_CHANGE_TIMEOUT_S */
#define BATT_UPDATE_TIMEOUT_S            15

#define LSAD_BATMON_CH                    6
#define LSAD_GND_CH                       0

/**
* @def         CONVERT(x)
* @brief       Converts an ADC code to a voltage, calculated as follows
*              voltage = adc_code * (1.8 V * 1000 [mV]/0.9 V / 2^14 steps.)
*              Add 5 to the result before dividing by 10 to take integer
*              truncation into account.
* @param[in]   x   the ADC code input
* @return      The voltage output in mV
* @assumptions Low frequency mode for the ADC is used, meaning that the
*              resolution of the ADC is 14-bits. CONVERT provides voltage
*              level as a milliVolt value based on the input ADC code.
*/
#define CONVERT(x)                      ((uint32_t)((((x * 9000) >> 13) + 5) / 10))

void AppBatt_SetBatMonAlarm(void);

uint8_t AppBatt_ReadBattLevel(void *p_bas_nb);

void AppBatt_BattLevelReadHandler(co_timer_t* p_timer);

void AppBatt_BattLevelLow_Handler(void);

void AppBatt_ReadBattLevelInit(void);

void LSAD_ChannelInit(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */

struct app_batt_read_t
{
	uint32_t lsad_sum_mV;
	uint8_t prev_batt_lvl_percent;
	uint8_t read_cnt;
	co_timer_t read_timer;
};


#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_BATT_H */

