/**
  ******************************************************************************
  * File Name          : USART.c
  * Date               : 24/09/2014 18:42:49
  * Description        : This file provides code for the configuration
  *                      of the USART instances.
  ******************************************************************************
  *
  * COPYRIGHT(c) 2014 STMicroelectronics
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
#include "usart2.h"
//#include "gpio.h"

/* USER CODE BEGIN 0 */
uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA=0;       //接收状态标记	 
/* USER CODE END 0 */

UART_HandleTypeDef huart2;

/* USART2 init function */

void MX_USART2_UART_Init(void)
{
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_8;
  huart2.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart2);
}

//void HAL_UART_MspInit(UART_HandleTypeDef* huart)
//{

//  GPIO_InitTypeDef GPIO_InitStruct;
//  if(huart->Instance==USART2)
//  {
//    /* Peripheral clock enable */
//    __USART2_CLK_ENABLE();
//  
//    /**USART2 GPIO Configuration    
//    PA2     ------> USART2_TX
//    PA3     ------> USART2_RX 
//    */
//    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
//    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
//    GPIO_InitStruct.Pull = GPIO_NOPULL;
//    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
//    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
//    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		
//	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);		
//    /* Peripheral interrupt init*/
//    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
//    HAL_NVIC_EnableIRQ(USART2_IRQn);
//  }
//}

//void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
//{

//  if(huart->Instance==USART2)
//  {
//    /* Peripheral clock disable */
//    __USART2_CLK_DISABLE();
//  
//    /**USART2 GPIO Configuration    
//    PA2     ------> USART2_TX
//    PA3     ------> USART2_RX 
//    */
//    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

//    /* Peripheral interrupt Deinit*/
//    HAL_NVIC_DisableIRQ(USART2_IRQn);
//  }
//} 



#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 

}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
 
//重定义fputc函数 
int fputc(int ch, FILE *f)
{      
	while((USART2->ISR&0X40)==0);//循环发送,直到发送完毕   
    USART2->TDR = (uint8_t) ch;      
	return ch;
}
#endif 


void USART2_IRQHandler(void)                	//串口1中断服务程序
	{
		
	uint8_t Res;
     HAL_UART_IRQHandler(&huart2);
    if(__HAL_UART_GET_IT(&huart2, UART_IT_RXNE) != RESET)  //
    {
          USART2->RQR |= 0x08;//?? RXNE
          Res =USART2->RDR;//USARTx_RX_Data(&UartHandle);
        if((USART_RX_STA&0x8000)==0)//
       {
               if(USART_RX_STA&0x4000)//
             {
                     if(Res!=0x0a)USART_RX_STA=0;//
                    else USART_RX_STA|=0x8000;        // 
             }
           else //?????0X0D
           { 
                 if(Res==0x0d)USART_RX_STA|=0x4000;
                 else
                {
                       USART_RX_BUF[USART_RX_STA&0X3FFF]=Res ;
                      USART_RX_STA++;
                      if(USART_RX_STA>(USART_REC_LEN-1))USART_RX_STA=0;//         
               }         
          }
       }  
 
  } 
		HAL_NVIC_ClearPendingIRQ(USART2_IRQn);	
}	
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
