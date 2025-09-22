/**
 * @file bl_recovery_memory.h
 * @brief Defines the memory layout for the bootloader when used with the recovery bootloader.
 *
 * @details
 * This defines the layout of the MRAM as follows:
 * - Recovery bootloader reserved area
 * - Bootloader reserved area
 * - Bootloader download area
 * - Application area
 *
 * This takes into account the sections of MRAM memory
 * which are reserved for the DEU, Bond data and Mesh data.
 *
 * TODO: Any reference to Data / Code is legacy from RSL15 and is to be removed
 * at a future date
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

#ifndef INCLUDE_BL_MEMORY_H_
#define INCLUDE_BL_MEMORY_H_

/*******************************************************************************
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 *******************************************************************************/
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/** @addtogroup SECURE_BOOTg
 *  @{
 */

/*************************************************************************************************
 * Include files
 *************************************************************************************************/

#include <hw.h>

/*************************************************************************************************
 * Symbolic constants
 *************************************************************************************************/
/** @brief The image block size when loading data. (Size of a code sector in bytes.) */
#define BL_CODE_SECTOR_SIZE     (1024)

/** @brief Number of bytes to transfer in one reception of the updated image */
#define BL_CODE_DEFAULT_TRANSFER_SIZE (BL_CODE_SECTOR_SIZE)

/** @brief The size of the area reserved for use by the ROM and stack. */
#define BL_RESERVED_SIZE  \
        (MRAM_DEU_RESERVED_SIZE + MRAM_BOND_INFO_SIZE)

/** @brief Define a size for the secure storage area. */
/* TODO: fix sizing later */
#define BL_SECURE_STORAGE_SIZE  (BL_RESERVED_SIZE + 4096)

/** @brief Define the base address of the secure storage area */
#define BL_SECURE_STORAGE_BASE  (MRAM_DEU_RESERVED_BASE - BL_SECURE_STORAGE_SIZE)

/** @brief Define the top of the secure storage area. */
#define BL_SECURE_STORAGE_TOP   \
        (BL_SECURE_STORAGE_BASE + BL_SECURE_STORAGE_SIZE - 1)

/* Note that this must match the value used in the linker script and must be
 * an even number */

#ifdef BL_RECOVERY_DEBUG
    #define BL_BOOTLOADER_SIZE_BYTES        (33 * 1024)
	/** @brief Define the size of the recovery bootloader in bytes. */
    #define BL_RECOVERY_BOOTLOADER_SIZE (11 * 1024)
#else
    #define BL_BOOTLOADER_SIZE_BYTES        (20 * 1024)
	/** @brief Define the size of the recovery bootloader in bytes. */
    #define BL_RECOVERY_BOOTLOADER_SIZE (7 * 1024)
#endif /* BL_RECOVERY_DEBUG */

/** @brief Define a size for the boot info storage area. */
#define BL_BOOT_INFO_STORAGE_SIZE  (1 * 1024)

/** @brief The base address of the recovery bootloader. */
#define BL_RECOVERY_BASE        MRAM_MAIN_BASE

/** @brief The top address of the recovery bootloader. */
#define BL_RECOVERY_TOP         (BL_RECOVERY_BASE + BL_RECOVERY_BOOTLOADER_SIZE -1)

/** @brief Define the base address of the secure storage area */
#define BL_BOOT_INFO_STORAGE_BASE  (BL_RECOVERY_BASE + BL_RECOVERY_BOOTLOADER_SIZE)

/** @brief Define the top of the secure storage area. */
#define BL_BOOT_INFO_STORAGE_TOP   \
        (BL_BOOT_INFO_STORAGE_BASE + BL_BOOT_INFO_STORAGE_SIZE - 1)

/** @brief The size of the area reserved for use by the bootloader. */
#define BL_BOOTLOADER_SIZE      BL_BOOTLOADER_SIZE_BYTES

/** @brief The base address of the bootloader. */
#define BL_BOOTLOADER_BASE      (BL_BOOT_INFO_STORAGE_BASE + BL_BOOT_INFO_STORAGE_SIZE)

/** @brief The top address of the bootloader. */
#define BL_BOOTLOADER_TOP      (BL_BOOTLOADER_BASE + BL_BOOTLOADER_SIZE - 1)

/** @brief Define the maximum size of the download area */
#define BL_DOWNLOAD_SIZE        BL_BOOTLOADER_SIZE

/** @brief Define the base address of the download area */
#define BL_DOWNLOAD_BASE        (BL_BOOTLOADER_BASE  + BL_BOOTLOADER_SIZE)

/** @brief Define the top address of the download area */
#define BL_DOWNLOAD_TOP         (BL_DOWNLOAD_BASE + BL_BOOTLOADER_SIZE - 1)

/** @brief The base of the code MRAM. */
#define BL_CODE_BASE      (BL_DOWNLOAD_BASE + BL_BOOTLOADER_SIZE)

/** @brief Define the top of MRAM code memory. */
#define BL_CODE_TOP       (BL_SECURE_STORAGE_BASE - 1)

/** @brief Code size is derived from the base and top addresses. */
#define BL_CODE_SIZE      (BL_CODE_TOP - BL_CODE_BASE + 1)

/** @brief Define the base address of the application */
#define BL_APPLICATION_BASE     BL_CODE_BASE

/** @brief Define the total available MRAM for application and download */
#define BL_AVAILABLE_SIZE       (BL_CODE_SIZE - BL_SECURE_STORAGE_SIZE)

/** @brief Define the maximum size of an application. (must be 2K aligned) */
#define BL_APPLICATION_SIZE     ((BL_AVAILABLE_SIZE) & 0xFFFFF800)

/*************************************************************************************************
 * Macros
 *************************************************************************************************/

/*************************************************************************************************
 * Type Definitions
 *************************************************************************************************/

/*************************************************************************************************
 * Function Prototypes
 *************************************************************************************************/

/** @} *//* End of the SECURE_BOOTg group */

/*******************************************************************************
 * Close the 'extern "C"' block
 *******************************************************************************/
#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* INCLUDE_BL_MEMORY_H_ */
