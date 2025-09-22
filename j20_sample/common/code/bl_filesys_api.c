/**
 * @file bl_simple_filer.c
 * @brief This provides the high level interface to a very simple file system.
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

/* Simple file system, primarily for storing attestation keys but
 * allowing for more general storage of small files if needed.
 *
 * Basic operation:
 *  File system location is defined in bl_memory.h and occupies a range
 *  of sectors in data flash. Due to the limitations of the flash, the
 *  file system space is set to 11K bytes.
 *
 *  The file system is organized in blocks which align with the underlying
 *  data sectors. Each data sector is 256 bytes in length.
 *  There are therefore 44 blocks available for use.
 *
 *  A single file may be stored in more than one block.
 *  A single block can only contain information for a single file.
 *
 *  The first sector contains the inode table which describes the blocks
 *  which are allocated to each file. There is a single inode entry for each
 *  file held in the file system. Each inode is defined as 12 bytes, there
 *  is therefore a maximum number of 21 files supported by the system.
 *
 *  Each inode/file contains the following information:
 *
 *      - The list of blocks allocated to the file. This is a 48 bit mask
 *      where a 1 indicates that data block is used by that file.
 *
 *      - The file id which as space is limited is defined as a
 *      16 bit value. How this is derived from a textual filename is
 *      left to the caller.
 *
 *      - A flags word which indicates if the file is readable, writable
 *      or can be deleted.
 *
 *      - Size of the file in bytes. This is a 16 byte value as the maximum
 *      size of the store is defined as 44K so this is large enough to handle
 *      any file which can be stored.
 *
 *  Potential Enhancements
 *      - If the secure storage area is increased then the inode table will
 *      need to be enlarged to accommodate this. This also implies that the
 *      masks and size would need to be enlarged.
 *      - The files are not held with any consistency information so it
 *      may benefit by the addition of a CRC on each file. This CRC would be
 *      held as part of the inode table.
 */
/*************************************************************************************************
 * Include files
 *************************************************************************************************/
#include <string.h>

#include <hw.h>
#include <MRAM_rom.h>

#include <bl_filesys_api.h>

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    #include "bl_file_encryption.h"
#endif /* (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

/*************************************************************************************************
 * Module Symbolic Constants
 *************************************************************************************************/
/** @brief Define a consistency check marker for the start of the inode table. */
#define BL_FS_INODE_START_BYTES     0x55EC

/** @brief Define a consistency check marker for the end of the inode table. */
#define BL_FS_INODE_END_BYTES     0x5EC5

/** @brief Defines the size of the workspace used during encryption / decryption */
#define BL_FS_WORKSPACE_SIZE 1024

/** @brief Defines the size of the array used to hold the IV for encryption */
#define BL_FS_IV_SIZE 32

/** @brief Defines the CRC configuration for the file system */
#define BL_FS_CRC_CONFIG (CRC_LITTLE_ENDIAN | CRC_BIT_ORDER_STANDARD)

/*************************************************************************************************
 * Module Macros
 *************************************************************************************************/

/*************************************************************************************************
 * Module Type Definitions
 *************************************************************************************************/
/* @brief Define the inode table as a structure to ease access. */
typedef struct
{
    uint16_t startBytes;
    BL_FSINode_t inodes[BL_FS_BOOT_INFO_INODE_COUNT];
    uint16_t endBytes;
} BL_FSINodeTableBootInfo_t;

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
/* @brief Define the inode table as a structure to ease access. */
typedef struct
{
    uint16_t startBytes;
    BL_FSINode_t inodes[BL_FS_SECURE_INODE_COUNT];
    uint16_t endBytes;
} BL_FSINodeTableSecureStorage_t;
#endif /* (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

/* TODO: Figure out method to have different sized inode tables */
typedef struct
{
    uint32_t *p_FSINodeTable;
    uint32_t *p_FSDataSpace;
    uint32_t FSINodeTableSize;
    uint32_t FSDataSize;
    uint32_t FSSizeBytes;
    uint32_t FSBlockCount;
    uint32_t FSINodeCount;
    bool     encrypted;
    BL_FSStorageLocation_t storageLocation;
    BL_FSINodeTableBootInfo_t inode_table;
} BL_FSStorage_t;

/*************************************************************************************************
 * Module Variable Definitions
 *************************************************************************************************/
