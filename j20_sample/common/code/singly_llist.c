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
 * @file   singly_llist.c
 * @brief  Source code for the Data Structures Library Singly Linked List
 */

/*************************************************************************************************
 *  Include files
 *************************************************************************************************/
#include <stddef.h>
#include "../include/singly_llist.h"

/*************************************************************************************************
 *  Module Symbolic Constants
 *************************************************************************************************/

/*************************************************************************************************
 *  Module Macros
 *************************************************************************************************/

/*************************************************************************************************
 *  Module Type Definitions
 *************************************************************************************************/

/*************************************************************************************************
 *  Module Variable Definitions
 *************************************************************************************************/

/*************************************************************************************************
 *  Function Prototypes
 *************************************************************************************************/

/*************************************************************************************************
 *  Function Definitions
 *************************************************************************************************/
void SNodeInit(SNode_t * p_node, uint32_t key, void *p_data)
{
    p_node->key = key;
    p_node->p_data = p_data;
    p_node->p_next = NULL;
}

/**
 * @brief Initialize Singly Linked List object
*/
void SListInit(SLlist_t *p_list)
{
    p_list->p_head = NULL;
    p_list->curr_size = 0;
}

bool SListEmpty(SLlist_t *p_list)
{
    return (0 == p_list->curr_size);
}

uint32_t SListSize(SLlist_t *p_list)
{
    return p_list->curr_size;
}

/**
 * @brief Insert Node at Head
*/
void SListInsert(SLlist_t *p_list, SNode_t *p_node)
{
    p_node->p_next = p_list->p_head;
    p_list->p_head = p_node;
    p_list->curr_size++;
}

/**
 * @brief Traverse the list to a given index and return the node pointer
*/
SNode_t *SListGet(SLlist_t *p_list, uint32_t key)
{
    SNode_t *curr = p_list->p_head;
    while ((curr != NULL) && (curr->key != key))
    {
        curr = curr->p_next;
    }
    
    return curr;
}

SNode_t *SListRemove(SLlist_t *p_list, uint32_t key)
{
    SNode_t *p_prev = NULL;
    SNode_t *p_curr = p_list->p_head;

    while ((p_curr != NULL) && (p_curr->key != key))
    {
        p_prev = p_curr;
        p_curr = p_curr->p_next;
    }

    if (p_curr != NULL)
    {
        if (p_curr == p_list->p_head)
        {
            p_list->p_head = p_curr->p_next;
        }
        else
        {
            p_prev->p_next = p_curr->p_next;
        }
        p_list->curr_size--;
    }

    return p_curr;
}
