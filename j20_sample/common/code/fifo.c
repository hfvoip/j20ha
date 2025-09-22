/**
 * @file fifo.c
 * @brief Source code for a (software) circular buffer with sizes in powers of 2
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

#include "../include/fifo.h"

/* Returns the maximum number of words that can be held in the FIFO.
 * The current construction of the FIFO, which has no 'current' word count,
 * causes us to lose the use of one word of the FIFO, but should gain read/write
 * thread independence... */
fifo_type FIFO_MaxWords(FIFO_t *p_fifo)
{
    return p_fifo->buffer_words - 1;
}

/* 'Thread safe' for reading. */
bool FIFO_IsEmpty(FIFO_t *p_fifo)
{
    return p_fifo->read_i==p_fifo->write_i;
}

/* 'Thread safe' for writing. */
bool FIFO_IsFull(FIFO_t *p_fifo)
{
    return (p_fifo->read_i)? p_fifo->read_i==p_fifo->write_i+1 :
                             p_fifo->buffer_words==p_fifo->write_i+1;

    /* r -> 0, but changes after test ==> OK,
     *   as false branch doesn't depend on r  (can get false full, which is OK)
     * r -> MFP-1, but wraps after test ==> OK,
     *   as r advancing can't cause a false !full
     * r -> MFP (due to interrupt of dequeue) ==> OK to put single word in FIFO,
     * as data has already been read */
}

/* 'Thread safe' for reading.
 * Returns the number of FIFO words occupied by valid data. */
fifo_type FIFO_WordsFull(FIFO_t *p_fifo)
{
    fifo_type words = p_fifo->write_i + p_fifo->buffer_words - p_fifo->read_i;      /* latch indices */

    return (words>=p_fifo->buffer_words)? words - p_fifo->buffer_words : words;     /* test */
}

/* 'Thread safe' for writing.
 * Returns the number of FIFO words available for writing.
 * (One word is lost in this implementation, as read == write means empty.) */
fifo_type FIFO_WordsEmpty(FIFO_t *p_fifo)
{
    return p_fifo->buffer_words-1 - FIFO_WordsFull(p_fifo);
}

#if DEBUG_FIFO_BUFFER
/* Read and reset the FIFO's high watermark. */
fifo_type FIFO_HighWatermark(FIFO_t *p_fifo)
{
    fifo_type max_buffer_words = p_fifo->max_buffer_words;

    p_fifo->max_buffer_words = FIFO_WordsFull(p_fifo);

    return max_buffer_words;
}
#endif

/* Initialize a FIFO.
 * fifo_bufptr should be a pointer to an array of words of length fifo_words
 * ( == FIFO_WORDS). */
void FIFO_Init(FIFO_t *p_fifo, fifo_type fifo_words, word_type *fifo_bufptr)
{
    p_fifo->write_i      = 0;
    p_fifo->read_i       = 0;
    p_fifo->buffer_words = fifo_words;
    p_fifo->p_buffer = fifo_bufptr;

#if DEBUG_FIFO_BUFFER
    p_fifo->max_buffer_words = 0;
#endif
}

/* Reset a FIFO to its initial state (without freeing or reinitializing the buffer). */
void FIFO_Reset(FIFO_t *p_fifo)
{
    p_fifo->write_i = 0;
    p_fifo->read_i  = 0;
}

/* Add a word to the FIFO.
 * Doesn't check for buffer fullness!
 * 'Thread safe' for writing. */
void FIFO_EnqueueWord(FIFO_t *p_fifo, word_type word)
{
    p_fifo->p_buffer[p_fifo->write_i++] = word;             /* write
                                                             * increment index */

    if (p_fifo->buffer_words==p_fifo->write_i)              /* wrap index */
    {
        p_fifo->write_i = 0;
    }

#if DEBUG_FIFO_BUFFER
    if (FIFO_WordsFull(p_fifo)>p_fifo->max_buffer_words)
    {
        p_fifo->max_buffer_words = FIFO_WordsFull(p_fifo);
    }
#endif
}

/* Add an array of n words to the FIFO.
 * Doesn't check for buffer fullness! */
void FIFO_EnqueueWords(FIFO_t *p_fifo, word_type *wordp, fifo_type n)
{
    fifo_type words_til_wrap = p_fifo->buffer_words - p_fifo->write_i;

    if (n>=words_til_wrap)
    {
        n -= words_til_wrap;

        while (words_til_wrap--)
        {
            p_fifo->p_buffer[p_fifo->write_i++] = *wordp++;
        }

        p_fifo->write_i = 0;
    }

    while (n--)
    {
        p_fifo->p_buffer[p_fifo->write_i++] = *wordp++;
    }

#if DEBUG_FIFO_BUFFER
    if (FIFO_WordsFull(p_fifo)>p_fifo->max_buffer_words)
    {
        p_fifo->max_buffer_words = FIFO_WordsFull(p_fifo);
    }
#endif
}

/* Remove and return a word from the FIFO.
 * Doesn't check for buffer emptiness!
 * 'Thread safe' for reading. */
word_type FIFO_DequeueWord(FIFO_t *p_fifo)
{
    word_type word = p_fifo->p_buffer[p_fifo->read_i++];    /* read
                                                             * increment index */

    if (p_fifo->buffer_words==p_fifo->read_i)               /* wrap index */
    {
        p_fifo->read_i = 0;
    }

    return word;
}

/* Remove and return an array of n words from the FIFO.
 * Caller should provide a buffer of length n.
 * Returns the actual number of words dequeued.
 * Note that all words will be in network (BIG-ENDIAN) order. */
fifo_type FIFO_DequeueWords(FIFO_t *p_fifo, word_type *wordp, fifo_type n)
{
    fifo_type words_in_fifo    = FIFO_WordsFull(p_fifo);
    fifo_type words_to_dequeue = (n>words_in_fifo)? words_in_fifo : n;
    fifo_type words_til_wrap   = p_fifo->buffer_words - p_fifo->read_i;

    n = words_to_dequeue;

    if (n>=words_til_wrap)
    {
        n -= words_til_wrap;

        while (words_til_wrap--)
        {
            *wordp++ = p_fifo->p_buffer[p_fifo->read_i++];
        }

        p_fifo->read_i = 0;
    }

    while (n--)
    {
        *wordp++ = p_fifo->p_buffer[p_fifo->read_i++];
    }

    return words_to_dequeue;
}

/* Return the value of the nth oldest FIFO bin.
 * Return value is undefined if n > p_fifo->buffer_words. */
word_type FIFO_GetWord(FIFO_t *p_fifo, fifo_type n)
{
    fifo_type word_to_get = p_fifo->read_i + n;

    while (p_fifo->buffer_words<=word_to_get)
    {
        word_to_get -= p_fifo->buffer_words;
    }

    return p_fifo->p_buffer[word_to_get];
}

/* Remove up to the n oldest words from the FIFO.
 * Returns the actual number of words dumped. */
fifo_type FIFO_DumpWords(FIFO_t *p_fifo, fifo_type n)
{
    fifo_type words_in_fifo  = FIFO_WordsFull(p_fifo);
    fifo_type words_to_erase = (n>words_in_fifo)? words_in_fifo : n;

    p_fifo->read_i += words_to_erase;

    while (p_fifo->buffer_words<=p_fifo->read_i)
    {
        p_fifo->read_i -= p_fifo->buffer_words;
    }

    return words_to_erase;
}
