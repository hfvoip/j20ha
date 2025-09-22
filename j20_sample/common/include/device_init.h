/**
 * @file device_init.h
 * @brief The common function prototypes and definitions containing the required settings
 * for sample codes
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
#ifndef APP_INIT_H_
#define APP_INIT_H_
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
#include <hw.h>
#include "swmTrace_api.h"

/* ----------------------------------------------------------------------------
 * Definitions
 * ------------------------------------------------------------------------- */

/* GPIO definitions */
#define DEBUG_CATCH_GPIO                    GPIO7

/* Set user clock */
#define USER_CLK                            48000000

#ifndef APP_CPU_AT_FULL_SPEED
/* Set to 1 to run the SYSCLK at full speed (48 MHz) and set to 0 to run the
 * SYSCLK at reduced speed (16 MHz). */
#define APP_CPU_AT_FULL_SPEED               (1U)
#endif

/* Possible Batt/VCC/VDDA configurations. Please consult the Hardware Reference
 * Manual to determine what external components are required in each mode. */
typedef enum
{
    /* VCC in LDO mode and VDDA in charge pump mode are the default modes on
     * system power-up. Good efficiency when VBAT < 1.45V.
     * Suitable for a wide VBAT range, from 0.9 V - 4.5 V. */
    ZNAIR_VCC_LDO_VDDA_CP = 0,

    /* VCC in buck mode and VDDA in charge pump mode provides the best efficiency.
     * Suitable for a VBAT range of 1.1 - 2.0 V.
     * Very noisy mode. */
    ZNAIR_VCC_BUCK_VDDA_CP,

    /* VCC in charge pump mode and VDDA in buck mode also provides the best efficiency.
     * Suitable for a VBAT range of 2.5 - 4.5 V. */
    LIION_VCC_CP_VDDA_BUCK,

    /* VCC in charge pump mode and VDDA in LDO mode.
     * Suitable for a VBAT range of 2.0 - 4.5 V. */
    LIION_VCC_CP_VDDA_LDO,

    /* VCC in LDO mode and VDDA in charge pump mode used with a LiIon battery.
     * Suitable for a wide VBAT range of 0.9 - 4.5 V. */
    LIION_VCC_LDO_VDDA_CP,

    /* VCC in LDO mode and VDDA in LDO mode is the lowest noise mode.
     * A low-efficiency mode suitable only for development purposes.
     * Suitable for a VBAT range of 1.8 - 4.5 V. */
    OTHER_VCC_LDO_VDDA_LDO,

    MAX_POWER_SUPPLY_CONFIG_OPTIONS
} PowerConfig_t;

/* If less noise is necessary for low power RF TX operations or RF RX
 * operations, set this definition to 1 to force the usage of the
 * VDDA LDO. This will increase current consumption of those operations.
 * Not used when the buck converter is not used. */
#define FORCE_LOW_NOISE                     1

#ifndef TARGET_VDDA_MODE_DCDC_900
#define TARGET_VDDA_MODE_DCDC_900           TARGET_VDDA_MODE_LDO_1800
#endif /* ifndef TARGET_VDDA_MODE_DCDC_900 */

#ifndef TARGET_VCC_MODE_DCDC_900
#define TARGET_VCC_MODE_DCDC_900            TARGET_VCC_MODE_LDO_900
#endif /* ifndef TARGET_VCC_MODE_DCDC_900 */

/** MRAM VCC boost value for sequential erase write */
#define VCC_BOOST_SEQ_ERASE_WRITE           (8)

/** MRAM VCC boost value for sector erase */
#define VCC_BOOST_SECTOR_ERASE              (0xC)

/** RF XTAL Frequency Trim for EVB */
#if RSL20_CID == 102
#define RF_XTAL_FREQ_TRIM_EVB               (0x58)
#else /* RSL20_CID */
#define RF_XTAL_FREQ_TRIM_EVB               (0x8E)
#endif /* RSL20_CID */

/* Invalid PowerConfig_t value when calling Configure_Device_For_Supply() */
#define ERROR_INVALID_POWER_CFG             (1 << 31)

/* MRAM cache configuration
 * Possible cache size configurations:
 * MRAM cache size, more cache equals higher hit rates, but more
 * power usage from the cache itself.
 * - MRAM_CACHE_8KB
 * - MRAM_CACHE_16KB
 * - MRAM_CACHE_24KB
 * - MRAM_CACHE_32KB
 * Possible pre-fetchers configurations:
 * Will pre-fetch instructions or data respectively from MRAM.
 * - MRAM_CACHE_DATA_PREFETCH_ENABLE
 * - MRAM_CACHE_INSTR_PREFETCH_ENABLE
 * - MRAM_CACHE_DATA_PREFETCH_DISABLE
 * - MRAM_CACHE_INSTR_PREFETCH_DISABLE*/
#define MRAM_CACHE_CONFIG                   (MRAM_CACHE_ENABLE |\
                                             MRAM_CACHE_32KB |\
                                             MRAM_CACHE_DATA_PREFETCH_ENABLE |\
                                             MRAM_CACHE_INSTR_PREFETCH_ENABLE)

/* Global Variable for MRAM_ECC_IRQHandler */
extern uint32_t ecc_uncorrected_error_count;

/* Global Variable for power supply config applied in Device_Initialize() */
extern PowerConfig_t power_supply_cfg;

/* ----------------------------------------------------------------------------
 * Function Prototypes
 * ------------------------------------------------------------------------- */

/**
 * @brief Initializes device clock and sets the appropriate setting
 * @return ERROR_NO_ERROR or a HAL trim error code or ERROR_INVALID_POWER_CFG
 *         if using an invalid power supply configuration
 */
uint32_t Device_Initialize(void);

/**
 * @brief Initializes power supply settings based on the chosen power
 *        supply configuration
 * @param[in] power_supply_cfg Variable indicating the current power
 *                             supply configuration
 * @return ERROR_NO_ERROR or ERROR_INVALID_POWER_CFG if using an invalid
 *         power supply configuration
 */
uint32_t Configure_Device_For_Supply(PowerConfig_t power_supply_cfg);

/**
 * @brief Unmask interrupt function
 */
void Unmask_Interrupts(void);

/**
 * @brief   Handle 3 bit ECC Failures.
 */
extern void MRAM_ECC_IRQHandler(void);

#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif/* APP_INIT_H_ */
