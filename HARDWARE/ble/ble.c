/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/
#include "stm32f4xx.h"
#include "ble.h"
#include "delay.h"

 __IO char ble_buffer[BLE_BUFFERSIZE];  //作为蓝牙模块的接收缓冲区
 __IO int  ble_cnt=0;					  //作为蓝牙模块的接收计数器
 __IO uint8_t ble_recvflag = 0;		  //作为蓝牙模块的接收标志位



//串口2的初始化
void USART2_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //1.打开GPIO端口  PA2 PA3  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  //2.打开串口时钟  USART2 -- APB1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
  
  //3.选择引脚的复用功能
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2  , GPIO_AF_USART2);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3  , GPIO_AF_USART2);
  
  //4.配置GPIO引脚参数并初始化
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;			 			//复用模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 			//输出速度
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		 			//推挽复用
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			 			//上拉电阻
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2|GPIO_Pin_3;	//引脚编号
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	//5.配置USART1的参数并初始化
  USART_InitStructure.USART_BaudRate 		= baud;										//波特率
  USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;		//数据位
  USART_InitStructure.USART_StopBits 		= USART_StopBits_1;				//停止位
  USART_InitStructure.USART_Parity 			= USART_Parity_No;				//检验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
  USART_Init(USART2, &USART_InitStructure);
  
  //6.配置中断参数并初始化
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;								//中断通道编号
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;							//子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//打开中断通道
  NVIC_Init(&NVIC_InitStructure);
  
	//7.选择中断源   接收到数据则触发中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

  //8.打开USART2
  USART_Cmd(USART2, ENABLE);
}


void USART2_IRQHandler(void)
{
	//判断中断是否触发
	if( USART_GetITStatus(USART2, USART_IT_RXNE) == SET )
	{
		//判断接收缓冲区是否满了
		if(ble_cnt < BLE_BUFFERSIZE)
		{
			ble_buffer[ble_cnt++] = USART_ReceiveData(USART2); //一次只能接收1个字节
			
			//说明数据接收完成
			if(ble_buffer[ble_cnt-1] == '#')
			{
				ble_recvflag = 1;
			}
		}
	}
}
