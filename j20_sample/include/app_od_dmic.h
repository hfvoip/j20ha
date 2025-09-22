/**
 * @file app_od_dmic.h
 * @brief Header file for code supporting DMIC and OD peripherals
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

#ifndef INCLUDE_APP_OD_DMIC_H_
#define INCLUDE_APP_OD_DMIC_H_

/* ----------------------------------------------------------------------------
 * If building with a C++ compiler, make all of the definitions in this header
 * have a C binding.
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
extern "C"
{
#endif    /* ifdef __cplusplus */

/* ----------------------------------------------------------------------------
 * Include files
 * --------------------------------------------------------------------------*/

#include <hw.h>

/* ----------------------------------------------------------------------------
 * Defines
 * --------------------------------------------------------------------------*/
#define OD_DMA                      3
#define DMIC_DMA                    2

#if RSL20_CID == 101
#define DMIC_CLK_DIO                GPIO11
#define DMIC_DATA_DIO               GPIO9
#else
#define DMIC_CLK_DIO                GPIO10
#define DMIC_DATA_DIO               GPIO9
#endif

#define DMIC_GPIO_CONFIG           (GPIO_LPF_DISABLE | GPIO_NO_PULL | GPIO_2X_DRIVE)

#define OD_DATA_16_MSB_ADDR        (((uint32_t)&AUDIO->OD_DATA))

#define AUDIO_ERROR_FLAG_MASK      (OD_UNDERRUN_DETECTED        | \
                                    DMIC0_OVERRUN_DETECTED      | \
                                    DMIC1_OVERRUN_DETECTED)

#define AUDIO_ERROR_CLEAR          (OD_UNDERRUN_FLAG_CLEAR      | \
                                    DMIC0_OVERRUN_FLAG_CLEAR    | \
                                    DMIC1_OVERRUN_FLAG_CLEAR)

//#define APP_DMIC0_GAIN              DMIC0_NOMINAL_GAIN
//#define APP_OD_GAIN                 OD_NOMINAL_GAIN

#define APP_DMIC0_GAIN              0x800
#define APP_OD_GAIN              	0x880

#define APP_OUTPUT_LIMITER          OUTPUT_LIMITER_OFF

/* DMIC/OD configuration: Decimated Sample Rate = 4 MHz / 128 = 31.25 kHz */
#define APP_AUDIO_CFG_NO_ENABLE    (DMIC_DECIMATE_BY_128        | \
                                    OD_UNDERRUN_PROTECT_ENABLE  | \
                                    OD_DMA_REQ_ENABLE           | \
                                    OD_DATA_MSB_ALIGNED         | \
                                    OD_DISABLE                  | \
                                    DMIC1_DMA_REQ_DISABLE       | \
                                    DMIC1_DATA_MSB_ALIGNED      | \
                                    DMIC1_DISABLE               | \
                                    DMIC0_DMA_REQ_ENABLE        | \
                                    DMIC0_DATA_MSB_ALIGNED      | \
                                    DMIC0_DISABLE)

#define APP_AUDIO_INT_CFG          (DMIC0_RDY_INT_DISABLE           | \
                                    DMIC1_RDY_INT_DISABLE           | \
                                    DMIC0_HF_RDY_INT_DISABLE        | \
                                    DMIC1_HF_RDY_INT_DISABLE        | \
                                    DMIC0_OVERRUN_INT_ENABLE        | \
                                    DMIC1_OVERRUN_INT_DISABLE       | \
                                    DMIC0_HF_OVERRUN_INT_DISABLE    | \
                                    DMIC1_HF_OVERRUN_INT_DISABLE    | \
                                    OD_REQ_INT_DISABLE              | \
                                    OD_HF_REQ_INT_DISABLE           | \
                                    OD_HF_RDY_INT_DISABLE           | \
                                    OD_UNDERRUN_INT_ENABLE          | \
                                    OD_HF_UNDERRUN_INT_DISABLE      | \
                                    OD_HF_OVERRUN_INT_DISABLE)
