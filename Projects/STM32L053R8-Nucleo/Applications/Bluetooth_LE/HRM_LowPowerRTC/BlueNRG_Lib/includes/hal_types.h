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
*   FILENAME        -  bluenrg_types.h
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
#ifndef _HCI_TYPES_H_
#define _HCI_TYPES_H_

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>


#if (PLATFORM_TYPE == PLATFORM_WINDOWS)
#include <Windows.h>
#endif
/******************************************************************************
 * Macro Declarations
******************************************************************************/

#if (PLATFORM_TYPE != PLATFORM_WINDOWS)

#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif
#ifndef NULL
#define NULL 0
#endif
#endif

//#define SUCCESS (0)
//#define FAILURE (-1)

#define YES TRUE
#define NO FALSE


/*#define NULL ((void *)0)*/


/******************************************************************************
 * Type defenetions
******************************************************************************/


typedef unsigned int uint;
typedef unsigned int uint32;
typedef unsigned short uint16;
typedef unsigned char uint8;

typedef signed int int32;
typedef signed short int16;
typedef signed char int8;

typedef unsigned char uchar;
typedef unsigned long int ulint;

#if (PLATFORM_TYPE == PLATFORM_WINDOWS)
#define bool BOOL
#else
typedef unsigned char BOOL;
#endif


#define UNPACK_1_BYTE_PARAMETER(ptr)  \
                (uint8)(*((uint8 *)ptr))


#define UNPACK_2_BYTE_PARAMETER(ptr)  \
                (uint16)((uint16)(*((uint8 *)ptr))) |   \
                (uint16)((((uint16)(*((uint8 *)ptr + 1))) << 8))


#define UNPACK_3_BYTE_PARAMETER(ptr)  \
                (uint32)(((uint32)(*((uint8 *)ptr))) << 16)    |   \
                (uint32)(((uint32)(*((uint8 *)ptr + 1))) << 8) |   \
                (uint32)(((uint32)(*((uint8 *)ptr + 2))))


#define UNPACK_4_BYTE_PARAMETER(ptr)  \
                (uint32)(((uint32)(*((uint8 *)ptr))) << 24)     |   \
                (uint32)(((uint32)(*((uint8 *)ptr + 1))) << 16) |   \
                (uint32)(((uint32)(*((uint8 *)ptr + 2))) << 8)  |   \
                (uint32)(((uint32)(*((uint8 *)ptr + 3))))


#endif /* _HCI_TYPES_H_ */
