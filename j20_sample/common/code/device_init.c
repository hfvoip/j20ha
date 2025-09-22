/**
 * @file device_init.c
 * @brief The common function containing the required settings for sample codes
 *
 * @copyright @parblock
 * Copyright (c) 2024 Semiconductor Components Industries, LLC (d/b/a
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

#include <stdio.h>

#include "device_init.h"
#include "MRAM_rom.h"
#include "sassert.h"

/* GPIOs should be above this voltage (in mV) for ACS_PAD_CFG_HIGH_VDDO to be enabled */
#define ACS_PAD_CFG_THRESHOLD           1150

/* LSAD reading to voltage (in mV) converstion */
#define CONVERT_LSAD_OUTPUT(x)          ((uint32_t)(((x * 9000) >> 13) / 10))

/* Number of GPIOs that can be used as an input to the LSAD at one time */
#define NUM_POS_INPUT_SEL               4

#ifndef BL_VERSION_OVERRIDE
#include <bl_defines.h>

BL_BOOT_VERSION("APP", 1, 0, 0);
#endif /* BL_VERSION_OVERRIDE */

/* Global Variable for MRAM_ECC_IRQHandler */
uint32_t ecc_uncorrected_error_count;

/* Global Variable for power supply config applied in Device_Initialize() */
#ifdef BOOTLOADER
PowerConfig_t power_supply_cfg = OTHER_VCC_LDO_VDDA_LDO;
#else
PowerConfig_t power_supply_cfg = ZNAIR_VCC_LDO_VDDA_CP;
#endif

/**
 * @brief Measure VDDO using the LSAD and set PAD_CFG appropriately
 */
static void Set_PAD_CFG(void);

/**
 * @brief Initialize the system, including GPIOs and interrupts.
 */
uint32_t Device_Initialize(void)
{
    uint32_t error = ERROR_NO_ERROR;

    /* Interrupts off */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();

    /* Set MRAM cache as invalid, restarting the MRAM
     * cache from a fresh state */
    SYSCTRL->MRAM_CACHE_CFG = MRAM_CACHE_VALID_RESET;

    /* Configure MRAM cache as specified in device_init.h */
    SYSCTRL->MRAM_CACHE_CFG = MRAM_CACHE_CONFIG;

#ifndef BOOTLOADER
    /* Check for recovery GPIO to enable recovery. */
    SYS_GPIO_CONFIG(DEBUG_CATCH_GPIO, (GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE |
                                    GPIO_WEAK_PULL_UP | GPIO_2X_DRIVE));

    while ((Sys_GPIO_Read(DEBUG_CATCH_GPIO)) == 0)
    {
        SYS_WATCHDOG_REFRESH();
    }

#endif /* BOOTLOADER */

    error = SYS_TRIM_LOAD_DEFAULT();
    SYS_ASSERT(error == ERROR_NO_ERROR);

    /* Set optimal 32 kHz XTAL load capacitor trim value.   */
    ACS_XTAL32K_CTRL->CLOAD_TRIM_BYTE = RSL20_EVB_XTAL32K_TRIM_BYTE;

    /* Set optimal sleep mode trim values */
    ACS->SLEEP_MODE_CFG = (ACS->SLEEP_MODE_CFG & ~(ACS_SLEEP_MODE_CFG_RAM_BTRIM_Mask))
                           | BTRIM_OPTIMAL;

//#if APP_CPU_AT_FULL_SPEED
#if 1
    /* Enable the 48MHz operation */
    Sys_Clocks_XTALClkConfig(RFCLK_PRESCALE_1);
#else
    /* Enable the 48MHz XTAL at 16MHz */
    Sys_Clocks_XTALClkConfig(RFCLK_PRESCALE_3);
#endif  /* if APP_CPU_AT_FULL_SPEED */

    /* Switch to (divided 48 MHz) oscillator clock, and update the
     * SystemCoreClock global variable. */
    Sys_Clocks_SystemClkConfig(SYSCLK_CLKSRC_RFCLK);

    SYSCTRL_RF_XTAL_FREQ_TRIM_CFG->FREQ_TRIM_BYTE = RF_XTAL_FREQ_TRIM_EVB;

    /* Configure clock dividers */
    //Sys_Clocks_DividerConfig(USER_CLK);

    error |= Configure_Device_For_Supply(power_supply_cfg);
    SYS_ASSERT(error == ERROR_NO_ERROR);

    /* The GPIO pad charge pump should be set to AUTO mode
     * so it is enabled as needed. */
    ACS->PAD_CFG = PAD_PUMP_AUTO;

    /* Set pad configuration depending on VDDO level */
    Set_PAD_CFG();

    /* Configure Baseband Controller Interface */
    BB_IF->CTRL = (BB_CLK_ENABLE | BBCLK_DIVIDER_16);

    /* Enable interrupt to handle 3 bit ECC failures */
    NVIC_EnableIRQ(MRAM_ECC_IRQn);

    /* Enable clock detector reset */
    ACS->CLK_DET_CTRL |= CLK_DET_RESET_ENABLE;

    /* Disable JTAG Data TDI, TDO connections to GPIO 14 and 15.
     * Disabling these frees up those GPIOs to be used by the
     * application for other purposes. */
    GPIO->JTAG_SW_PAD_CFG &= ~CM33_JTAG_DATA_ENABLED;

    return error;
}

