/**
 * @file app_codecs.h
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

#ifndef APP_CODECS_H_
#define APP_CODECS_H_

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

/* ----------------------------------------------------------------------------
 * Defines
 * ------------------------------------------------------------------------- */

/* ---------------------------------------------------------------------------
 * Function prototype definitions
 * --------------------------------------------------------------------------*/

/**
 * @brief Configure codecs
 */
void App_Codec_Init(void);

/**
 * @brief Load codecs
 */
void App_Codec_Load(void);

void App_Codec_ExecutePendingAction(void);

/**
 * @brief Stops codec
 */
void App_Codec_Stop(void);


/**
 * @brief Set the volume gain for the decoder output
 * @param gain:        15 bit fixed decimal value of the gain
 */
void App_Codec_SetDecoderVolumeGain(int16_t gain);

/**
 * @brief Reconfigures the DSP, reloading code and resetting it
 */
void App_Codec_Reconfig(void);


/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* APP_CODECS_H_ */
