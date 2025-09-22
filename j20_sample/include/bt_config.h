/**
 ****************************************************************************************
 *
 * @file bt_config.h
 *
 * @brief Template Bluetooth configuration that can be used as reference.  It:
 *         - provides an list of all compile #defines needed to configure bt_common, and
 *         - provides examples of switch settings and #define values.
 *
 *        To use this file, developers must clone it into their project, rename it
 *        "bt_config.h", and modify it for their particular application.
 *
 *        Note that some modifications are required to avoid collisions with other
 *        instances of this code.
 *
 ****************************************************************************************
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

#ifndef BT_CONFIG_H
#define BT_CONFIG_H

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Includes
 * ------------------------------------------------------------------------- */
#include "common_gap.h"

/* ----------------------------------------------------------------------------
 * Required Configuration
 * --------------------------------------------------------------------------*/
/* This is a test address. Change it! */
#define APP_PUBLIC_ADDRESS              { 0x78, 0x00, 0x22, 0x33, 0x44, 0xAB }

/* ----------------------------------------------------------------------------
 * Configuration
 * --------------------------------------------------------------------------*/
/* Default TX power setting */
#define DEF_TX_POWER                    0

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* BT_CONFIG_H */
