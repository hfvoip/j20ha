/**
 * @file bl_simple_filer.h
 * @brief This provides the high level interface to a very simple file system.
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


#ifndef INCLUDE_BL_SIMPLE_FILER_H_
#define INCLUDE_BL_SIMPLE_FILER_H_

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
#include <stdint.h>
#include <stdbool.h>

#ifdef BOOTLOADER
/* File system is only required when secure storage option is selected */
#include "bl_options.h"
/* Select the memory layout depending if the recovery bootloader is used */
#if (BL_OPT_FEATURE_RECOVERY_MODE == BL_OPT_FEATURE_ENABLED)
    #include "bl_memory.h"
#else
    #include "bl_memory_secure.h"
#endif /* BL_OPT_FEATURE_RECOVERY_MODE */
#else
#include "bl_memory.h"
#endif

/*************************************************************************************************
 * Symbolic constants
 *************************************************************************************************/
/** @brief Define the maximum supported file size. */
#define BL_FS_MAX_FILE_SIZE     BL_CODE_SECTOR_SIZE

/** @brief Define the status values possible from the secure filer module. */
typedef enum
{
    BL_FS_NO_ERROR,
    BL_FS_VALIDATE_FAILURE,
    BL_FS_FS_FULL,
    BL_FS_MISSING_FILE,
    BL_FS_DUPLICATE_FILE,
    BL_FS_INVALID_OPERATION,
    BL_FS_ENCRYPTION_FAILURE,
    BL_FS_MRAM_READ_FAILURE,
    BL_FS_MRAM_WRITE_FAILURE,
    BL_FS_MRAM_ERASE_FAILURE,
    BL_FS_CRC_MISMATCH
} BL_FStoreStatus_t;

/** @brief Define the flags associated with stored files. */
typedef enum
{
    BL_FS_CAN_READ = 1,
    BL_FS_CAN_WRITE = 2,
    BL_FS_CAN_DELETE = 4
} BL_FSFlags_t;

/** @brief Define a set of reserved file names that should not be accessible
 * directly from the loader.
 */
typedef enum
{
    BL_FS_NOFILE,
    BL_FS_ATTEST_AES_128,
    BL_FS_ATTEST_AES_192,
    BL_FS_ATTEST_AES_256,
    BL_FS_ATTEST_RSA_1024,
    BL_FS_ATTEST_RSA_2048,
    BL_FS_ATTEST_RSA_3072,
    BL_FS_ATTEST_ECC_256,
    BL_FS_MAX_RESERVED_FILE,
} BL_FSReservedFilename_t;

typedef enum
{
    BL_FS_INFO_STORAGE,
    BL_FS_SECURE_STORAGE,
    BL_FS_STORAGE_MAX
} BL_FSStorageLocation_t;

typedef enum
{
    BL_FS_SIZE_AND_CRC_ID = 1,
    BL_FS_SUPPLY_CONFIG_ID,
    BL_FS_SPI_CIPO_GPIO_ID,
    BL_FS_SPI_GPIO_CONFIG_ID,
    BL_FS_MAX_ID
}BL_FSIDNumber_t;

typedef enum
{
    BL_SUPPLY_CONFIG_VCC_LDO_VDDA_LDO = 0,
    BL_SUPPLY_CONFIG_VCC_LDO_VDDA_CP,
    BL_SUPPLY_CONFIG_VCC_CP_VDDA_LDO,
    BL_SUPPLY_CONFIG_VCC_BUCK_VDDA_CP,
    BL_SUPPLY_CONFIG_VCC_CP_VDDA_BUCK,
    BL_SUPPLY_CONFIG_UNKNOWN,
    BL_SUPPLY_CONFIG_CONFIRM, /* Used in the confirmation message that the supply has been set properly */
    BL_SUPPLY_CONFIG_ZNAIR_VCC_LDO_VDDA_CP,
    BL_SUPPLY_CONFIG_LIION_VCC_LDO_VDDA_CP,
    BL_SUPPLY_CONFIG_MAX
} BL_SupplyConfigType_t;

