/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/

#include "stm32f4xx.h"

//蜂鸣器串口初始化
void BEEP_Config(void)
{
	//1.
	GPIO_InitTypeDef  GPIO_InitStructure;
	
	//2.打开GPIO端口的时钟 GPIOF端口
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	//3.对结构体变量的成员进行赋值
	GPIO_InitStructure.GPIO_Pin 	= GPIO_Pin_8;								//引脚编号

	GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_OUT;						//输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;						//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;				//输出速率
	GPIO_InitStructure.GPIO_PuPd 	= GPIO_PuPd_NOPULL;					//无上下拉
	
	//4.对GPIO端口进行初始化
	GPIO_Init(GPIOF, &GPIO_InitStructure);
}