uint32_t Configure_Device_For_Supply(PowerConfig_t power_supply_cfg)
{
    uint32_t power_supply_mode = 0;
    uint32_t clk_div_cfg1_val = 0;
    uint32_t mram_vcc_seq_word_erase_write_boost = 0;
    uint32_t mram_vcc_sector_erase_boost = 0;
    uint32_t mram_seq_word_erase_write_cp_boost = SEQ_WORD_ERASE_WRITE_CP_BOOST_ENABLED;
    uint32_t mram_sector_erase_cp_boost = SECTOR_ERASE_CP_BOOST_ENABLED;
    uint32_t mram_seq_word_dcdc_boost = SEQ_WORD_ERASE_WRITE_DCDC_BOOST_ENABLED;
    uint32_t mram_sector_erase_dcdc_boost = SECTOR_ERASE_DCDC_BOOST_ENABLED;

    /* Enable the CPCLK temporarily as a pre-caution */
    CLK->DIV_CFG1 &= ~CPCLK_DISABLE;

    switch (power_supply_cfg)
    {
        case ZNAIR_VCC_LDO_VDDA_CP:
        case LIION_VCC_LDO_VDDA_CP:
            power_supply_mode = (VCC_MODE_LDO | VDDA_MODE_CP);
            clk_div_cfg1_val = CPCLK_PRESCALE_8;
            mram_vcc_seq_word_erase_write_boost = VCC_BOOST_SEQ_ERASE_WRITE;
            mram_vcc_sector_erase_boost = VCC_BOOST_SECTOR_ERASE;
            break;
        case ZNAIR_VCC_BUCK_VDDA_CP:
            power_supply_mode = (VCC_MODE_DCDC | VDDA_MODE_CP);
            clk_div_cfg1_val = CPCLK_PRESCALE_8;
            mram_vcc_seq_word_erase_write_boost = VCC_BOOST_SEQ_ERASE_WRITE;
            mram_vcc_sector_erase_boost = VCC_BOOST_SECTOR_ERASE;
            break;
        case LIION_VCC_CP_VDDA_BUCK:
            power_supply_mode = (VCC_MODE_CP | VDDA_MODE_DCDC);
            clk_div_cfg1_val = CPCLK_PRESCALE_8;
            mram_vcc_seq_word_erase_write_boost = 0;
            mram_vcc_sector_erase_boost = 0;
            break;
        case LIION_VCC_CP_VDDA_LDO:
            power_supply_mode = (VCC_MODE_CP | VDDA_MODE_LDO);
            clk_div_cfg1_val = CPCLK_PRESCALE_8;
            mram_vcc_seq_word_erase_write_boost = 0;
            mram_vcc_sector_erase_boost = 0;
            break;
        case OTHER_VCC_LDO_VDDA_LDO:
            power_supply_mode = (VCC_MODE_LDO | VDDA_MODE_LDO);
            mram_vcc_seq_word_erase_write_boost = 0;
            mram_vcc_sector_erase_boost = 0;
#ifndef BOOTLOADER
            /* If the charge pump is not used, disable it */
            clk_div_cfg1_val = CPCLK_PRESCALE_8 | CPCLK_DISABLE;
#else
            clk_div_cfg1_val = CPCLK_PRESCALE_8 | CPCLK_ENABLE;
#endif /* BOOTLOADER */
            break;
        default:
            /* Invalid power supply configuration */
            return ERROR_INVALID_POWER_CFG;
    }

    if ((power_supply_cfg != LIION_VCC_CP_VDDA_BUCK) && (power_supply_cfg != LIION_VCC_CP_VDDA_LDO))
    {
        Sys_Trim_Load_DCDC(TRIM, TARGET_VCC_MODE_LDO_840, TARGET_VDDA_MODE_LDO_1680);
    }
    else
    {
        Sys_Trim_Load_DCDC(TRIM, TARGET_VCC_MODE_LDO_955, TARGET_VDDA_MODE_LDO_1910);
    }

    /* MRAM boost configuration */
    SYSCTRL->MRAM_BOOST_CFG =
          mram_seq_word_erase_write_cp_boost
        | mram_sector_erase_cp_boost
        | (mram_vcc_seq_word_erase_write_boost << SYSCTRL_MRAM_BOOST_CFG_SEQ_WORD_ERASE_WRITE_VCC_BOOST_Pos)
        | (mram_vcc_sector_erase_boost << SYSCTRL_MRAM_BOOST_CFG_SECTOR_ERASE_VCC_BOOST_Pos)
        | mram_seq_word_dcdc_boost
        | mram_sector_erase_dcdc_boost;

#ifndef BOOTLOADER
    /* Set VCC and VDDA operating modes, disable forcing of the CP_CLK. */
    ACS->DCDC_CTRL = ((ACS->DCDC_CTRL & ~(ACS_DCDC_CTRL_VDDA_MODE_Mask |
                      ACS_DCDC_CTRL_VCC_MODE_Mask | CP_CLK_FORCE_ENABLE))
                     | power_supply_mode | CP_CLK_AUTO_ENABLE);
#else
    /* Set VCC and VDDA operating modes, enable forcing of the CP_CLK. */
    ACS->DCDC_CTRL = ((ACS->DCDC_CTRL & ~(ACS_DCDC_CTRL_VDDA_MODE_Mask |
            ACS_DCDC_CTRL_VCC_MODE_Mask | CP_CLK_FORCE_ENABLE))
                     | power_supply_mode | CP_CLK_FORCE_ENABLE);
#endif /* BOOTLOADER */

    CLK->DIV_CFG1 = (CLK->DIV_CFG1 & ~(CLK_DIV_CFG1_CPCLK_PRESCALE_Mask |
                     CPCLK_DISABLE))
                    | clk_div_cfg1_val;

    return ERROR_NO_ERROR;
}

