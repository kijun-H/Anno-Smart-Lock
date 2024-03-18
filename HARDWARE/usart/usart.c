/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/

#include "usart.h"

__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; // 发送缓冲,最大USART2_MAX_SEND_LEN字节
// 串口接收缓存区
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; // 接收缓冲,最大USART2_MAX_RECV_LEN个字节.

// 通过判断接收连续2个字符之间的时间差不大于100ms来决定是不是一次连续的数据.
// 如果2个字符接收间隔超过100ms,则认为不是1次连续数据.也就是超过100ms没有接收到
// 任何数据,则表示此次接收完毕.
// 接收到的数据状态
//[15]:0,没有接收到数据;1,接收到了一批数据.
//[14:0]:接收到的数据长度
u16 USART3_RX_STA = 0;

// 定时器7中断服务程序
void TIM7_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) // 是更新中断
  {
    USART3_RX_STA |= 1 << 15;                   // 标记接收完成
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // 清除TIM7更新中断标志
    TIM_Cmd(TIM7, DISABLE);                     // 关闭TIM7
  }
}

// 通用定时器中断初始化
// 这里始终选择为APB1的2倍，而APB1为42M
// arr：自动重装值。
// psc：时钟预分频数
void TIM7_Int_Init(u16 arr, u16 psc)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // TIM7时钟使能

  // 定时器TIM7初始化
  TIM_TimeBaseStructure.TIM_Period = arr;                     // 设置在下一个更新事件装入活动的自动重装载寄存器周期的值
  TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // 设置用来作为TIMx时钟频率除数的预分频值
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 设置时钟分割:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM向上计数模式
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);             // 根据指定的参数初始化TIMx的时间基数单位

  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); // 使能指定的TIM7中断,允许更新中断

  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // 子优先级1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器
}

//串口1的初始化
void USART1_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //1.打开GPIO端口  PA9 PA10  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  //2.打开串口时钟  USART1 -- APB2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  //3.选择引脚的复用功能
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9  , GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10 , GPIO_AF_USART1);
  
  //4.配置GPIO引脚参数并初始化
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;			 			//复用模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 			//输出速度
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		 			//推挽复用
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			 			//上拉电阻
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_9;	//引脚编号
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	//5.配置USART1的参数并初始化
  USART_InitStructure.USART_BaudRate 		= baud;										//波特率
  USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;		//数据位
  USART_InitStructure.USART_StopBits 		= USART_StopBits_1;				//停止位
  USART_InitStructure.USART_Parity 			= USART_Parity_No;				//检验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //收发模式
  USART_Init(USART1, &USART_InitStructure);
  
  //6.配置中断参数并初始化
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;								//中断通道编号
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//抢占优先级
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;							//子优先级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//打开中断通道
  NVIC_Init(&NVIC_InitStructure);
  
	//7.选择中断源   接收到数据则触发中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  //8.打开USART1
  USART_Cmd(USART1, ENABLE);
}


// 串口3的初始化
void USART3_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  // 1.打开GPIO端口  PA2 PA3
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // 2.打开串口时钟  USART2 -- APB1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  USART_DeInit(USART3); // 复位串口2

  // 3.选择引脚的复用功能
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART3);

  // 4.配置GPIO引脚参数并初始化
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           // 复用模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // 输出速度
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         // 推挽复用
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           // 上拉电阻
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // 引脚编号
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // 5.配置USART2的参数并初始化
  USART_InitStructure.USART_BaudRate = baud;                                      // 波特率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // 数据位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // 停止位
  USART_InitStructure.USART_Parity = USART_Parity_No;                             // 检验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // 无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // 收发模式
  USART_Init(USART3, &USART_InitStructure);

  USART_Cmd(USART3, ENABLE);                     // 使能串口
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // 开启中断

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 抢占优先级2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // 子优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);                           // 根据指定的参数初始化VIC寄存器

  TIM7_Int_Init(1000 - 1, 8400 - 1); // 100ms中断
  USART3_RX_STA = 0;                 // 清零
  TIM_Cmd(TIM7, DISABLE);            // 关闭定时器7
}




//串口1发送字符串的函数
void USART1_SendString(char *str)
{ 
	//循环发送字符
  while( *str != '\0' )
	{
		USART_SendData(USART1,*str++);
		while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET ); //等待发送数据寄存器为空  表示发送完成		
	}
}

// 串口3,printf 函数
// 确保一次发送数据不超过USART2_MAX_SEND_LEN字节
void u3_printf(char *fmt, ...)
{
  u16 i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART3_TX_BUF, fmt, ap);
  va_end(ap);
  i = strlen((const char *)USART3_TX_BUF); // 此次发送数据的长度
  for (j = 0; j < i; j++)                  // 循环发送数据
  {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);   // 等待上次传输完成
    USART_SendData(USART3, (uint8_t)USART3_TX_BUF[j]); // 发送数据到串口2
  }
}

// 串口1的中断服务函数
void USART1_IRQHandler(void)
{
	uint8_t data = 0;
	//判断中断是否触发
	if( USART_GetITStatus(USART1, USART_IT_RXNE) == SET )
	{
		data = USART_ReceiveData(USART1); //一次只能接收1个字节
		USART_SendData(USART1,data);      //通过USART1发送1字节
	}
}

// 串口2的中断服务函数
void USART3_IRQHandler(void)
{
  u8 res;
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // 接收到数据
  {

    res = USART_ReceiveData(USART3);
    if ((USART3_RX_STA & (1 << 15)) == 0) // 接收完的一批数据,还没有被处理,则不再接收其他数据
    {
      if (USART3_RX_STA < USART3_MAX_RECV_LEN) // 还可以接收数据
      {
        TIM_SetCounter(TIM7, 0); // 计数器清空
        if (USART3_RX_STA == 0)
          TIM_Cmd(TIM7, ENABLE);              // 使能定时器7
        USART3_RX_BUF[USART3_RX_STA++] = res; // 记录接收到的值
      }
      else
      {
        USART3_RX_STA |= 1 << 15; // 强制标记接收完成
      }
    }
  }
}


