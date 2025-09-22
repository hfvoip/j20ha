/**
 * @file gpio_defs_evb_2_1.h
 * @brief Configure GPIO assignments for use with EVB 2.1
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

#ifndef GPIO_DEFS_EVB_2_1_H
#define GPIO_DEFS_EVB_2_1_H

/* ----------------------------------------------------------------------------
 * Include files
 * ------------------------------------------------------------------------- */
#include "hw.h"

/* ----------------------------------------------------------------------------
 * Definitions
 * ------------------------------------------------------------------------- */

/* Bootloader */
#define GPIO_BOOTLOADER_MODE_0 GPIO0   /* Used by bootloader to enter SPI / UART download mode */
#define GPIO_BOOTLOADER_MODE_1 GPIO13  /* If chip enters download mode app execution will pause */

/* Button */
#define GPIO_PUSH_BUTTON       GPIO7

/* DMIC */
#define GPIO_DMIC_SDI          GPIO9
#define GPIO_DMIC_CLK          GPIO10

/* I2C */
#define GPIO_I2C_SDA           GPIO12
#define GPIO_I2C_SCL           GPIO13

/* I3C */
#define GPIO_I3C_SDA           GPIO9
#define GPIO_I3C_SCL           GPIO8

/* JTAG */
#define GPIO_JTAG_TDI          GPIO14  /* disable JTAG TDI and TDO if GPIO 14 and 15 are needed */
#define GPIO_JTAG_TDO          GPIO15  /* JTAG pins are NOT reassignable */

/* LED */
#define GPIO_LED_BLUE          GPIO4
#define GPIO_LED_GREEN         GPIO11

/* PCM */
#define GPIO_PCM_CLK           GPIO1
#define GPIO_PCM_FRAME         GPIO2
#define GPIO_PCM_SERI          GPIO3
#define GPIO_PCM_SERO          GPIO14

/* SPI */
#define GPIO_SPI_CS            GPIO1
#define GPIO_SPI_CLK           GPIO5
#define GPIO_SPI_COPI          GPIO6
#define GPIO_SPI_CIPO          GPIO8

/* UART */
#define GPIO_UART_RX           GPIO5
#define GPIO_UART_TX           GPIO6

#endif /* GPIO_DEFS_EVB_2_1_H */
