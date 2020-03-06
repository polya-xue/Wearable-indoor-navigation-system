/**
  ******************************************************************************
  * File Name          : main.c
  * Description        : Main program body
  ******************************************************************************
  *
  * COPYRIGHT(c) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "Ultra.h"
#include "usart2.h"
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */
#define  ULTRASONIC1_PORT        GPIOB
#define  ULTRASONIC1_PIN         GPIO_PIN_1
#define  ULTRASONIC2_PORT        GPIOB
#define  ULTRASONIC2_PIN         GPIO_PIN_4
#define  ULTRASONIC3_PORT        GPIOB
#define  ULTRASONIC3_PIN         GPIO_PIN_5

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim6;
/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
uint8_t  meter_state1=0,meter_state2=0,meter_state3=0;
uint16_t Ultra_Dis1,Ultra_Dis2,Ultra_Dis3;
uint8_t Ultra_Flag;

/* USER CODE BEGIN 0 */

void Trig_Delay()		//
{
	uint8_t i=50;
	do
	{
	} while (--i);
}

void Trig1_Enable(void)
{
	HAL_GPIO_WritePin(ULTRASONIC1_PORT,ULTRASONIC1_PIN,GPIO_PIN_SET);//产生高电平
	Trig_Delay();
	HAL_GPIO_WritePin(ULTRASONIC1_PORT,ULTRASONIC1_PIN,GPIO_PIN_RESET);//产生低电平
}
void Trig2_Enable(void)
{
	HAL_GPIO_WritePin(ULTRASONIC2_PORT,ULTRASONIC2_PIN,GPIO_PIN_SET);//产生高电平
	Trig_Delay();
	HAL_GPIO_WritePin(ULTRASONIC2_PORT,ULTRASONIC2_PIN,GPIO_PIN_RESET);//产生低电平
}
void Trig3_Enable(void)
{
	HAL_GPIO_WritePin(ULTRASONIC3_PORT,ULTRASONIC3_PIN,GPIO_PIN_SET);//产生高电平
  Trig_Delay();
	HAL_GPIO_WritePin(ULTRASONIC3_PORT,ULTRASONIC3_PIN,GPIO_PIN_RESET);//产生低电平	
}
/* TIM6 init function */
void MX_TIM6_Init(void)
{
  TIM_MasterConfigTypeDef sMasterConfig;

  htim6.Instance = TIM6;
  htim6.Init.Prescaler = 31;
  htim6.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim6.Init.Period = 0;
  HAL_TIM_Base_Init(&htim6);

  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  HAL_TIMEx_MasterConfigSynchronization(&htim6, &sMasterConfig);
}
/** Configure pins as 
        * Analog 
        * Input 
        * Output
        * EVENT_OUT
        * EXTI
*/
void Ultra_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;
  /* GPIO Ports Clock Enable */
//  __GPIOC_CLK_ENABLE();
//  __GPIOH_CLK_ENABLE();
//  __GPIOA_CLK_ENABLE();
//  __GPIOB_CLK_ENABLE();
  /* EXTI interrupt init*/
	HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);	
	
  //超声波一
  /*Configure GPIO pin : PA10-ultra sonic echo1,PA9-ultra sonic echo2,PA8-ultra sonic echo3 */
  GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_9|GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  /*Configure GPIO pin : PB1-ultra sonic trig1,PB4-ultra sonic trig2,PB5-ultra sonic trig3 */
  GPIO_InitStruct.Pin = GPIO_PIN_1|GPIO_PIN_4|GPIO_PIN_5;//|GPIO_PIN_5|GPIO_PIN_1;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	HAL_GPIO_WritePin(ULTRASONIC1_PORT,ULTRASONIC1_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ULTRASONIC2_PORT,ULTRASONIC2_PIN,GPIO_PIN_RESET);
	HAL_GPIO_WritePin(ULTRASONIC3_PORT,ULTRASONIC3_PIN,GPIO_PIN_RESET);
}

