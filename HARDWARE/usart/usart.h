#ifndef _USART_H_
#define _USART_H_

#include "stm32f4xx.h" //�������
#include "time.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#define USART3_MAX_RECV_LEN 400 // �����ջ����ֽ���
#define USART3_MAX_SEND_LEN 400 // ����ͻ����ֽ���

extern u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; // ���ջ���,���USART2_MAX_RECV_LEN�ֽ�
extern u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; // ���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
extern u16 USART3_RX_STA;                     // ��������״̬

// ��ʱ��7�жϷ������
void TIM7_IRQHandler(void);

void TIM7_Int_Init(u16 arr, u16 psc);

// ����1�ĳ�ʼ��
void USART1_Config(u32 baud);

// ����2�ĳ�ʼ��
void USART3_Config(u32 baud);

//����1�����ַ����ĺ���
void USART1_SendString(char *str);

// ����3�����ַ����ĺ���
void u3_printf(char *fmt, ...);

// ����1���жϷ�����
void USART1_IRQHandler(void);

// ����3���жϷ�����
void USART3_IRQHandler(void);

#endif
