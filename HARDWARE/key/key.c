/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/
#include "stm32f4xx.h"
#include "key.h"
//#include "sys.h" 
#include "delay.h"
#include "stm32f4xx.h"

//������ʼ������
void KEY_Init(void) //IO��ʼ��
{ 
	//1.�����GPIO������صĳ�ʼ���ṹ�����
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//2.��GPIO�˿ڵ�ʱ�� GPIOB , GPIOC , GPIOE�˿� 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//3.�Խṹ�������Ա�ĸ�ֵ
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ; //�������ű��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//�������ű��
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//�������ű��
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //�������ű��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//�������ű��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//�������ű��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//�������ű��
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //����ģʽ
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //����
	GPIO_Init(GPIOG, &GPIO_InitStructure);
}




//����������
//���ذ���ֵ
//mode:0,��֧��������;1,֧��������;
//0��û���κΰ�������
//1��KEY0����
//2��KEY1����
//3��KEY2���� 
//4��KEY3����
//ע��˺�������Ӧ���ȼ�,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//�������ɿ���־
	
	if(mode)
		key_up=1;  //֧������	

	GPIO_ResetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		delay_ms(300);
		key_up = 0; 
		return 1;//A
	}
	
	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_ResetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		delay_ms(300);
		key_up = 0; 
		return 2;//B
	}
	
	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_ResetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		delay_ms(300);
		key_up = 0; 
		return 3;//C
	}
	
	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_ResetBits(GPIOA , GPIO_Pin_4);
	
	
	if( !GPIO_ReadInputDataBit(GPIOG, GPIO_Pin_9) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
	{
		delay_ms(300);
		key_up = 0; 
		return 4;//D
	}
	
	
	key_up=1; 	    
 	return 0;// �ް�������
}
