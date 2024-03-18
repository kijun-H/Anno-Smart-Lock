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

//按键初始化函数
void KEY_Init(void) //IO初始化
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




//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下 
//4，KEY3按下
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!
u8 KEY_Scan(u8 mode)
{	 
	static u8 key_up=1;//按键按松开标志
	
	if(mode)
		key_up=1;  //支持连按	

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
 	return 0;// 无按键按下
}
