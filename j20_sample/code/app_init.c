/**
 * @file app_init.c
 * @brief Source file for application initialization
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
#include "bt_config.h"


#if (LED_MANAGER_ENABLED == 1)
    /* LED blink period definitions, in milliseconds (only fast blink is used in this application) */
    #define LED_SLOW_BLINK_PERIOD LED_PERIOD_NOT_CONFIGURED
    #define LED_MEDIUM_BLINK_PERIOD LED_PERIOD_NOT_CONFIGURED
    #define LED_FAST_BLINK_PERIOD 400
#endif /* (LED_MANAGER_ENABLED == 1) */

static uint32_t traceOptions[] = {
		SWM_LOG_LEVEL_VERBOSE,              /* In all cases log verbose messages */
		SWM_UART_RX_PIN | UART_RX_GPIO,  /* Set RX pin for cases when using UART */
		SWM_UART_TX_PIN | UART_TX_GPIO,  /* Set TX pin for cases when using UART */
		SWM_UART_RX_ENABLE,              /* Enable the UART Rx Interrupts */
		SWM_UART_BAUD_RATE | UART_BAUD   /* Set Baud rate */

};

/* Device Information structure initialization, includes length and data string */
const diss_device_info_t deviceInfo =
{
    .MANUFACTURER_NAME  = {.len = APP_DIS_MANUFACTURER_NAME_LEN, .data = (uint8_t*) APP_DIS_MANUFACTURER_NAME},
    .MODEL_NB_STR       = {.len = APP_DIS_MODEL_NB_STR_LEN, .data = (uint8_t*) APP_DIS_MODEL_NB_STR},
    .SERIAL_NB_STR      = {.len = APP_DIS_SERIAL_NB_STR_LEN, .data = (uint8_t*) APP_DIS_SERIAL_NB_STR},
    .FIRM_REV_STR       = {.len = APP_DIS_FIRM_REV_STR_LEN, .data = (uint8_t*) APP_DIS_FIRM_REV_STR},
    .SYSTEM_ID          = {.len = APP_DIS_SYSTEM_ID_LEN, .data = (uint8_t*) APP_DIS_SYSTEM_ID},
    .HARD_REV_STR       = {.len = APP_DIS_HARD_REV_STR_LEN, .data = (uint8_t*) APP_DIS_HARD_REV_STR},
    .SW_REV_STR         = {.len = APP_DIS_SW_REV_STR_LEN, .data = (uint8_t*) APP_DIS_SW_REV_STR},
    .IEEE               = {.len = APP_DIS_IEEE_LEN, .data = (uint8_t*) APP_DIS_IEEE},
    .PNP                = {.len = APP_DIS_PNP_ID_LEN, .data = (uint8_t*) APP_DIS_PNP_ID},
};

#if (BUTTON_MANAGER_ENABLED == 1)
/* Callbacks to be registered for each button press type */
Button_Callbacks_t button_cbs[BUTTON_USER_NUM_GPIOS] =
{

    {
        App_ButtonShortPress,
        App_ButtonMediumPress,
        App_ButtonLongPress,
        App_ButtonSuperLongPress,
        App_ButtonDoublePress,
        App_ButtonTriplePress
    }
};

/* Indicator callbacks to be registered for each button press type */
Button_IndicatorCallbacks_t button_indicator_cbs =
{
    App_ButtonShortIndicator,
    App_ButtonMediumIndicator,
    App_ButtonLongIndicator,
    App_ButtonSuperLongIndicator,
    App_ButtonDoubleIndicator,
    App_ButtonTripleIndicator
};

/* Lengths to be assigned to each button press type */
Button_Lengths_t button_lengths =
{
    75,   /* Short press length = 75ms */
    500,  /* Medium press length = 500ms */
    1000, /* Long press length = 1000ms */
    5000, /* Super long press length = 5000ms */
    250   /* Multiple press timeout = 250ms */
};
#endif /*(BUTTON_MANAGER_ENABLED == 1) */

/* -----------------------------------------------------------------------------------------------
 * Initialize the Device
 * ----------------------------------------------------------------------------------------------*/
void App_Init(void)
{
    uint32_t ret_val;
    power_supply_cfg = LIION_VCC_CP_VDDA_LDO; //打开内部LDO

    /* Common initialization routine for all RSL20 apps */
    ret_val = Device_Initialize();

    /* Read BT public address */
    uint8_t app_public_addr[BD_ADDR_LEN] = APP_PUBLIC_ADDRESS;
    BT_SetPublicAddress(true, app_public_addr);

    /* Initialize trace library */
    swmTrace_init(traceOptions, 5);

    /* Print app device name */
    swmLogInfo("App device name: %s\r\n", APP_DEVICE_NAME);

    /* Print trim error if found when initializing device */
    if(ret_val != ERROR_NO_ERROR)
    {
        swmLogInfo("Trim Error: 0x%08x \r\n", (unsigned long)ret_val);
    }

    /* Initialization for button and LED GPIO handling */
    GPIOHandlerInit();

    /* Configuring LSAD input channels */
    LSAD_ChannelInit();
}