#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
/** @brief Define a label that can be used when deriving an encryption key. */
static const uint8_t fs_label[] =
{ 0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38 };

static BL_FSStorage_t Secure_Storage =
{
        .p_FSINodeTable = BL_FS_SECURE_INODE_TABLE,
        .p_FSDataSpace = BL_FS_SECURE_DATA_SPACE,
        .FSINodeTableSize = BL_FS_SECURE_INODE_TABLE_SIZE,
        .FSSizeBytes = BL_FS_SECURE_SIZE,
        .FSBlockCount = BL_FS_SECURE_BLOCK_COUNT,
        .FSINodeCount = BL_FS_SECURE_INODE_COUNT,
        .encrypted = true,
        .storageLocation = BL_FS_SECURE_STORAGE
};

/** @brief Keep a single encryption block which will act an an IV. */
/* Seed the initial value with some invariant data */
static uint8_t fs_initial_value[BL_FS_IV_SIZE] = { 0x5A, 0xA5, 0, 0, 0, 0, 0, 0, 0,\
                                                    0, 0, 0, 0, 0, 0xAB, 0xCD };
#endif /* BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE  */

/** @brief Keep a single sector available as a working area for encryption. */
static uint8_t fs_workspace[BL_FS_WORKSPACE_SIZE] =
{ 0 };

static BL_FSStorage_t Info_Storage =
{
        .p_FSINodeTable = (uint32_t*)BL_FS_BOOT_INFO_INODE_TABLE,
        .p_FSDataSpace = (uint32_t*)BL_FS_BOOT_INFO_DATA_SPACE,
        .FSINodeTableSize = BL_FS_BOOT_INFO_INODE_TABLE_SIZE,
        .FSDataSize = BL_FS_BOOT_INFO_SIZE,
        .FSSizeBytes = BL_BOOT_INFO_STORAGE_SIZE,
        .FSBlockCount = BL_FS_BOOT_INFO_BLOCK_COUNT,
        .FSINodeCount = BL_FS_BOOT_INFO_INODE_COUNT,
        .encrypted = false,
        .storageLocation = BL_FS_INFO_STORAGE
};
/*************************************************************************************************
 * Function Prototypes
 *************************************************************************************************/
static BL_FStoreStatus_t BL_FStoreMakeInfoFilesystem(BL_FSStorageLocation_t location);

static BL_FStoreStatus_t BL_FSRead(uint32_t *p_dst, uint32_t *p_src, uint16_t size, bool encrypted);

static BL_FStoreStatus_t BL_FSWrite(uint32_t *p_dst, uint32_t *p_src, uint16_t size, bool encrypted);

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
static uint8_t *BL_FSIVFromInode(BL_FSINode_t *p_inode);
#endif /* BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

static BL_FStoreStatus_t BL_FSHasValidFileSystem(BL_FSStorage_t *p_file_system);

static BL_FStoreStatus_t BL_FStoreSaveInodeTable(BL_FSStorage_t *p_file_system);

static BL_FSINode_t *BL_FStoreFindFile(BL_FSStorage_t *p_file_system, BL_FSFileId_t id);

static bool BL_FStoreIsFlagSet(BL_FSStorage_t *p_file_system, BL_FSFileId_t id, BL_FSFlags_t flag);

static BL_FSINode_t *BL_FStoreFindFreeInode(BL_FSStorage_t *p_file_system);

static BL_FStoreStatus_t BL_FStoreFindFreeBlock(BL_FSStorage_t *p_file_system, BL_FSINode_t *p_inode, uint32_t *p_start_addr, uint32_t size);

static BL_FSStorage_t *BL_FStoreSelectWorkingLocation(BL_FSStorageLocation_t location);

static BL_FStoreStatus_t BL_FStoreUpdateExistingFile(BL_FSStorageLocation_t location, BL_FSFileId_t id, uint32_t *p_buffer, uint16_t size, uint16_t flags);

static BL_FStoreStatus_t BL_FStoreNewFile(BL_FSStorageLocation_t location, BL_FSFileId_t id, uint32_t *p_buffer, uint16_t size, uint16_t flags);
/*************************************************************************************************
 * Function Definitions
 *************************************************************************************************/