static void Set_PAD_CFG(void)
{
    /* Disable the LSAD and connect all inputs to default values */
    LSAD->CFG = LSAD_DISABLE;

    /* Clear all interrupts */
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    LSAD->CFG = LSAD_NORMAL | LSAD_PRESCALE_200; /* Normal mode, all 8 channels sampled */
    LSAD->INT_CFG =  LSAD_INT_CH7 | LSAD_INT_ENABLE;
    while (!(LSAD->MONITOR_STATUS & LSAD_READY_TRUE)); /* Filter out initial samples */
    LSAD->MONITOR_STATUS |= LSAD_READY_CLEAR;

    bool above_threshold_flag = false;
    /*
     * Iterate through all 16 GPIO Pins to measure the voltage
     * We only have 4 POS_INPUT_SEL so we have to measure 4 GPIOs at one time
     */
    for (size_t i = 0; i < GPIO_PAD_COUNT; i++)
    {
        uint32_t lsad_inp_idx = i % NUM_POS_INPUT_SEL;
        /* LSAD Input Configuration. Positive: GPIO, Negative: Vref */
        Sys_LSAD_InputConfig(
            lsad_inp_idx, (lsad_inp_idx << LSAD_INPUT_SEL_POS_INPUT_SEL_Pos) | LSAD_NEG_INPUT_VREF,
            i, -1);
        GPIO->CFG[i] |= GPIO_WEAK_PULL_UP;

        /* 4 inputs setup, ready to take measurement */
        if (lsad_inp_idx == (NUM_POS_INPUT_SEL - 1))
        {
            /* Turn on interrupt with given channel_number */
            LSAD->INT_CFG = LSAD_INT_CH3 | LSAD_INT_ENABLE;
            while (!(LSAD->MONITOR_STATUS & LSAD_READY_TRUE));
            LSAD->MONITOR_STATUS |= LSAD_READY_CLEAR;

            /* Measurement ready, check results */
            for (size_t j = 0; j < NUM_POS_INPUT_SEL; j++)
            {
                if (CONVERT_LSAD_OUTPUT(LSAD->ABS_DATA_SAT[j]) >= ACS_PAD_CFG_THRESHOLD)
                {
                    above_threshold_flag = true;

                    /* Break out of outer loop */
                    i = GPIO_PAD_COUNT;
                    break;
                }
            }
        }
    }

    LSAD->INT_CFG = LSAD_INT_DISABLE;
    LSAD->CFG = LSAD_DISABLE;
    LSAD->MONITOR_STATUS = MONITOR_ALARM_CLEAR |
                           LSAD_OVERRUN_CLEAR |
                           LSAD_READY_CLEAR;

    if (above_threshold_flag)
    {
        ACS->PAD_CFG |= HIGH_VDDO_ENABLED;
    }
    else
    {
        ACS->PAD_CFG &= ~HIGH_VDDO_ENABLED;
    }
}

/**
 * @brief By default interrupt masks are disabled. If application needs
 *  interrupt, this function should be called after Device_Init function
 *
 */
void Unmask_Interrupts(void)
{
    /* Enabling the interrupt masks */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}

/**
 * @brief Handle 3 bit ECC Failures.
 */
void MRAM_ECC_IRQHandler(void)
{
    if ((MRAM->ECC_STATUS & MRAM_ECC_UNCORRECTED_ERROR) == MRAM_ECC_UNCORRECTED_ERROR)
    {
        /* Below print statement can be uncommented if needed */
        /* printf("The address of the last failing word is: %lu", MRAM->ECC_ERROR_ADDR); */
        ecc_uncorrected_error_count++;
    }
}
