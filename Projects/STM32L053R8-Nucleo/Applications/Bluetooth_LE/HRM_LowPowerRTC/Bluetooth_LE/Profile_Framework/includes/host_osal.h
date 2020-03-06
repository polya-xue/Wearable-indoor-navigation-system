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
*   FILENAME        -  host_osal.h
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
******************************************************************************
*  File Description 
*  ---------------------
* This file will have OS abstraction layer Declarations to be used by the host 
* software stack. 
******************************************************************************/
#ifndef _HOST_OSAL_H_
#define _HOST_OSAL_H_

/******************************************************************************
 * Include Files
******************************************************************************/
#include <host_config.h>
#if (PLATFORM_TYPE == PLATFORM_WINDOWS)
#include <windows.h>
#include <stdio.h>
#endif

/******************************************************************************
 * Macro Declarations
******************************************************************************/
extern void DisableInterrupts(void);
extern void EnableInterrupts(void);

#define DISABLE_INTERRUPTS()		{DisableInterrupts();}
#define ENABLE_INTERRUPTS()		{EnableInterrupts();}

/******************************************************************************
 * Type Declarations
******************************************************************************/
#if (PLATFORM_TYPE == PLATFORM_WINDOWS)
typedef HANDLE HOS_THREAD_ID_TYPE;

typedef SECURITY_ATTRIBUTES HOS_THREAD_ATTR_TYPE;

typedef HANDLE HOS_THREAD_MUTEX_TYPE;

typedef SECURITY_ATTRIBUTES HOS_THREAD_MUTEX_ATTR_TYPE;

typedef HANDLE HOS_THREAD_CONDITION_TYPE;

typedef SECURITY_ATTRIBUTES HOS_THREAD_CONDITION_ATTR_TYPE;

/*#define HOS_THREAD_CONDITION_INITIALIZER PTHREAD_COND_INITIALIZER*/

typedef LPTHREAD_START_ROUTINE HOS_THREAD_ENTRY_FUNCTION;

#define HOS_THREAD_DEFAULT_RETURN_VALUE           \
                    (DWORD)0

#else

typedef int HOS_THREAD_ID_TYPE;

typedef int HOS_THREAD_ATTR_TYPE;

typedef int HOS_THREAD_MUTEX_TYPE;

typedef int HOS_THREAD_MUTEX_ATTR_TYPE;

typedef int HOS_THREAD_CONDITION_TYPE;

typedef int HOS_THREAD_CONDITION_ATTR_TYPE;

#define HOS_THREAD_CONDITION_INITIALIZER

typedef void* (* HOS_THREAD_ENTRY_FUNCTION)(void *);

#define HOS_THREAD_DEFAULT_RETURN_VALUE    0

#endif
/******************************************************************************
 * Function Prototypes 
******************************************************************************/

/**
 * Osal_MemCpy
 * 
 * @param[in] copyTo Destination address
 * @param[in] copyFrom  Source address
 * @param[in] numBytes size in the bytes  
 *
 * This function copies numBytes number of bytes from a 
 * memory location pointed by copyFrom to a destination 
 * memory location pointed by copyTo
 * 
 */
void Osal_MemCpy(void* copyTo, const void* copyFrom,uint numBytes );

/**
 * Osal_MemCmp
 * 
 * @param[in] src1  	Address of source #1
 * @param[in] src2  	Address of source #2
 * @param[in] numBytes  Number of bytes to compare  
 *
 * This function compares the first numBytes
 * of memory specified by the source
 * address src1 and src2
 * 
 * @return  Zero, if both the memory locations are equal 
 *          A value greater than zero if first location is
 *          greater than the second location and a negative
 *          value if first location is lesser than the second 
 *          location
 */
int Osal_MemCmp(void* src1,const void* src2,uint numBytes);

/**
 * Osal_MemSet
 * 
 * @param[in] pMemAddr    Destination address
 * @param[in] value  	  Value to be set
 * @param[in] count       Size in the bytes  
 *
 * This function sets first count number of bytes in 
 * destination memory pointed by ptr 
 * to the specified value 
 */
void Osal_MemSet(void* pMemAddr,int value,uint count);

/**
 * Osal_Thread_Create
 * 
 * creates a thread. The parameters correspond to
 * the parameters required to create a thread in
 * windows
 */ 
int Osal_Thread_Create(HOS_THREAD_ID_TYPE * thread,
					   const HOS_THREAD_ATTR_TYPE * attr,
					   HOS_THREAD_ENTRY_FUNCTION start_routine, 
					   void * arg);

/**
 * Osal_Mutex_Init
 * 
 * initializes a mutex
 */ 
int Osal_Mutex_Init ( HOS_THREAD_MUTEX_TYPE * mutex, 
                       HOS_THREAD_MUTEX_ATTR_TYPE * mutex_attribute);

/**
 * Osal_Mutex_Lock
 * 
 * locks a mutex
 */ 
int Osal_Mutex_Lock (HOS_THREAD_MUTEX_TYPE * mutex);

/**
 * Osal_Mutex_Unlock
 * 
 * unlocks a mutex
 */ 
int Osal_Mutex_Unlock (HOS_THREAD_MUTEX_TYPE * mutex);

/**
 * Osal_Sleep
 * 
 * @param[in] sleep_time : sleep time in milliseconds
 * 
 * calls the platform specific function to add a delay
 */ 
void Osal_Sleep (uint sleep_time);

/**
 * Osal_Get_Cur_Time
 * 
 * returns the current time in milliseconds
 */
ulint Osal_Get_Cur_Time(void);
 
#endif /* _HOST_OSAL_H_ */
