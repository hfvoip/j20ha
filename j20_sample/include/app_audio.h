/**
 * @file app_audio.h
 * @brief Header file for audio path
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

#ifndef APP_AUDIO_H_
#define APP_AUDIO_H_

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
 * ------------------------------------------------------------------------- */
#define AUDIO_BLOCK					16

#define AUDIO_STREAMS               2

#ifdef USER_ENABLE_DEBUG_WAVEFORMS
#define GPIO_OUTPUT_CONFIG         (GPIO_LPF_DISABLE | GPIO_NO_PULL | GPIO_2X_DRIVE)
#define DBG_DIO0                    8
#define DBG_DIO1                    10
#define DBG_DIO2                    12
#define DBG_DIO3                    13
#define DBG_DIO4                    14
#define DBG_DIO5                    15
#endif


#ifdef USER_IN_BUFFER_SIZE
#define AUDIO_IN_BUFFER_SIZE        USER_IN_BUFFER_SIZE
#else
#define AUDIO_IN_BUFFER_SIZE        96
#endif

#ifdef USER_OUT_BUFFER_SIZE
#define AUDIO_OUT_BUFFER_SIZE       USER_OUT_BUFFER_SIZE
#else
#define AUDIO_OUT_BUFFER_SIZE       96
#endif


/* Audio buffers */


/* ---------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief Configure DMIC and OD peripherals
 */
void APP_Audio_Init(void);

/**
 * @brief Start audio path
 */
void APP_Audio_Start(void);

/**
 * @brief Stop Audio Path and disable DMIC and OD peripherals
 */
void APP_Audio_Stop(void);

/**
 * @brief Update Audio State
 */
void APP_Audio_Run(void);

/**
 * @brief Audio interrupt handler (only used for errors)
 */
void AUDIO_IRQHandler(void);

/**
 * @brief Interrupt Handler for ASRC Input DMA
 */
void DMA0_IRQHandler(void);

/**
 * @brief Interrupt Handler for PCM Input DMA
 */
void DMA4_IRQHandler(void);

/**
 * @brief Interrupt Handler for ASRC completion
 */
void ASRC_IDLE_IRQHandler(void);



/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* APP_AUDIO_H_ */