/**
 * @brief Read a single sector from a file.
 * @param p_dst A pointer to a buffer into which the data should be written.
 * @param p_src A pointer to the memory holding the file to read from.
 * @param size The number of bytes to read.
 * @param encrypted If true encrypt data before writing otherwise
 *                  write in plain text.
 * @return BL_FS_NO_ERROR if success, otherwise an error status.
 */
static BL_FStoreStatus_t BL_FSRead(uint32_t *p_dst, uint32_t *p_src, uint16_t size, bool encrypted)
{
#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    /* Decryption on read should happen in a working buffer before being copied
     * to the destination. This needs to account for any padding due to the
     * block size.
     */
    uint16_t padding = size & (BL_ENCRYPT_BLOCK_SIZE_BYTES - 1);
    if (padding != 0)
    {
        padding = BL_ENCRYPT_BLOCK_SIZE_BYTES - padding;
    }

    /* Copy encrypted data to working buffer. */
    memcpy(fs_workspace, p_src, size + padding);

    /* Decrypt the working buffer, this may over-run the size of the
     * destinaton buffer so we need an extra memcpy after decryption.
     */
    if (BL_EncryptDecryptBuffer(fs_workspace, size + padding) != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }

    /* Copy the requisite number of words from the working buffer to the
     * destination.
     */
    memcpy(p_dst, fs_workspace, size);
#endif /* (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

    memcpy(p_dst, p_src, size);
    return BL_FS_NO_ERROR;
}

/**
 * @brief Write a single sector to a file.
 * @param p_dst A pointer to the memory holding the file to write to.
 * @param p_src A pointer to a buffer from which the data should be read.
 * @param size The number of bytes to read.
 * @param encrypted If true encrypt data before writing otherwise
 *                  write in plain text.
 * @return BL_FS_NO_ERROR if success, otherwise an error status.
 */
static BL_FStoreStatus_t BL_FSWrite(uint32_t *p_dst, uint32_t *p_src, uint16_t size, bool encrypted)
{
    uint16_t padding = 0;

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    /* Encryption on write should happen in a working buffer before being
     * written. This needs to account for any padding due to the block size.
     */
    padding = size & (BL_ENCRYPT_BLOCK_SIZE_BYTES - 1);
    if (padding != 0)
    {
        padding = BL_ENCRYPT_BLOCK_SIZE_BYTES - padding;
    }

    /* Copy to working buffer and pad to a multiple of block length */
    memcpy(fs_workspace, p_src, size);
    memset(&fs_workspace[size], 0, padding);

    /* Encrypt the working buffer before writing to the flash. */
    if (BL_EncryptEncryptBuffer(fs_workspace, size + padding) != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /* (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

    MRAMStatus_t status = 0;

    if (MRAM_Erase_Sequential((uint32_t)p_dst, (size >> 2)) != MRAM_ERR_NONE)
    {
        /* Unable to erase sector */
        return BL_FS_MRAM_WRITE_FAILURE;
    }

    if (encrypted == true)
    {
        status = MRAM_WriteBuffer_Sequential((uint32_t)p_dst, (size + padding) >> 2, (uint32_t*)fs_workspace);
    }
    else
    {
         status = MRAM_WriteBuffer_Sequential((uint32_t)p_dst, (size >> 2), p_src);
    }
    if (status != MRAM_ERR_NONE)
    {
        /* Unable to program sector */
        return BL_FS_MRAM_WRITE_FAILURE;
    }

    return BL_FS_NO_ERROR;
}

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
/**
 * @brief A helper function to provide a unique IV value given an inode.
 * @param p_inode A pointer to an inode structure, or NULL.
 * @return A pointer to a buffer containing the IV to use.
 */
static uint8_t *BL_FSIVFromInode(BL_FSINode_t *p_inode)
{
    if (p_inode == NULL)
    {
        memset(&fs_initial_value[2], 0x73, sizeof(BL_FSINode_t));
    }
    else
    {
        memcpy(&fs_initial_value[2], (uint8_t *) p_inode, sizeof(BL_FSINode_t));
    }
    return fs_initial_value;
}
#endif /* BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