/** @brief define the inode size */
#define BL_FS_INODE_SIZE        sizeof(BL_FSINode_t)

/** @brief Define the basic block size. In this case a single data sector */
#define BL_FS_MIN_FILE_SIZE        sizeof(uint64_t)

/** @brief Defines the size of both the start and end bytes combined. */
#define BL_FS_INODE_START_END_BYTES_SIZE 4

/** @brief Defines the number of bytes required to 64bit align the size of the inode table */
#define BL_FS_INODE_64_BIT_ALIGN_PAD 4

/** @brief Defines the number of files that can be in the file system */
#define BL_FS_BOOT_INFO_INODE_COUNT 32

/** @brief Defines the size of the inode table in bytes */
#define BL_FS_BOOT_INFO_INODE_TABLE_SIZE ((BL_FS_BOOT_INFO_INODE_COUNT * sizeof(BL_FSINode_t)) \
                                            + BL_FS_INODE_START_END_BYTES_SIZE + BL_FS_INODE_64_BIT_ALIGN_PAD)

/** @brief The file system size is the secure storage size minus the inodes. */
#define BL_FS_BOOT_INFO_SIZE              (BL_BOOT_INFO_STORAGE_SIZE - BL_FS_BOOT_INFO_INODE_TABLE_SIZE)

/** @brief Define the total number of blocks */
#define BL_FS_BOOT_INFO_BLOCK_COUNT       (BL_FS_BOOT_INFO_SIZE / BL_FS_MIN_FILE_SIZE)

/** @brief Locate the inode table at the base of secure storage */
#define BL_FS_BOOT_INFO_INODE_TABLE       BL_BOOT_INFO_STORAGE_BASE

/** @brief Locate the file system data above the indoe table */
#define BL_FS_BOOT_INFO_DATA_SPACE        (BL_FS_BOOT_INFO_INODE_TABLE + BL_FS_BOOT_INFO_INODE_TABLE_SIZE)

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    /* @brief Define the inode table to be no bigger than one block */
    #define BL_FS_SECURE_INODE_TABLE_SIZE  BL_FS_MIN_FILE_SIZE

    /** @brief The file system size is the secure storage size minus the inodes. */
    #define BL_FS_SECURE_SIZE              (BL_SECURE_STORAGE_SIZE - BL_FS_SECURE_INODE_TABLE_SIZE)

    /** @brief Define the total number of blocks */
    #define BL_FS_SECURE_BLOCK_COUNT       (BL_FS_SECURE_SIZE / BL_FS_SECURE_BLOCK_SIZE)

    /** @brief Define the total number of inodes */
    #define BL_FS_SECURE_INODE_COUNT       (BL_FS_SECURE_INODE_TABLE_SIZE / BL_FS_SECURE_INODE_SIZE)

    /** @brief Locate the inode table at the base of secure storage */
    #define BL_FS_SECURE_INODE_TABLE       BL_SECURE_STORAGE_BASE

    /** @brief Locate the file system data above the indoe table */
    #define BL_FS_SECURE_DATA_SPACE        (BL_SECURE_STORAGE_BASE + BL_FS_SECURE_INODE_TABLE_SIZE)
#endif /* BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED */
#endif

/*************************************************************************************************
 * Macros
 *************************************************************************************************/
/* Definition of public function-like macros go here */

/*************************************************************************************************
 * Type Definitions
 *************************************************************************************************/
/** @brief Define a file identifier as a eight bit word */
typedef uint8_t BL_FSFileId_t;

/** @brief Define an inode, there will be one of these for each file. */
typedef struct __attribute__((packed))
{
    uint32_t *p_fileStartAddr;
    BL_FSFileId_t id;
    uint8_t flags;
    uint8_t size;
    uint16_t crc;
} BL_FSINode_t;