//2024-09-21:WORD_SIZE_16BITS_TO_16BITS   to WORD_SIZE_32BITS_TO_16BITS
#define OD_DMA_CFG                 (DMA_LITTLE_ENDIAN               | \
                                    DEST_TRANS_LENGTH_SEL           | \
                                    DMA_PRIORITY_0                  | \
                                    DMA_SRC_ALWAYS_ON               | \
                                    DMA_DEST_OD                     | \
                                    WORD_SIZE_32BITS_TO_32BITS      | \
                                    DMA_SRC_ADDR_INCR_1             | \
                                    DMA_DEST_ADDR_STATIC            | \
                                    DMA_SRC_ADDR_LSB_TOGGLE_DISABLE | \
                                    DMA_CNT_INT_DISABLE             | \
                                    DMA_COMPLETE_INT_ENABLE)

//2024-09-21:WORD_SIZE_16BITS_TO_16BITS   to WORD_SIZE_16BITS_TO_32BITS
#define DMIC_DMA_CFG               (DMA_LITTLE_ENDIAN               | \
                                    DEST_TRANS_LENGTH_SEL           | \
                                    DMA_PRIORITY_0                  | \
                                    DMA_SRC_DMIC                    | \
                                    DMA_DEST_ALWAYS_ON              | \
                                    WORD_SIZE_32BITS_TO_32BITS      | \
                                    DMA_SRC_ADDR_STATIC             | \
                                    DMA_DEST_ADDR_INCR_1            | \
                                    DMA_SRC_ADDR_LSB_TOGGLE_DISABLE | \
                                    DMA_CNT_INT_ENABLE              | \
                                    DMA_COMPLETE_INT_ENABLE)

#define APP_SDM_DCRM_CTRL          (DC_REMOVE_FREQ_55HZ    | \
                                    DC_REMOVE_ENABLE       | \
                                    IDC_REMOVE_FREQ_28HZ   | \
                                    IDC_REMOVE_ENABLE)



#define DMIC_CFG_NO_ENABLE         (DMIC0_DCRM_CUTOFF_FS_DIV_200 | \
                                    DMIC1_DCRM_CUTOFF_FS_DIV_200 | \
                                    DMIC1_DELAY_DISABLE          | \
                                    DMIC0_FALLING_EDGE           | \
                                    DMIC1_RISING_EDGE)

/* ----------------------------------------------------------------------------
 * Globals
 * --------------------------------------------------------------------------*/

/* ----------------------------------------------------------------------------
 * Function Prototypes
 * --------------------------------------------------------------------------*/

/**
 * @brief Configure the DMIC interface
 */
void APP_DMIC_Init(void);

/**
 * @brief Configure the OD interface
 */
void APP_OD_Init(void);

/**
 * @brief Initialize DMA channels for OD output
 */
void APP_OD_DMAInit(void);

/**
 * @brief Initialize DMA channels for DMIC input
 */
void APP_DMIC_DMAInit(void);

/**
 * @brief Enable the DMIC interface
 */
void APP_DMIC_Start(void);

/**
 * @brief Enable the OD interface
 */
void APP_OD_Start(void);

/**
 * @brief Disable OD & DMIC interfaces
 */
void APP_OD_DMIC_Stop(void);

/**
 * @brief Get pointer to most recently filled DMIC input buffer and
 * clear Inready status
 */
int16_t *APP_DMIC_GetReadyBufPtr(void);

/**
 * @brief Get pointer to OD output buffer
 */
int16_t *APP_OD_GetBufPtr(void);

/**
 * @brief Read buffer ready status
 */
bool APP_DMIC_Inready(void);

/**
 * @brief Interrupt Handler for the OD output DMA
 */
void DMA2_IRQHandler(void);

/**
 * @brief Interrupt Handler for the DMIC input DMA
 */
void DMA3_IRQHandler(void);


/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */


#endif /* INCLUDE_APP_OD_DMIC_H_ */
