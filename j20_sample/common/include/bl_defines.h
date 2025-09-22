/**
 * @file bl_defines.h
 * @brief This interface provides access to the application versions numbers.
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

#ifndef INCLUDE_BL_DEFINES_H_
#define INCLUDE_BL_DEFINES_H_

/*******************************************************************************
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/*************************************************************************************************
 * Include files
 *************************************************************************************************/
#include <hw.h>

/*************************************************************************************************
 * Symbolic constants
 *************************************************************************************************/
#define BL_BOOT_APP_NAME_LENGTH 8

/* Recovery bootloader = 8 kB */
#define RECOVERY_SIZE                   (8*1024)
/* Main bootloader = 20 kB */
#define BOOTLOADER_SIZE                 (20*1024)

#define RECOVERY_BASE                   MRAM_MAIN_BASE
#define RECOVERY_TOP                    (RECOVERY_BASE + RECOVERY_SIZE - 1)

#define BOOTLOADER_BASE                 (RECOVERY_TOP + 1)
/* BOOTLOADER_TOP reserves the bootloader area and bootloader download area */
#define BOOTLOADER_TOP                  (BOOTLOADER_BASE + (BOOTLOADER_SIZE * 2) - 1)

#define APPLICATION_BASE                (BOOTLOADER_TOP + 1)
#define APPLICATION_TOP                 MRAM_MAIN_TOP

/** @brief Define the minimum size of an application. (Currently size of vect table) */
#define APPLICATION_SIZE_MIN (40)

/*************************************************************************************************
 * Macros
 *************************************************************************************************/
/** @brief Define a mechanism to encode a version number as a uint16_t. */
#define BL_VERSION_ENCODE(m, n, r)  (((m) << 24) | ((n) << 16) | (r))

/** @brief Define a mechanism to decode a version number from a uint16_t. */
#define BL_VERSION_DECODE(num)      ((num >> 24) & 0xFF), ((num >> 16) & 0xFF), (num & 0xFFFF)

/** @brief Define the boot version including name and ensure it is
 * stored in an easily accessible location
 */
#define BL_BOOT_VERSION(id, major, minor, revision)     \
    __attribute__ ((section(".rodata.boot.version")))   \
    const BL_BootAppVersion_t blBootAppVersion =        \
    {                                                   \
        id, BL_VERSION_ENCODE(major, minor, revision)   \
    };


/*************************************************************************************************
 * Type Definitions
 *************************************************************************************************/
/** @brief Define the application ID as a six character string. */
typedef char BL_BootAppId_t[BL_BOOT_APP_NAME_LENGTH];

/** @brief Define the application version as id and version details. */
typedef struct __attribute__((__packed__))
{
    BL_BootAppId_t id;  /**< App ID string */
    uint32_t num;       /**< <major[31:24]>.<minor[23:16]>.<revision[15:0]> */
} BL_BootAppVersion_t;

typedef struct
{
    uint32_t image_size;       /**< Image size in bytes excluding the signature */
    BL_BootAppId_t build_id_a; /**< App build ID */
} BootDescriptor_t;

/** @brief Define the contents of a Hello response. */
typedef struct __attribute__((__packed__))
{
    BL_BootAppVersion_t boot_version;   /**< Version info of boot loader. */
    BL_BootAppVersion_t app_version;    /**< Version info of loaded application */
#ifdef BL_OPT_FEATURE_RECOVERY_MODE
    BL_BootAppVersion_t recovery_version; /** Version info of the recovery bootloader */
#endif /*(BL_OPT_FEATURE_RECOVERY_MODE == BL_OPT_FEATURE_ENABLED) */
    uint16_t            block_size;     /**< Loading block size to use */
} BL_HelloResponse_t;

/*************************************************************************************************
 * Function Prototypes
 *************************************************************************************************/

/*******************************************************************************
 * Close the 'extern "C"' block
 *******************************************************************************/
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* INCLUDE_BL_DEFINES_H_ */
