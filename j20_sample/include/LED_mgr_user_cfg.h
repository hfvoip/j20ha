/**
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
 *
 * @file LED_mgr_user_cfg.h
 * @brief User-configurable header file for specifying LED manager parameters
 * @details
 */

#ifndef LED_MGR_USER_CFG_H
#define LED_MGR_USER_CFG_H

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/*************************************************************************************************
 * Include files
 *************************************************************************************************/
/* Inclusion of system and local header files go here */

#include "LED_mgr.h"

/*************************************************************************************************
 * Symbolic constants
 *************************************************************************************************/
/* #define and enum of public statements go here */

/* This can be used at the application level to toggle the inclusion of all LED_manager code */
#define LED_MANAGER_ENABLED 1
#if (LED_MANAGER_ENABLED == 1)
    /* This is where the user specifies how many GPIOs are used for LEDs */
    #define LED_USER_NUM_GPIOS 2
    #if (LED_USER_NUM_GPIOS > LED_MAX_NUM_GPIOS)
        #error /* Only up to 3 LEDs are supported */
    #endif /* (LED_USER_NUM_GPIOS > LED_MAX_NUM_GPIOS) */

    /*
     * This is where the user specifies the GPIOs used for each active LED,
     * as well as the active state of the LEDs (active high or active low)
     */
    #if (LED_USER_NUM_GPIOS > 0)
        #if (RSL20_CID == 101)
            #define LED_USER_GPIO_0_NUM ((GPIO_SEL_t) GPIO_8)
        #else /* (RSL20_CID == 101) */
            #define LED_USER_GPIO_0_NUM ((GPIO_SEL_t) GPIO_11)
        #endif /* (RSL20_CID == 101) */
        #define LED_USER_GPIO_0_INDEX 0
        #define LED_USER_GPIO_ACTIVE_LEVEL ((LED_ActiveState_t) LED_ACTIVE_LOW)
    #endif /* (LED_USER_NUM_GPIOS > 0) */

    #if (LED_USER_NUM_GPIOS > 1)
        #if (RSL20_CID == 101)
            #define LED_USER_GPIO_1_NUM ((GPIO_SEL_t) GPIO_10)
        #else /* (RSL20_CID == 101) */
            #define LED_USER_GPIO_1_NUM ((GPIO_SEL_t) GPIO_4)
        #endif /* (RSL20_CID == 101) */
        #define LED_USER_GPIO_1_INDEX 1
    #endif /* (LED_USER_NUM_GPIOS > 1) */

    #if (LED_USER_NUM_GPIOS > 2)
        #define LED_USER_GPIO_2_NUM ((GPIO_SEL_t) GPIO_x)
        #define LED_USER_GPIO_2_INDEX 2
    #endif /* (LED_USER_NUM_GPIOS > 2) */

    /*
     * This is where the user specifies whether SysTick or one of the TIMERx peripherals should be
     * used for the LED manager
     * If the latter is selected, one of TIMER0, TIMER1, TIMER2, or TIMER3 must be specified
     */
    #define LED_USER_CLOCK_USE_TIMERx 1
    #if LED_USER_CLOCK_USE_TIMERx
        #define LED_USER_CLOCK_SOURCE TIMER0
    #else
        #define LED_USER_CLOCK_USE_SYSTICK 1
    #endif /* LED_USER_CLOCK_USE_TIMERx */

    /*
     * This is where the user informs the LED manager about the frequency and operating period of the
     * chosen timekeeping peripheral (SysTick or TIMERx), so that time can be tracked accurately
     * Note that these settings do not configure the selected timekeeping peripheral; the user
     * is responsible for doing that independently in application code prior to using this library
     */
    #define LED_TIMER_FREQ_HZ 250000
    #define LED_TIMER_PERIOD_MS 25
    #define LED_TIMER_CYCLES_PER_MS (LED_TIMER_FREQ_HZ / 1000)
    #define LED_TIMER_PERIOD_CYCLES ((uint32_t) (LED_TIMER_CYCLES_PER_MS * LED_TIMER_PERIOD_MS))

    /* This is where the user can specify the 3b priority of the timekeeping peripheral interrupt */
    #define LED_TIMER_INTERRUPT_PRIORITY 7
#endif /* (LED_MANAGER_ENABLED == 1) */

/*************************************************************************************************
 * Macros
 *************************************************************************************************/
/* Definition of public function-like macros go here */

/*************************************************************************************************
 * Type Definitions
 *************************************************************************************************/
/* Definition of public data types go here */

/*************************************************************************************************
 * Function Prototypes
 *************************************************************************************************/
/* Function prototypes for public functions go here */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LED_MGR_USER_CFG_H */
