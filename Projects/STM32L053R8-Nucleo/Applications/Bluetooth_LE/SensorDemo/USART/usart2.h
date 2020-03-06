#ifndef __usart2_H_
#define __usart2_H_
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32l0xx_hal.h"
#include "stdio.h"
#define USART_REC_LEN  			200  	//定义最大接收字节数 200	
extern uint8_t  USART_RX_BUF[USART_REC_LEN]; //接收缓冲,最大USART_REC_LEN个字节.末字节为换行符 
extern uint16_t USART_RX_STA;         		//接收状态标记
extern UART_HandleTypeDef huart2;

void MX_USART2_UART_Init(void);

#ifdef __cplusplus
}
#endif
#endif /*__ usart_H */

