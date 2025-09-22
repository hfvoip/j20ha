/**
 * @file loader.c
 * @brief Loader library to load LPDSP32 program from ARM Cortex-M33
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

#include <string.h>
#include <hw.h>
#include "loader.h"

/**
 * @brief       This loads a single PRAM descriptor into the LPDSP32 PRAM
 * @param[in]   descriptor  The descriptor for the memory area to be
 *                          copied.
 */
void loadSinglePRAMEntry(memoryDescription *descriptor, uint8_t core)
{
	uint32_t memSize = (descriptor->memSize * CM33_PM_LOADED_WORD_IN_BYTE / LPDSP_PM_WORD_IN_BYTE);
	if (core == 0) {
		memcpy((void *) (DSP0_PM_BASE + ((descriptor->vAddress * 2) & 0xFFFF)),
			   descriptor->buffer, memSize);
	}
	else if (core == 1) {
		// DSP1_PM_BASE points to the DSP1 program memory area which
		// takes into account the different configuration of the program
		// memory for Core 1 (2) this allows us to simply copy in the buffer.
		memcpy((void *) (DSP1_PM_BASE + ((descriptor->vAddress * 2) & 0xFFFF)),
			   descriptor->buffer, memSize);
	}
}

/**
 * @brief       Loads the LPDSP32 PRAM with all the program sections.
 * @param[in]   pram    A memory overview object which provides a list of
 *                      memory sections which need to be copied
 */
static void loadDSPPRAM(memoryOverviewEntry *pram, uint8_t core)
{
    for (unsigned int i = 0; i < pram->count; i++)
    {
        loadSinglePRAMEntry(&pram->entries[i], core);
    }
}

/**
 * @brief       Maps an address from the LPDSP32 address space into CM33
 * @param[in]   vAddress    An address in LPDSP32 memory space which must
 *                          be mapped to a CM33 address
 * @return      The CM33 address corresponding to the LPDSP32 address
 */
static uint32_t mapToCM33Space(uint32_t vAddress)
{
	/* for Jordan we don't need to be concerned about the DRAM as it is shared
	 * between the two cores, the linker configuration files will determine how
	 * to use the DRAM for each core.
	 */
	if (vAddress < 0x00800000)
    {
        /* this is a DMA address, adjust accordingly */
    	// return vAddress + DSP_DMA_BASE;
    	if (vAddress >= 0x00008000) {
    		return ((vAddress - 0x8000) + DSP_DRAM56_BASE);
    	}

		return vAddress + DSP_BRAM01_BASE;

    }
    else
    {
    	// DRAM8_BASE
		// return vAddress - DSS32_BRAM0 + DSP_DRAM78_BASE;
    	return vAddress - DSS32_BRAM0 + DSP_BRAM0_BASE;
    }
}

/**
 * @brief       Helper method to copy a single block of DRAM to the LPDSP32
 * @param[in]   dram    A memory descriptor defining the block of memory to
 *                      be copied
 */
static void initializeDSPDRAM(memoryDescription *dram)
{
    uint32_t size;
    uint32_t dspAddress;

    size = dram->fileSize;
    dspAddress = mapToCM33Space(dram->vAddress);

    if (size > 0)
    {
        /* If the dspAddress is not word aligned, skip the initial bytes added
         * as padding by the elfConverter script */
        unsigned char *src = dram->buffer + (dspAddress & 0x3);
        unsigned char *dst = (unsigned char *)dspAddress;
        while (size)
        {
            *dst++ = *src++;
            size--;
        }
    }
}

/**
 * @brief       Loads the DRAM associated with a program to the LPDSP32
 * @param[in]   dma_cntx     A memory overview object specifying the data memory A area
 * @param[in]   dmb_cntx     A memory overview object specifying the data memory B area
 */
void loadDSPDRAM(memoryOverviewEntry *dma_cntx, memoryOverviewEntry *dmb_cntx)
{
    for (int i = 0; i < dma_cntx->count; i++) initializeDSPDRAM(&dma_cntx->entries[i]);
    for (int i = 0; i < dmb_cntx->count; i++) initializeDSPDRAM(&dmb_cntx->entries[i]);
}

/**
 * @brief       Helper method to reset the loop cache on the LPDSP32
 */
void resetLoopCache(void)
{
    SYSCTRL->CM33_LOOP_CACHE_CFG = 0;
    SYSCTRL->CM33_LOOP_CACHE_CFG = 1;
}

/**
 * @brief       Generic loader that can be used to load simple programs
 *              from flash to the LPDSP32 PRAM and DRAM
 * @param[in]   overview    An overview object that contains the
 *                          specifications for all the PRAM and DRAM
 *                          sections to be copied.
 */
void loadDSPMemory(memoryOverview *overview, uint8_t core)
{
    /* Set the TEST_GPIO_TIME_MRAM_MEMCPY to high */
#if (LOAD_TIME_LOG_GPIO_ENABLED == 1)
    Sys_GPIO_Set_High(LOAD_TIME_LOG_GPIO);
#endif /* LOAD_TIME_LOG_GPIO_ENABLED */
    loadDSPPRAM(&overview->PM_cntx, core);
    loadDSPDRAM(&overview->DMA_cntx, &overview->DMB_cntx);

    /* Run LPDSP32 */
    resetLoopCache();
    if (core == 0)
    	DSP->CTRL[0] = DSP_RESET;
    else if (core == 1)
    	DSP->CTRL[1] = DSP_RESET;
    /* Set the TEST_GPIO_TIME_MRAM_MEMCPY to low */
#if (LOAD_TIME_LOG_GPIO_ENABLED == 1)
    Sys_GPIO_Set_Low(LOAD_TIME_LOG_GPIO);
#endif /* LOAD_TIME_LOG_GPIO_ENABLED */
}
