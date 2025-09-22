/**
 * @file app_asrc.h
 * @brief Header file for sample input signal buffers
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

#ifndef SAMPLE_INPUT_H_
#define SAMPLE_INPUT_H_

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

#define NUM_SAMPLES_INPUT_1KHZ_24KFS 24

/* ----------------------------------------------------------------------------
 * Globals
 * --------------------------------------------------------------------------*/

/* 16-bit 1kHz 0 dBFS tone sampled at Fs=24kHz */
extern const int16_t sample_input_1kHz_24kFs[NUM_SAMPLES_INPUT_1KHZ_24KFS];

/* ----------------------------------------------------------------------------
 * Close the 'extern "C"' block
 * ------------------------------------------------------------------------- */
#ifdef __cplusplus
}
#endif    /* ifdef __cplusplus */

#endif /* SAMPLE_INPUT_H_ */
