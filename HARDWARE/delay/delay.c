/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/
#include "delay.h"

//��ʱ΢��  ע�⣺24bit������  ����798915us
void delay_us(u32 nus)
{
	SysTick->CTRL = 0; 					// �رն�ʱ��
	SysTick->LOAD = nus*21 - 1; // ��������ֵ
	SysTick->VAL 	= 0; 					// �������ֵ
	SysTick->CTRL = 1; 					// ������ʱ��  21MHZ  
	while ((SysTick->CTRL & 0x00010000)==0);// �ȴ�ʱ�䵽��
	SysTick->CTRL = 0; 					// �رն�ʱ��
}

//��ʱ����  ע�⣺24bit������  ����798ms
void delay_ms(u32 nms)
{
	SysTick->CTRL = 0; 					// �رն�ʱ��
	SysTick->LOAD = nms*21000 - 1; // ��������ֵ
	SysTick->VAL 	= 0; 					// �������ֵ
	SysTick->CTRL = 1; 					// ������ʱ��  21MHZ  
	while ((SysTick->CTRL & 0x00010000)==0);// �ȴ�ʱ�䵽��
	SysTick->CTRL = 0; 					// �رն�ʱ��
}
































