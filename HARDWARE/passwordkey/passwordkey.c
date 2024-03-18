/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/
#include "stm32f4xx.h"
#include "passwordkey.h"

__IO char key_buffer[KEY_BUFFERSIZE];    //作为密码的接收缓冲区
__IO int  key_cnt=0;					  //作为密码的接收计数器
__IO uint8_t key_recvflag = 0;		  //作为密码的接收标志位

void KEY_Config(void)
{
	//1.定义和GPIO外设相关的初始化结构体变量
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//2.打开GPIO端口的时钟 GPIOB , GPIOC , GPIOE端口 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
	
	//3.对结构体变量成员的赋值
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 ; //按键引脚编号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT; //输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //输出速率
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;	//按键引脚编号
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;	//按键引脚编号
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //按键引脚编号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;	//按键引脚编号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;	//按键引脚编号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;	//按键引脚编号
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; //输入模式
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOG, &GPIO_InitStructure);
	
}