/*************************************************************************************************
 * Function Prototypes
 *************************************************************************************************/

/**
 * @brief Initializes the Info Storage File System.
 */
BL_FStoreStatus_t BL_FStoreInitializeInfoStorage(void);

/**
 * @brief Checks if the given file system is valid.
 * @brief location The file system to access.
 * @return The status of the operation, BL_FS_NO_ERROR if the file system exists
 */
BL_FStoreStatus_t BL_FStoreValidate(BL_FSStorageLocation_t location);

/**
 * @brief Retrieve the size in bytes of a requested file.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @return The size in bytes or 65535 if the file cannot be found.
 */
uint32_t BL_FStoreFileSize(BL_FSStorageLocation_t location, BL_FSFileId_t id);

/**
 * @brief Determine if the file exists in the store.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @return True if the file exists, false otherwise.
 */
bool BL_FStoreFileExists(BL_FSStorageLocation_t location, BL_FSFileId_t id);

/**
 * @brief Determine if the file exists in the store and can be read.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @return True if the file exists and can be read, false otherwise.
 */
bool BL_FStoreFileCanRead(BL_FSStorageLocation_t location, BL_FSFileId_t id);

/**
 * @brief Determine if the file exists in the store and can be written.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @return True if the file exists and can be written, false otherwise.
 */
bool BL_FStoreFileCanWrite(BL_FSStorageLocation_t location, BL_FSFileId_t id);

/**
 * @brief Determine if the file exists in the store and can be deleted.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @return True if the file exists and can be deleted, false otherwise.
 */
bool BL_FStoreFileCanDelete(BL_FSStorageLocation_t location, BL_FSFileId_t id);

/**
 * @brief Write a buffer from RAM to the secure file store.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @param p_buffer A pointer to a RAM buffer containing the file contents.
 * @param size The size of the buffer in bytes.
 * @param flags The set of flags associated with the file.
 * @return BL_FS_NO_ERROR if the file is successfully written,
 * error state otherwise..
 */
BL_FStoreStatus_t BL_FStoreWrite(BL_FSStorageLocation_t location, BL_FSFileId_t id,
                                 uint32_t *p_buffer, uint16_t size,
                                 uint16_t flags);

/**
 * @brief Read a file from the secure file store to a RAM buffer.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @param p_buffer A pointer to a RAM buffer to write the file contents.
 * @param p_max_size The maximum size of the buffer in bytes. On exit, this
 * value is updated to reflect the actual number of bytes read.
 * @return BL_FS_NO_ERROR if the file is successfully read,
 * error state otherwise..
 */
BL_FStoreStatus_t BL_FStoreRead(BL_FSStorageLocation_t location,
                                BL_FSFileId_t id, uint32_t *p_buffer,
                                uint16_t *p_max_size);

/**
 * @brief Delete a file from the secure file.
 * @brief location The file system to access.
 * @param id The file identifier.
 * @return BL_FS_NO_ERROR if the file is successfully deleted,
 * error state otherwise..
 */
BL_FStoreStatus_t BL_FStoreDelete(BL_FSStorageLocation_t location, BL_FSFileId_t id);

/**
 * @brief Retrieve the secure store directory information.
 * @brief location The file system to access.
 * @param p_buffer A pointer to a RAM buffer to write the directory contents.
 * @param p_max_size The maximum size of the buffer in bytes. On exit, this
 * @param show_hidden Flag indicating if the reserved files should be included.
 * value is updated to reflect the actual number of bytes read.
 * @return BL_FS_NO_ERROR if the directory is successfully read,
 * error state otherwise..
 */
BL_FStoreStatus_t BL_FStoreFileList(BL_FSStorageLocation_t location,
                                    uint8_t *p_buffer, uint16_t *p_max_size,
                                    bool show_hidden);

/*******************************************************************************
 * Close the 'extern "C"' block
 *******************************************************************************/
#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif /* INCLUDE_BL_SIMPLE_FILER_H_ */
