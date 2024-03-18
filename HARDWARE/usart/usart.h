#ifndef _USART_H_
#define _USART_H_

#include "stm32f4xx.h" //必须添加
#include "time.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define USART3_MAX_RECV_LEN 400 // 最大接收缓存字节数
#define USART3_MAX_SEND_LEN 400 // 最大发送缓存字节数

extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; // 接收缓冲,最大USART2_MAX_RECV_LEN字节
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; // 发送缓冲,最大USART2_MAX_SEND_LEN字节
extern u16 USART3_RX_STA;                     // 接收数据状态

// 定时器7中断服务程序
void TIM7_IRQHandler(void);

void TIM7_Int_Init(u16 arr, u16 psc);

// 串口1的初始化
void USART1_Config(u32 baud);

// 串口2的初始化
void USART3_Config(u32 baud);

//串口1发送字符串的函数
void USART1_SendString(char *str);

// 串口3发送字符串的函数
void u3_printf(char *fmt, ...);

// 串口1的中断服务函数
void USART1_IRQHandler(void);

// 串口3的中断服务函数
void USART3_IRQHandler(void);

#endif
