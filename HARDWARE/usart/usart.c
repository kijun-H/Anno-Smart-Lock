/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/

#include "usart.h"

__align(8) u8 USART3_TX_BUF[USART3_MAX_SEND_LEN]; // ���ͻ���,���USART2_MAX_SEND_LEN�ֽ�
// ���ڽ��ջ�����
u8 USART3_RX_BUF[USART3_MAX_RECV_LEN]; // ���ջ���,���USART2_MAX_RECV_LEN���ֽ�.

// ͨ���жϽ�������2���ַ�֮���ʱ������100ms�������ǲ���һ������������.
// ���2���ַ����ռ������100ms,����Ϊ����1����������.Ҳ���ǳ���100msû�н��յ�
// �κ�����,���ʾ�˴ν������.
// ���յ�������״̬
//[15]:0,û�н��յ�����;1,���յ���һ������.
//[14:0]:���յ������ݳ���
u16 USART3_RX_STA = 0;

// ��ʱ��7�жϷ������
void TIM7_IRQHandler(void)
{
  if (TIM_GetITStatus(TIM7, TIM_IT_Update) != RESET) // �Ǹ����ж�
  {
    USART3_RX_STA |= 1 << 15;                   // ��ǽ������
    TIM_ClearITPendingBit(TIM7, TIM_IT_Update); // ���TIM7�����жϱ�־
    TIM_Cmd(TIM7, DISABLE);                     // �ر�TIM7
  }
}

// ͨ�ö�ʱ���жϳ�ʼ��
// ����ʼ��ѡ��ΪAPB1��2������APB1Ϊ42M
// arr���Զ���װֵ��
// psc��ʱ��Ԥ��Ƶ��
void TIM7_Int_Init(u16 arr, u16 psc)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM7, ENABLE); // TIM7ʱ��ʹ��

  // ��ʱ��TIM7��ʼ��
  TIM_TimeBaseStructure.TIM_Period = arr;                     // ��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ
  TIM_TimeBaseStructure.TIM_Prescaler = psc;                  // ����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ����ʱ�ӷָ�:TDTS = Tck_tim
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // TIM���ϼ���ģʽ
  TIM_TimeBaseInit(TIM7, &TIM_TimeBaseStructure);             // ����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ

  TIM_ITConfig(TIM7, TIM_IT_Update, ENABLE); // ʹ��ָ����TIM7�ж�,��������ж�

  NVIC_InitStructure.NVIC_IRQChannel = TIM7_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;        // �����ȼ�1
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���
}

//����1�ĳ�ʼ��
void USART1_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;
  
  //1.��GPIO�˿�  PA9 PA10  
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
  
  //2.�򿪴���ʱ��  USART1 -- APB2
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
  
  //3.ѡ�����ŵĸ��ù���
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9  , GPIO_AF_USART1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10 , GPIO_AF_USART1);
  
  //4.����GPIO���Ų�������ʼ��
  GPIO_InitStructure.GPIO_Mode 	= GPIO_Mode_AF;			 			//����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 			//����ٶ�
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		 			//���츴��
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			 			//��������
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10|GPIO_Pin_9;	//���ű��
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
	//5.����USART1�Ĳ�������ʼ��
  USART_InitStructure.USART_BaudRate 		= baud;										//������
  USART_InitStructure.USART_WordLength 	= USART_WordLength_8b;		//����λ
  USART_InitStructure.USART_StopBits 		= USART_StopBits_1;				//ֹͣλ
  USART_InitStructure.USART_Parity 			= USART_Parity_No;				//����λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	//��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx; //�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure);
  
  //6.�����жϲ�������ʼ��
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;								//�ж�ͨ�����
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;				//��ռ���ȼ�
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;							//�����ȼ�
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;									//���ж�ͨ��
  NVIC_Init(&NVIC_InitStructure);
  
	//7.ѡ���ж�Դ   ���յ������򴥷��ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);

  //8.��USART1
  USART_Cmd(USART1, ENABLE);
}


