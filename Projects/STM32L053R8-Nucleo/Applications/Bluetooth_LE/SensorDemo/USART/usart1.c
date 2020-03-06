#include "usart1.h"
#include "gpio.h"

/* USER CODE BEGIN 0 */
uint8_t USART_RX_BUF[USART_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
//接收状态
//bit15，	接收完成标志
//bit14，	接收到0x0d
//bit13~0，	接收到的有效字节数目
uint16_t USART_RX_STA=0;       //接收状态标记	 
/* USER CODE END 0 */

UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;
/* USART1 init function */

void MX_USART1_UART_Init(void)
{
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_8;
  huart1.Init.OneBitSampling = UART_ONEBIT_SAMPLING_DISABLED;
  huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  HAL_UART_Init(&huart1);
}

void HAL_UART_MspInit(UART_HandleTypeDef* huart)
{

  GPIO_InitTypeDef GPIO_InitStruct;
  if(huart->Instance==USART1)
  {
    /* Peripheral clock enable */
    __USART1_CLK_ENABLE();
  
    /**USART1 GPIO Configuration    
    PA9     ------> USART1-TX
    PA10     ------> USART1-RX
    */
    GPIO_InitStruct.Pin = GPIO_PIN_9|GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART1;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		
	  __HAL_UART_ENABLE_IT(&huart1,UART_IT_RXNE);		
    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
  }
  if(huart->Instance==USART2)
  {
    /* Peripheral clock enable */
    __USART2_CLK_ENABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    GPIO_InitStruct.Pin = GPIO_PIN_2|GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);		
	__HAL_UART_ENABLE_IT(&huart2,UART_IT_RXNE);		
    /* Peripheral interrupt init*/
    HAL_NVIC_SetPriority(USART2_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART2_IRQn);
  }
}

void HAL_UART_MspDeInit(UART_HandleTypeDef* huart)
{

  if(huart->Instance==USART1)
  {
    /* Peripheral clock disable */
    __USART1_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART1_TX
    PA3     ------> USART1_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_9|GPIO_PIN_10);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART1_IRQn);
  }
	if(huart->Instance==USART2)
  {
    /* Peripheral clock disable */
    __USART2_CLK_DISABLE();
  
    /**USART2 GPIO Configuration    
    PA2     ------> USART2_TX
    PA3     ------> USART2_RX 
    */
    HAL_GPIO_DeInit(GPIOA, GPIO_PIN_2|GPIO_PIN_3);

    /* Peripheral interrupt Deinit*/
    HAL_NVIC_DisableIRQ(USART2_IRQn);
  }
} 



//#if 1
//#pragma import(__use_no_semihosting)             
////标准库需要的支持函数                 
//struct __FILE 
//{ 
//	int handle; 

//}; 

//FILE __stdout;       
////定义_sys_exit()以避免使用半主机模式    
//void _sys_exit(int x) 
//{ 
//	x = x; 
//} 
// 
////重定义fputc函数 
//int fputc(int ch, FILE *f)
//{      
//	while((USART1->ISR&0X40)==0);//循环发送,直到发送完毕   
//    USART1->TDR = (uint8_t) ch;      
//	return ch;
//}
//#endif 


void USART1_IRQHandler(void)                	//串口1中断服务程序
	{
		
	uint8_t Res;
     HAL_UART_IRQHandler(&huart1);
    if(__HAL_UART_GET_IT(&huart1, UART_IT_RXNE) != RESET)  //
    {
          USART1->RQR |= 0x08;//?? RXNE
          Res =USART1->RDR;//USARTx_RX_Data(&UartHandle);
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
		HAL_NVIC_ClearPendingIRQ(USART1_IRQn);	
}	
/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
