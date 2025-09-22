/**
 * @file fifo.h
 * @brief Interface for a (software) circular buffer
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

#ifndef FIFO_H
#define FIFO_H

/* Includes */
#include <stdbool.h>
#include <stdint.h>

/* Defines & Switches */
#define DEBUG_FIFO_BUFFER                   0

/* The maximum depth of the FIFO (is one less than this). */
/* Note:  This macro is an optional convenience -
 * it is recommended to use it, but it is not used directly in the FIFO source. */
#define FIFO_WORDS                          16u

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
#if DEBUG_FIFO_BUFFER
    fifo_type  max_buffer_words;
#endif
    word_type *p_buffer;
} FIFO_t;

/* Prototypes */
fifo_type FIFO_MaxWords(FIFO_t *);
bool      FIFO_IsEmpty(FIFO_t *);
bool      FIFO_IsFull(FIFO_t *);
fifo_type FIFO_WordsFull(FIFO_t *);
fifo_type FIFO_WordsEmpty(FIFO_t *);
#if DEBUG_FIFO_BUFFER
fifo_type FIFO_HighWatermark(FIFO_t *);
#endif
void      FIFO_Init(FIFO_t *, fifo_type, word_type *);
void      FIFO_Reset(FIFO_t *);
void      FIFO_EnqueueWord(FIFO_t *, word_type);
void      FIFO_EnqueueWords(FIFO_t *, word_type *, fifo_type);
word_type FIFO_DequeueWord(FIFO_t *);
fifo_type FIFO_DequeueWords(FIFO_t *, word_type *, fifo_type);
word_type FIFO_GetWord(FIFO_t *, fifo_type);
fifo_type FIFO_DumpWords(FIFO_t *, fifo_type);

#endif /* FIFO_H */