static uint16_t BL_FSCalculateFileCRC(uint32_t *p_File, uint8_t size_words)
{
    uint16_t crc = 0;
    uint32_t crc_config = BL_FS_CRC_CONFIG;
    SYS_CRC_CONFIG(crc_config);
    CRC->VALUE = CRC_CCITT_INIT_VALUE;

    for (int i = 0; i < size_words; i++)
    {
        CRC->ADD_32 = *p_File;
        p_File++;
    }

    crc = CRC->FINAL;
    return crc;
}

/**
 * @brief Helper function to determine if a valid file system is available.
 * @param p_file_system points to the file system to validate.
 * @return True if the file system inode table appears to be valid. False
 * otherwise.
 */
static BL_FStoreStatus_t BL_FSHasValidFileSystem(BL_FSStorage_t *p_file_system)
{

    BL_FSRead((uint32_t *) &p_file_system->inode_table, p_file_system->p_FSINodeTable,
              p_file_system->FSINodeTableSize, p_file_system->encrypted);

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    if (BL_EncryptResetDecryption(BL_FSIVFromInode(NULL)) != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }
    if (BL_EncryptComplete() != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /* BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

    if ((p_file_system->inode_table.startBytes == BL_FS_INODE_START_BYTES)
            && (p_file_system->inode_table.endBytes == BL_FS_INODE_END_BYTES))
    {
        return BL_FS_NO_ERROR;
    }

    return BL_FS_VALIDATE_FAILURE;
}

/**
 * @brief Helper function to write the inode table to the file store.
 * @param p_file_system points to the file system of where to store the updated inode table.
 * @return BL_FS_NO_ERROR if success, otherwise an error status.
 */
static BL_FStoreStatus_t BL_FStoreSaveInodeTable(BL_FSStorage_t *p_file_system)
{
    if (BL_FSWrite(p_file_system->p_FSINodeTable, (uint32_t *) &p_file_system->inode_table,
                    p_file_system->FSINodeTableSize, p_file_system->encrypted) != BL_FS_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    if (BL_EncryptResetEncryption(BL_FSIVFromInode(NULL)) != BL_ENCRYPT_NO_ERROR)
    {
            return BL_FS_ENCRYPTION_FAILURE;
    }

    if (BL_EncryptComplete() != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /* BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

    return BL_FS_NO_ERROR;
}

/**
 * @brief Helper function to locate the inode for a file with a specified id.
 * @param p_file_system points to the file system of where the file is stored.
 * @param id The identifier of the file.
 * @return A pointer to the inode associated with the file or NULL.
 */
static BL_FSINode_t *BL_FStoreFindFile(BL_FSStorage_t *p_file_system, BL_FSFileId_t id)
{
    if (BL_FSHasValidFileSystem(p_file_system) == BL_FS_NO_ERROR)
    {
        for (int index = 0; index < p_file_system->FSINodeCount; index++)
        {
            BL_FSINode_t *p_inode = &p_file_system->inode_table.inodes[index];
            if (p_inode->id == id)
            {
                return p_inode;
            }
        }
    }

    return NULL;
}

/**
 * @brief Helper function used to determine if a specific flag is set.
 * @param p_file_system points to the file system of where the file is stored.
 * @param id The identifier of the file to be checked.
 * @param flag The flag or set of flags to be checked.
 * @return true if the flag(s) are set, false otherwise.
 */
static bool BL_FStoreIsFlagSet(BL_FSStorage_t *p_file_system, BL_FSFileId_t id, BL_FSFlags_t flag)
{
    BL_FSINode_t *p_inode = BL_FStoreFindFile(p_file_system, id);
    if (p_inode == NULL)
    {
        return false;
    }

    return ((p_inode->flags & flag) == flag);
}

/**
 * @brief Helper function to locate an unused inode in the file table.
 * @param p_file_system points to the file system of where to find the free inode.
 * @return A pointer to an unused and empty inode if one exists.
 * False otherwise.
 */
static BL_FSINode_t *BL_FStoreFindFreeInode(BL_FSStorage_t *p_file_system)
{
    /* An inode is defined as unused if the size and filename are zero */
    for (uint32_t index = 0; index < p_file_system->FSINodeCount; index++)
    {
        BL_FSINode_t *p_inode = &p_file_system->inode_table.inodes[index];
        if ((p_inode->id == 0) && (p_inode->size == 0))
        {
            /* inode should be empty, but enforce it */
            memset(p_inode, 0, sizeof(BL_FSINode_t));
            return p_inode;
        }
    }
    return NULL;
}

/**
 * @brief Helper function to locate an empty file in the data section.
 * @param p_file_system points to the file system of where the file is stored.
 * @param p_inode address of the new inode to be created. Used to find the previous inode size and start address.
 * @param p_start_addr points to the start address of the new file once an empty space is found.
 * @param size Size of the file to be written.
 * @return BL_FS_NO_ERROR on success otherwise BL_FS_FS_FULL
 */
static BL_FStoreStatus_t BL_FStoreFindFreeBlock(BL_FSStorage_t *p_file_system, BL_FSINode_t *p_inode, uint32_t *p_start_addr, uint32_t size)
{
    uint32_t previousInodeIndex = 0;
    uint32_t previousFileSize;
    uint32_t *p_previousFileAddr;

    for (uint32_t index = 0; index < p_file_system->FSINodeCount; index++)
    {
        BL_FSINode_t *p_inode_current = &p_file_system->inode_table.inodes[index];
        if (p_inode_current == p_inode)
        {
            if (index != 0)
            {
                previousInodeIndex = (index - 1);
                previousFileSize = p_file_system->inode_table.inodes[previousInodeIndex].size;
                p_previousFileAddr = p_file_system->inode_table.inodes[previousInodeIndex].p_fileStartAddr;
                *p_start_addr = (uint32_t)(p_previousFileAddr + (previousFileSize >> 2));
            }
            else
            {
                *p_start_addr = (uint32_t)p_file_system->p_FSDataSpace;
            }
            return BL_FS_NO_ERROR;
        }
    }

    return BL_FS_FS_FULL;
}

/**
 * @brief Helper function to find the storage location that is trying to be accessed
 * @param location The storage that is trying to be accessed
 * @return Returns a pointer to the storage location trying to be accessed.
 */
static BL_FSStorage_t *BL_FStoreSelectWorkingLocation(BL_FSStorageLocation_t location)
{
    BL_FSStorage_t *p_storage;
    switch(location)
    {
        case BL_FS_INFO_STORAGE:
        {
            p_storage = &Info_Storage;
            break;
        }
        case BL_FS_SECURE_STORAGE:
        {
#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
            p_storage = &Secure_Storage;
#endif /* (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */
            break;
        }
        default:
        {
            return 0;
            break;
        }
    }
    return p_storage;
}

/**
 * @brief Updates an existing file with new information
 * @param location The location of the file system with the file to be updated.
 * @param id ID of the file to be updated.
 * @param p_buffer Data to be written to MRAM.
 * @param size Size of the the new data to be written.
 * @param flags Flags for Read, Write, Delete access
 * @return BL_FS_NO_ERROR on success otherwise return the error code.
 */
static BL_FStoreStatus_t BL_FStoreUpdateExistingFile(BL_FSStorageLocation_t location, BL_FSFileId_t id, uint32_t *p_buffer, uint16_t size, uint16_t flags)
{
    BL_FStoreStatus_t status = BL_FS_NO_ERROR;
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    BL_FSINode_t *p_inode = BL_FStoreFindFile(p_storage, id);
    if (p_inode == NULL)
    {
        return BL_FS_MISSING_FILE;
    }

    if ((p_inode->flags & BL_FS_CAN_WRITE) == BL_FS_CAN_WRITE)
    {
        uint32_t *p_sector = p_inode->p_fileStartAddr;
        uint32_t *p_src = p_buffer;

        if (BL_FSWrite(p_sector, p_src, size, p_storage->encrypted) != BL_FS_NO_ERROR)
        {
            status = BL_FS_MRAM_WRITE_FAILURE;
        }
        if (status ==BL_FS_NO_ERROR)
        {
            /* Calculate CRC for the file being written */
            p_inode->crc = BL_FSCalculateFileCRC(p_buffer, (size >> 2));
            BL_FStoreSaveInodeTable(p_storage);
        }
    }
    else
    {
        status = BL_FS_INVALID_OPERATION;
    }
    return status;
}

/**
 * @brief Write a buffer from RAM to the file store.
 * @param location The location of the file system with the file to be written.
 * @param id The file identifier.
 * @param p_buffer A pointer to a RAM buffer containing the file contents.
 * @param size The size of the buffer in bytes.
 * @param flags The set of flags associated with the file.
 * @return BL_FS_NO_ERROR if the file is successfully written,
 * error state otherwise.
 */
static BL_FStoreStatus_t BL_FStoreNewFile(BL_FSStorageLocation_t location, BL_FSFileId_t id, uint32_t *p_buffer, uint16_t size, uint16_t flags)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    /* Check if there are any free inodes */
    BL_FSINode_t *p_inode = BL_FStoreFindFreeInode(p_storage);
    if (p_inode == NULL)
    {
        return BL_FS_FS_FULL;
    }

    /* If the size is less then 2 words the file will occupy 2 words */
    if (size < BL_FS_MIN_FILE_SIZE)
    {
        size = BL_FS_MIN_FILE_SIZE;
    }

    /* And find the free address, use a temp variable to store this until the
     * data is written to disk
     */
    BL_FSINode_t working;
    uint32_t newStartAddr;
    if (BL_FStoreFindFreeBlock(p_storage, p_inode, &newStartAddr, size) != BL_FS_NO_ERROR)
    {
        return BL_FS_FS_FULL;
    }

    working.p_fileStartAddr = (uint32_t*) newStartAddr;
    working.id = id;
    working.flags = flags;
    working.size = size;

    /* Calculate CRC for the file being written */
    working.crc = BL_FSCalculateFileCRC(p_buffer, (size >> 2));

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    if (BL_EncryptResetEncryption(BL_FSIVFromInode(&working)) != BL_ENCRYPT_NO_ERROR)
    {
            return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /*(BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

    /* At this stage the working inode block flags indicate the set of blocks
     * which are available for use. So just copy the buffer to the data blocks.
     */
    uint32_t *p_sector = working.p_fileStartAddr;
    uint32_t *p_src = p_buffer;

    if (BL_FSWrite(p_sector, p_src, size, p_storage->encrypted) != BL_FS_NO_ERROR)
    {
        return BL_FS_MRAM_WRITE_FAILURE;
    }

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    if (BL_EncryptComplete() != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /*(BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */

    /* Data written, update the inode and file system */
    memcpy(p_inode, (uint8_t*) &working, sizeof(BL_FSINode_t));

    return BL_FStoreSaveInodeTable(p_storage);
}

BL_FStoreStatus_t BL_FStoreInitializeInfoStorage()
{
    BL_FStoreStatus_t status = BL_FS_NO_ERROR;
    status = BL_FStoreValidate(BL_FS_INFO_STORAGE);
    if (status == BL_FS_VALIDATE_FAILURE)
    {
        /* No Valid Filesystem could be found re-initalize it */
        status = BL_FStoreMakeInfoFilesystem(BL_FS_INFO_STORAGE);
    }

    return status;
}

BL_FStoreStatus_t BL_FStoreValidate(BL_FSStorageLocation_t location)
{
    BL_FSStorage_t *p_storageToInitialize = BL_FStoreSelectWorkingLocation(location);

    if (BL_FSHasValidFileSystem(p_storageToInitialize) != BL_FS_NO_ERROR)
    {
        return BL_FS_VALIDATE_FAILURE;
    }

    return BL_FS_NO_ERROR;
}

/**
 * @brief Try to make a new file system.
 * @brief location The file system to access.
 * @return BL_FS_NO_ERROR if a new file system can be initialized.
 */
static BL_FStoreStatus_t BL_FStoreMakeInfoFilesystem(BL_FSStorageLocation_t location)
{
    BL_FSStorage_t *p_storageToMake = BL_FStoreSelectWorkingLocation(location);

    uint32_t mram_erase_result = MRAM_ERR_NONE;

    /* Clear the inode area */
    mram_erase_result = MRAM_Erase_NonSequential((uint32_t)p_storageToMake->p_FSINodeTable, (p_storageToMake->FSINodeTableSize >> 2));
    if (mram_erase_result != MRAM_ERR_NONE)
    {
        return BL_FS_MRAM_ERASE_FAILURE;
    }

    /* Clear the storage area after the size and CRC info*/
    mram_erase_result = MRAM_Erase_NonSequential((BL_FS_BOOT_INFO_DATA_SPACE + sizeof(uint64_t)),
                                                ((p_storageToMake->FSDataSize - sizeof(uint64_t)) >> 2));
    if (mram_erase_result != MRAM_ERR_NONE)
    {
        return BL_FS_MRAM_ERASE_FAILURE;
    }

    /* Write an empty inode table */
    memset(&p_storageToMake->inode_table, 0, sizeof(BL_FSINodeTableBootInfo_t));
    p_storageToMake->inode_table.startBytes = BL_FS_INODE_START_BYTES;
    p_storageToMake->inode_table.endBytes = BL_FS_INODE_END_BYTES;

    mram_erase_result = MRAM_WriteBuffer_NonSequential((uint32_t)p_storageToMake->p_FSINodeTable,
            (p_storageToMake->FSINodeTableSize >> 2), (uint32_t*)&p_storageToMake->inode_table);

    /* Check if there are any free inodes */
    BL_FSINode_t *p_inode = BL_FStoreFindFreeInode(p_storageToMake);
    if (p_inode == NULL)
    {
        return BL_FS_FS_FULL;
    }

    uint64_t size_and_CRC = 0;
    MRAM_ReadDoubleWord(BL_FS_BOOT_INFO_DATA_SPACE, (uint32_t*)&size_and_CRC);

    p_inode->crc= BL_FSCalculateFileCRC((uint32_t*)&size_and_CRC, (sizeof(size_and_CRC) >> 2));
    p_inode->p_fileStartAddr = (uint32_t*) BL_FS_BOOT_INFO_DATA_SPACE;
    p_inode->id = BL_FS_SIZE_AND_CRC_ID;
    p_inode->flags = (BL_FS_CAN_READ | BL_FS_CAN_WRITE);
    p_inode->size = sizeof(uint64_t);

    BL_FStoreSaveInodeTable(p_storageToMake);

    BL_FStoreStatus_t status = BL_FStoreValidate(BL_FS_INFO_STORAGE);

    return status;
}

BL_FStoreStatus_t BL_FStoreFileList(BL_FSStorageLocation_t location, uint8_t *p_buffer, uint16_t *p_max_size, bool show_hidden)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    if (!BL_FSHasValidFileSystem(p_storage))
    {
        return BL_FS_INVALID_OPERATION;
    }

    if (*p_max_size < (p_storage->FSINodeCount * sizeof(BL_FSINodeTableBootInfo_t)))
    {
        return BL_FS_INVALID_OPERATION;
    }

    uint8_t *p_ptr = p_buffer;
    for (uint32_t index = 0; index < p_storage->FSINodeCount; index++)
    {
        BL_FSINode_t *p_inode = &p_storage->inode_table.inodes[index];
        if (p_inode->id > BL_FS_NOFILE)
        {
            if ((p_inode->id >= BL_FS_MAX_RESERVED_FILE) || show_hidden)
            {
                memcpy(p_ptr, (uint8_t*) p_inode, sizeof(BL_FSINode_t));
                p_ptr += sizeof(BL_FSINode_t);
            }
        }
    }

    *p_max_size = (p_ptr - p_buffer);

    return BL_FS_NO_ERROR;
}

uint32_t BL_FStoreFileSize(BL_FSStorageLocation_t location, BL_FSFileId_t id)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    BL_FSINode_t *p_inode = BL_FStoreFindFile(p_storage, id);
    if (p_inode == NULL)
    {
        return 0xFFFF;
    }

    return p_inode->size;
}

bool BL_FStoreFileExists(BL_FSStorageLocation_t location, BL_FSFileId_t id)
{

    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    BL_FSINode_t *p_inode = BL_FStoreFindFile(p_storage, id);
    return (p_inode != NULL);
}

bool BL_FStoreFileCanRead(BL_FSStorageLocation_t location, BL_FSFileId_t id)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    return BL_FStoreIsFlagSet(p_storage, id, BL_FS_CAN_READ);
}

bool BL_FStoreFileCanWrite(BL_FSStorageLocation_t location, BL_FSFileId_t id)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    return BL_FStoreIsFlagSet(p_storage, id, BL_FS_CAN_WRITE);
}

bool BL_FStoreFileCanDelete(BL_FSStorageLocation_t location, BL_FSFileId_t id)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    return BL_FStoreIsFlagSet(p_storage, id, BL_FS_CAN_DELETE);
}

BL_FStoreStatus_t BL_FStoreWrite(BL_FSStorageLocation_t location, BL_FSFileId_t id, uint32_t *p_buffer, uint16_t size, uint16_t flags)
{
    BL_FStoreStatus_t status = BL_FS_INVALID_OPERATION;

    /* Make sure the file doesn't exist already */
    if (BL_FStoreFileExists(location, id))
    {
        status = BL_FStoreUpdateExistingFile(location, id, p_buffer,size, flags);
    }
    else
    {
        status = BL_FStoreNewFile(location, id, p_buffer,size, flags);
    }

    return status;
}

BL_FStoreStatus_t BL_FStoreRead(BL_FSStorageLocation_t location, BL_FSFileId_t id, uint32_t *p_buffer, uint16_t *p_max_size)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    BL_FSINode_t *p_inode = BL_FStoreFindFile(p_storage, id);
    if (p_inode == NULL)
    {
        *p_max_size = 0;
        return BL_FS_MISSING_FILE;
    }

    if (p_inode->size > *p_max_size)
    {
        *p_max_size = 0;
        return BL_FS_INVALID_OPERATION;
    }

#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    if (BL_EncryptResetDecryption(BL_FSIVFromInode(p_inode)) != BL_ENCRYPT_NO_ERROR)
    {
        *p_max_size = 0;
        return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /*(BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */


    uint32_t *p_sector = p_inode->p_fileStartAddr;
    uint32_t *p_dst = p_buffer;

    if (BL_FSRead(p_dst, p_sector, p_inode->size, p_storage->encrypted) != BL_FS_NO_ERROR)
    {
        return BL_FS_MRAM_READ_FAILURE;
    }

    /* Calculate the CRC of the stored file */
    uint16_t calculatedCRC = BL_FSCalculateFileCRC(p_buffer, (p_inode->size >> 2));
    if (calculatedCRC != p_inode->crc)
    {
        return BL_FS_CRC_MISMATCH;
    }
#ifdef BL_OPT_FEATURE_SECURE_STORAGE
#if (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED)
    if (BL_EncryptComplete() != BL_ENCRYPT_NO_ERROR)
    {
        return BL_FS_ENCRYPTION_FAILURE;
    }
#endif /* (BL_OPT_FEATURE_SECURE_STORAGE == BL_OPT_FEATURE_ENABLED) */
#endif /* BL_OPT_FEATURE_SECURE_STORAGE */
    *p_max_size = p_inode->size;

    return BL_FS_NO_ERROR;
}

/* TODO: Implement Deleting files */
BL_FStoreStatus_t BL_FStoreDelete(BL_FSStorageLocation_t location, BL_FSFileId_t id)
{
    BL_FSStorage_t *p_storage = BL_FStoreSelectWorkingLocation(location);

    BL_FSINode_t *p_inode = BL_FStoreFindFile(p_storage, id);
    if (p_inode == NULL)
    {
        return BL_FS_MISSING_FILE;
    }

    uint32_t remaining = p_inode->size;
    uint8_t *p_sector = (uint8_t *) p_storage->p_FSDataSpace;
    uint32_t mask = 0x0001;
    uint32_t word = 0;
    while (remaining > 0)
    {
//        if (p_inode->blocks[word] & mask)
//        {
//            uint32_t size = MIN(remaining, BL_FS_BLOCK_SIZE);
//
//            uint32_t mram_erase_result = ERRNO_NO_ERROR;
//
//            mram_erase_result = MRAM_EraseSector((uint32_t) p_sector);
//
//            if (mram_erase_result != MRAM_ERR_NONE)
//            {
//                if (mram_erase_result == MRAM_ERR_UNKNOWN)
//                {
//                    if ((MRAM_BlankCheck((uint32_t) p_sector, 1024) != MRAM_ERR_NONE))
//                    {
//                        return BL_FS_MRAM_ERASE_FAILURE;
//                    }
//                }
//                else
//                {
//                    return BL_FS_MRAM_ERASE_FAILURE;
//                }
//            }
//            remaining -= size;
//        }

        p_sector += BL_FS_MIN_FILE_SIZE;
        mask <<= 1;
        if (mask > 0x8000)
        {
            mask = 0x0001;
            word += 1;
        }
    }

    memset(p_inode, 0, sizeof(BL_FSINode_t));
    return BL_FStoreSaveInodeTable(p_storage);
}
