/**
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
 *
 * @file   singly_llist.h
 * @brief  Header file for the Data Structures Library Singly Linked List
 */

#ifndef S_LLIST_H
#define S_LLIST_H

#if __cpluplus
extern "C" {
#endif

/*************************************************************************************************
 *  Includes
 *************************************************************************************************/
#include <stdint.h>
#include <stdbool.h>

/*************************************************************************************************
 *  Forward Declarations
 *************************************************************************************************/


/*************************************************************************************************
 *  Type Definitions
 *************************************************************************************************/
typedef struct SNode
{
    uint32_t key;            /* Unique key used to search list */
    void *p_data;            /* Pointer to any struct used to hold data */
    struct SNode *p_next;
} SNode_t;

typedef struct SLlist
{
    uint32_t curr_size;
    SNode_t *p_head;
} SLlist_t;

/*************************************************************************************************
 *  Function Prototypes
 *************************************************************************************************/
void SNodeInit(SNode_t *, uint32_t, void *);

void SListInit(SLlist_t *);
bool SListEmpty(SLlist_t *);
uint32_t SListSize(SLlist_t *);
void SListInsert(SLlist_t *, SNode_t *);
SNode_t *SListGet(SLlist_t *, uint32_t);
SNode_t *SListRemove(SLlist_t *, uint32_t);

#if __cpluplus
}
#endif
#endif /* S_LLIST_H */