// ����3�ĳ�ʼ��
void USART3_Config(u32 baud)
{
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  // 1.��GPIO�˿�  PA2 PA3
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

  // 2.�򿪴���ʱ��  USART2 -- APB1
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

  USART_DeInit(USART3); // ��λ����2

  // 3.ѡ�����ŵĸ��ù���
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART3);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART3);

  // 4.����GPIO���Ų�������ʼ��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;           // ����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;      // ����ٶ�
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         // ���츴��
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;           // ��������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; // ���ű��
  GPIO_Init(GPIOA, &GPIO_InitStructure);

  // 5.����USART2�Ĳ�������ʼ��
  USART_InitStructure.USART_BaudRate = baud;                                      // ������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;                     // ����λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;                          // ֹͣλ
  USART_InitStructure.USART_Parity = USART_Parity_No;                             // ����λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None; // ��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;                 // �շ�ģʽ
  USART_Init(USART3, &USART_InitStructure);

  USART_Cmd(USART3, ENABLE);                     // ʹ�ܴ���
  USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); // �����ж�

  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ��ռ���ȼ�2
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;        // �����ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);                           // ����ָ���Ĳ�����ʼ��VIC�Ĵ���

  TIM7_Int_Init(1000 - 1, 8400 - 1); // 100ms�ж�
  USART3_RX_STA = 0;                 // ����
  TIM_Cmd(TIM7, DISABLE);            // �رն�ʱ��7
}




//����1�����ַ����ĺ���
void USART1_SendString(char *str)
{ 
	//ѭ�������ַ�
  while( *str != '\0' )
	{
		USART_SendData(USART1,*str++);
		while( USART_GetFlagStatus(USART1,USART_FLAG_TXE) == RESET ); //�ȴ��������ݼĴ���Ϊ��  ��ʾ�������		
	}
}

// ����3,printf ����
// ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
void u3_printf(char *fmt, ...)
{
  u16 i, j;
  va_list ap;
  va_start(ap, fmt);
  vsprintf((char *)USART3_TX_BUF, fmt, ap);
  va_end(ap);
  i = strlen((const char *)USART3_TX_BUF); // �˴η������ݵĳ���
  for (j = 0; j < i; j++)                  // ѭ����������
  {
    while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);   // �ȴ��ϴδ������
    USART_SendData(USART3, (uint8_t)USART3_TX_BUF[j]); // �������ݵ�����2
  }
}

// ����1���жϷ�����
void USART1_IRQHandler(void)
{
	uint8_t data = 0;
	//�ж��ж��Ƿ񴥷�
	if( USART_GetITStatus(USART1, USART_IT_RXNE) == SET )
	{
		data = USART_ReceiveData(USART1); //һ��ֻ�ܽ���1���ֽ�
		USART_SendData(USART1,data);      //ͨ��USART1����1�ֽ�
	}
}

// ����2���жϷ�����
void USART3_IRQHandler(void)
{
  u8 res;
  if (USART_GetITStatus(USART3, USART_IT_RXNE) != RESET) // ���յ�����
  {

    res = USART_ReceiveData(USART3);
    if ((USART3_RX_STA & (1 << 15)) == 0) // �������һ������,��û�б�����,���ٽ�����������
    {
      if (USART3_RX_STA < USART3_MAX_RECV_LEN) // �����Խ�������
      {
        TIM_SetCounter(TIM7, 0); // ���������
        if (USART3_RX_STA == 0)
          TIM_Cmd(TIM7, ENABLE);              // ʹ�ܶ�ʱ��7
        USART3_RX_BUF[USART3_RX_STA++] = res; // ��¼���յ���ֵ
      }
      else
      {
        USART3_RX_STA |= 1 << 15; // ǿ�Ʊ�ǽ������
      }
    }
  }
}


