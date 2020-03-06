#ifndef __Ultra_H
#define __Ultra_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stdio.h"
	 
/* Private function prototypes -----------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void Ultra_GPIO_Init(void);
void MX_TIM6_Init(void);

/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/
void Set_Echo_Disable_Interrupt(void);
void Set_Echo1_Rising_Interrupt(void);
void Set_Echo1_Falling_Interrupt(void);
void Set_Echo2_Rising_Interrupt(void);
void Set_Echo2_Falling_Interrupt(void);
void Set_Echo3_Rising_Interrupt(void);
void Set_Echo3_Falling_Interrupt(void);
void Trig1_Enable(void);
void Trig2_Enable(void);
void Trig3_Enable(void);

#ifdef __cplusplus
}
#endif
#endif 
