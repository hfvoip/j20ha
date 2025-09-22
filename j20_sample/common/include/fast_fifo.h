/**
 * @file fast_fifo.h
 * @brief Interface for a (software) circular buffer with sizes in powers of 2
 *
 * @copyright @parblock
 * Copyright (c) 2024 Semiconductor Components Industries, LLC (d/b/a
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

#ifndef FAST_FIFO_H
#define FAST_FIFO_H

/* Includes */
#include <stdbool.h>
#include <stdint.h>

/* Defines & Switches */
#define DEBUG_FIFO_BUFFER                   0

/* Types & Structures */

/* FIFO Types */
typedef int32_t  word_type;         /* data word type */
typedef uint32_t fifo_type;         /* All indices and buffer sizes must
                                     * use the same data type. */

/* FIFO */
typedef struct {
    fifo_type  write_i;
    fifo_type  read_i;
    fifo_type  buffer_words;
    fifo_type  buffer_mask;
#if DEBUG_FIFO_BUFFER
    fifo_type  max_buffer_words;
#endif
    word_type *p_buffer;
} F_FIFO_t;

/* Prototypes */
fifo_type F_FIFO_MaxWords(F_FIFO_t *);
bool      F_FIFO_IsEmpty(F_FIFO_t *);
bool      F_FIFO_IsFull(F_FIFO_t *);
fifo_type F_FIFO_WordsFull(F_FIFO_t *);
fifo_type F_FIFO_WordsEmpty(F_FIFO_t *);
#if DEBUG_FIFO_BUFFER
fifo_type F_FIFO_HighWatermark(F_FIFO_t *);
#endif
bool      F_FIFO_Init(F_FIFO_t *, fifo_type, word_type *);
void      F_FIFO_Reset(F_FIFO_t *);
void      F_FIFO_EnqueueWord(F_FIFO_t *, word_type);
void      F_FIFO_EnqueueWords(F_FIFO_t *, word_type *, fifo_type);
word_type F_FIFO_DequeueWord(F_FIFO_t *);
fifo_type F_FIFO_DequeueWords(F_FIFO_t *, word_type *, fifo_type);
word_type F_FIFO_GetWord(F_FIFO_t *, fifo_type);
fifo_type F_FIFO_DumpWords(F_FIFO_t *, fifo_type);

#endif /* FAST_FIFO_H */