void GPIOHandlerInit(void)
{
#if (BUTTON_MANAGER_ENABLED == 1)
    /* Button GPIO pin array */
    GPIO_SEL_t button_GPIO_pins[BUTTON_USER_NUM_GPIOS] = {BUTTON_GPIO};

    /* Button GPIO and interrupt configuration */
    SYS_GPIO_CONFIG(BUTTON_GPIO,
                    (GPIO_MODE_GPIO_IN | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP | GPIO_2X_DRIVE));

    Sys_GPIO_IntConfig(BUTTON_GPIO_INDEX, (GPIO_INT_DEBOUNCE_ENABLE |
                                           #if (RSL20_CID == 101)
                                           GPIO_INT_SRC_GPIO_0 |
                                           #else /* !(RSL20_CID == 101) */
                                           GPIO_INT_SRC_GPIO_7 |
                                           #endif /* (RSL20_CID == 101) */
                                           GPIO_INT_EVENT_TRANSITION),
                                           DEBOUNCE_SLOWCLK_DIV1024,
                                           BUTTON_DEBOUNCE_COUNT);

    /* TIMER0 configuration for button handling */
    Sys_Timer_Stop(BUTTON_USER_CLOCK_SOURCE);
    Sys_Timer_Config(BUTTON_USER_CLOCK_SOURCE,
                     TIMER_PRESCALE_2,
                     (TIMER_FREE_RUN | TIMER_SLOWCLK_DIV2),
                     BUTTON_TIMER_PERIOD_CYCLES);
    Sys_Timer_Start(BUTTON_USER_CLOCK_SOURCE);

    /* Interrupt enabling and priority configuration */
    NVIC_SetPriority(TIMER0_IRQn, BUTTON_TIMER_INTERRUPT_PRIORITY);
    NVIC_SetPriority(GPIO0_IRQn, BUTTON_GPIO_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(TIMER0_IRQn);
    NVIC_EnableIRQ(GPIO0_IRQn);

    /* button_manager initialization */
    Button_Initialize(BUTTON_USER_NUM_GPIOS,
                      BUTTON_USER_GPIO_ACTIVE_LEVEL,
                      BUTTON_TIMER_PERIOD_MS,
                      button_GPIO_pins,
                      button_cbs,
                      &button_indicator_cbs,
                      &button_lengths);
#endif /* (BUTTON_MANAGER_ENABLED == 1) */

#if (LED_MANAGER_ENABLED == 1)
    /* LED GPIO pin array */
    GPIO_SEL_t LED_GPIO_pins[LED_USER_NUM_GPIOS] = {LED_STATE_GPIO, CONNECTION_STATE_GPIO};

    /* LED GPIO configuration */
    SYS_GPIO_CONFIG(LED_STATE_GPIO,
                    (GPIO_MODE_GPIO_OUT | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP | GPIO_2X_DRIVE));
    SYS_GPIO_CONFIG(CONNECTION_STATE_GPIO,
                    (GPIO_MODE_GPIO_OUT | GPIO_LPF_DISABLE | GPIO_WEAK_PULL_UP | GPIO_2X_DRIVE));

    /* TIMER0 configuration for LED handling */
    Sys_Timer_Stop(LED_USER_CLOCK_SOURCE);
    Sys_Timer_Config(LED_USER_CLOCK_SOURCE,
                     TIMER_PRESCALE_2,
                     (TIMER_FREE_RUN | TIMER_SLOWCLK_DIV2),
                     LED_TIMER_PERIOD_CYCLES);
    Sys_Timer_Start(LED_USER_CLOCK_SOURCE);

    /* Interrupt enabling */
    NVIC_SetPriority(TIMER0_IRQn, LED_TIMER_INTERRUPT_PRIORITY);
    NVIC_EnableIRQ(TIMER0_IRQn);

    /* LED_manager initialization */
    LED_Initialize(LED_USER_NUM_GPIOS,
                   LED_USER_GPIO_ACTIVE_LEVEL,
                   LED_TIMER_PERIOD_MS,
                   LED_GPIO_pins,
                   LED_SLOW_BLINK_PERIOD,
                   LED_MEDIUM_BLINK_PERIOD,
                   LED_FAST_BLINK_PERIOD);
#endif /* (LED_MANAGER_ENABLED == 1) */
}

void BatteryServiceServerInit(void)
{
    AppBASS_Initialize(APP_BAS_NB,
                        AppBatt_ReadBattLevel,
                        TIMER_SETTING_S(BATT_UPDATE_TIMEOUT_S),
                        TIMER_SETTING_S(BATT_CHANGE_TIMEOUT_S));

}

void DeviceInformationServiceServerInit(void)
{
    AppDISS_Initialize(APP_DIS_FEATURES,
                            (const diss_device_info_t*) &deviceInfo);
}

void CustomServiceServerInit(void)
{
    AppCustomSS_Init(TIMER_SETTING_S(CUSTOMSS_NOTIF_TIMEOUT_S));
}

void App_DisableInterrupts(void)
{
    /* Mask all interrupts */
    __set_PRIMASK(PRIMASK_DISABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_DISABLE_INTERRUPTS);

    /* Disable all existing interrupts, clearing all pending source */
    Sys_NVIC_DisableAllInt();
    Sys_NVIC_ClearAllPendingInt();
}

void App_EnableInterrupts(void)
{
    /* Unmask all interrupts */
    __set_PRIMASK(PRIMASK_ENABLE_INTERRUPTS);
    __set_FAULTMASK(FAULTMASK_ENABLE_INTERRUPTS);
}
