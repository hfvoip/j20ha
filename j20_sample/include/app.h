/**
 * @file app.h
 * @brief Application header file for the Bidirectional PCM Audio Passthrough sample application
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

#ifndef APP_H
#define APP_H

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


#include <asrc.h>

/* ----------------------------------------------------------------------------
 * Defines
 * ------------------------------------------------------------------------- */
#define CONCAT(x, y)                    x##y
#define THREE_BLOCK_APPN(x, y, z)       x##y##z
#define DMA_NUM(x)                      CONCAT(DMA, x)
#define DMA_IRQn(x)                     THREE_BLOCK_APPN(DMA, x, _IRQn)
#define DMA_IRQ_FUNC(x)                 THREE_BLOCK_APPN(DMA, x, _IRQHandler)

/* De-bounce definition*/
#define MAX_DEBOUNCE                    255

#define UINT32_MAX_VAL  0xFFFFFFFF

//48M 运行，如果是0,为16M
#define	APP_CPU_AT_FULL_SPEED			1

/* ----------------------------------------------------------------------------
 * Globals
 * --------------------------------------------------------------------------*/

/* Error counters - Should always equal zero */
extern uint32_t asrc_in_error_count;
extern uint32_t asrc_update_error_count;
extern uint32_t pcm_underrun_count;
extern uint32_t pcm_overrun_count;

/* State Machine flags */
extern uint8_t asrc_update_flag;

extern volatile uint16_t app_audio_int;

/* ---------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief Initialize system for application
 */
void APP_Initialize (void);

/**
 * @brief Disable interrupts
 */
void APP_DisableInterrupts(void);

/**
 * @brief Clear CNTS and buffers for all DMA channels used
 */
void APP_ClearDMAChannels(void);

/**
 * @brief Reset and disable all interfaces used in the audio path
 */
void APP_DisableAudioPath(void);

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif    /* APP_H */