/*
* 函数名称: Set_Echo_Disable_Interrupt(void)
* 功能描述: 关闭中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo_Disable_Interrupt(void)
{
	HAL_NVIC_DisableIRQ(EXTI4_15_IRQn);
}

/*
* 函数名称: Set_Echo1_Falling_Interrupt(void)
* 功能描述: 开启上升沿中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo1_Rising_Interrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
/*
* 函数名称: Set_Echo1_Falling_Interrupt(void)
* 功能描述: 开启上升沿中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo2_Rising_Interrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_9;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
/*
* 函数名称: Set_Echo1_Falling_Interrupt(void)
* 功能描述: 开启上升沿中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo3_Rising_Interrupt(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_PIN_11;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);
}
/*
* 函数名称: Set_Echo_Falling_Interrupt(void)
* 功能描述: 开启下降沿中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo1_Falling_Interrupt(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.Pin = GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	  /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);	
}
/*
* 函数名称: Set_Echo_Falling_Interrupt(void)
* 功能描述: 开启下降沿中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo2_Falling_Interrupt(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.Pin = GPIO_PIN_9;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	  /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);	
}
/*
* 函数名称: Set_Echo_Falling_Interrupt(void)
* 功能描述: 开启下降沿中断
* 输入参数: 无
* 返回参数: 无
*/
void Set_Echo3_Falling_Interrupt(void)
{
	  GPIO_InitTypeDef GPIO_InitStruct;
	  GPIO_InitStruct.Pin = GPIO_PIN_11;
    GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);	
	  /* EXTI interrupt init*/
    HAL_NVIC_SetPriority(EXTI4_15_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(EXTI4_15_IRQn);	
}
/*
* 函数名称: HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
* 功能描述: 外部中断处理函数
* 输入参数: uint16_t GPIO_Pin 
* 返回参数: 无
*/
//void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
//{
//	if(GPIO_Pin == GPIO_PIN_10)				//echo1中断
//	{
//		if(meter_state1==0)
//		{
//			meter_state1=1;
//			__HAL_TIM_SetCounter(&htim6, 0);
//			HAL_TIM_Base_Start_IT(&htim6);         
//      Set_Echo_Disable_Interrupt();		
//			Set_Echo1_Falling_Interrupt();				
//		}
//    else
//		{
//			meter_state1=0;
//			HAL_TIM_Base_Stop_IT(&htim6);
//			Ultra_Dis1=__HAL_TIM_GetCounter(&htim6);
//			if(Ultra_Dis1<800)
//					Ultra_Flag1=1;
//			else 
//				  Ultra_Flag1=0;
//			Set_Echo_Disable_Interrupt();
//			Set_Echo1_Rising_Interrupt();
//		}			
//	}
//	if(GPIO_Pin == GPIO_PIN_9)				//echo2中断
//	{
//		if(meter_state2==0)
//		{
//			meter_state2=1;
//			__HAL_TIM_SetCounter(&htim6, 0);
//			HAL_TIM_Base_Start_IT(&htim6);         
//      Set_Echo_Disable_Interrupt();		
//			Set_Echo2_Falling_Interrupt();				
//		}
//    else
//		{
//			meter_state2=0;
//			HAL_TIM_Base_Stop_IT(&htim6);
//			Ultra_Dis2=__HAL_TIM_GetCounter(&htim6);
//			if(Ultra_Dis2<800)
//					Ultra_Flag2=1;
//			else 
//				  Ultra_Flag2=0;
//			Set_Echo_Disable_Interrupt();
//			Set_Echo2_Rising_Interrupt();
//		}			
//	}
//	if(GPIO_Pin == GPIO_PIN_8)				//echo3中断
//	{
//		if(meter_state3==0)
//		{
//			meter_state3=1;
//			__HAL_TIM_SetCounter(&htim6, 0);
//			HAL_TIM_Base_Start_IT(&htim6);         
//      Set_Echo_Disable_Interrupt();		
//			Set_Echo3_Falling_Interrupt();				
//		}
//    else
//		{
//			meter_state3=0;
//			HAL_TIM_Base_Stop_IT(&htim6);
//			Ultra_Dis3=__HAL_TIM_GetCounter(&htim6);
//			if(Ultra_Dis3<800)
//					Ultra_Flag3=1;
//			else 
//				  Ultra_Flag3=0;
//			Set_Echo_Disable_Interrupt();
//			Set_Echo3_Rising_Interrupt();
//		}			
//	}
//}

/* USER CODE END 4 */

/*
* 函数名称: HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
* 功能描述: 定时器中断程序
* 输入参数: TIM_HandleTypeDef *htim
* 返回参数: 无
*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance == TIM6)
	{
		__HAL_TIM_SetCounter(&htim6, 0) ;
		__HAL_TIM_SetAutoreload(&htim6,50000);
    //printf("Ultra Flag1!");		
	}
}


#ifdef USE_FULL_ASSERT

/**
   * @brief Reports the name of the source file and the source line number
   * where the assert_param meter_error has occurred.
   * @param file: pointer to the source file name
   * @param line: assert_param meter_error line source number
   * @retval None
   */
void assert_failed(uint8_t* file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
    ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */

}

#endif

/**
  * @}
  */ 

/**
  * @}
*/ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
