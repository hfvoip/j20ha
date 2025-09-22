/**
 * @file loader.h
 * @brief Loader library support
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

#ifndef LOADER_H_
#define LOADER_H_

#include <stdint.h>

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif /* ifdef __cplusplus */

#define _MEMORY_DESCRIPTION_            1
#define _MEMORY_OVERVIEW_DESCRIPTION_   1
#define DSS32_BRAM0                     0x00800000

#define LPDSP_PM_WORD_IN_BYTE 			6
#define CM33_PM_LOADED_WORD_IN_BYTE 	5

/* Flag that controls whether to use the time logging feature for loader */
#define LOAD_TIME_LOG_GPIO_ENABLED      0
#if LOAD_TIME_LOG_GPIO_ENABLED
    /* The GPIO that is used for logging time for the loader application */
    #define LOAD_TIME_LOG_GPIO          1
#endif /* LOAD_TIME_LOG_GPIO_ENABLED */

typedef struct
{
    void *buffer;
    uint32_t fileSize;
    uint32_t memSize;
    uint32_t vAddress;
} memoryDescription;

typedef struct
{
    memoryDescription *entries;
    uint32_t count;
} memoryOverviewEntry;

typedef struct
{
    memoryOverviewEntry PM_cntx;
    memoryOverviewEntry DMA_cntx;
    memoryOverviewEntry DMB_cntx;
} memoryOverview;

/* Normally when loading the LPDSP32 we can just use this routine, added core
 * 0 or 1 for RSL20 LPDSP32 support */
void loadDSPMemory(memoryOverview *overview, uint8_t core);

/* provide a hook to explicitly reset the loop cache */
void resetLoopCache(void);

/* For cases where finer control of the loading process is required, we expose
 * the following helper functions. added core for Jordan multi-LPDSP32 support for
 * PRAM.
 */
void loadSinglePRAMEntry(memoryDescription *descriptor, uint8_t core);

void loadDSPDRAM(memoryOverviewEntry *dma_cntx, memoryOverviewEntry *dmb_cntx);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif /* ifdef __cplusplus */

#endif    /* LOADER_H_ */
