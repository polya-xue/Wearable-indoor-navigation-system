/******************************************************************************
*
* THE PRESENT CODE AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME.
* AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
* INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE
* CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING
* INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
*
* (C) 2013 STMicroelectronics International NV
*
*   FILENAME        -  list.c
*
*   COMPILER        -  gnu gcc
*
*******************************************************************************
*
*   CHANGE HISTORY
*   ---------------------------
*
*   $Date$:      09/03/2012
*   $Revision$:  first version
*   $Author$:    
*   Comments: 
*
*******************************************************************************
*
*  File Description 
*  ---------------------
* 
*
******************************************************************************/
#ifndef _LIST_H_
#define _LIST_H_

/******************************************************************************
 * Include Files
******************************************************************************/

/******************************************************************************
 * Macro Declarations
******************************************************************************/

/******************************************************************************
 * Function Prototype 
******************************************************************************/

typedef struct _tListNode {
	struct _tListNode * next;
	struct _tListNode * prev;
}tListNode, *pListNode;

void list_init_head (tListNode * listHead);

BOOL list_is_empty (tListNode * listHead);

void list_insert_head (tListNode * listHead, tListNode * node);

void list_insert_tail (tListNode * listHead, tListNode * node);

void list_remove_node (tListNode * node);

void list_remove_head (tListNode * listHead, tListNode ** node );

void list_remove_tail (tListNode * listHead, tListNode ** node );

void list_insert_node_after (tListNode * node, tListNode * ref_node);

void list_insert_node_before (tListNode * node, tListNode * ref_node);

int list_get_size (tListNode * listHead);

void list_get_next_node (tListNode * ref_node, tListNode ** node);

void list_get_prev_node (tListNode * ref_node, tListNode ** node);

#endif /* _LIST_H_ */
