/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/

#include "stm32f4xx.h"
#include "MFRC522.h"
#include "delay.h"
#include "oled_iic.h"
#include "ble.h"
#include "lock.h"
#include "beep.h"
#include "key.h"
#include "passwordkey.h"
#include "usart.h"
#include "as608.h"
#include "voice.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

char Password_buffer[5]="5212#";//�洢���������
int  Password_cnt=0;					  //��Ϊ����Ľ��ռ�����
uint8_t Password_recvflag = 0;		  //��Ϊ����Ľ��ձ�־λ
u8 ensure=0;
u8 key_num;
char str[256]={0};

void MFRC522_Module(void);
void Bluetooth_Module(void);
void Key_Scan(void);
void AS608_Module(void);
void BEEP_Config(void);

//�������
int main()
{
	
	//1.Ӳ����ʼ��
	USART1_Config(9600);	
	USART2_Config(9600); 	 		//������ʼ��
	USART3_Config(57600);	 		//ָ�Ƴ�ʼ��
	LOCK_Config();						//����ʼ��
	BEEP_Config();						//��������ʼ��
	Voice_Config();			 			//������ʼ��
	KEY_Config();			 				//������ʼ��
	MFRC522_Initializtion(); 	//RFID�ĳ�ʼ��
	
	extern const u8 BMP1[];
	HZ= GB16_NUM();
	OLED_Init();			 //OLED��ʼ��
	
	OLED_Clear(); 						//����
	OLED_ShowCH(12,0,"��ŵ��������");
	
	
  while(1)
	{
		Key_Scan();							//����ģ��
		Bluetooth_Module(); 		//����ģ��
		MFRC522_Module();				//RFIDģ��
	}
}



/*********************************************MFRC522ģ��********************************************/
void MFRC522_Module(void)
{
	u8  status;
	u8  card_pydebuf[2];
	u8  card_numberbuf[10];  //���һ���ֽ���У���ֽ�
	u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	u8  card_readbuf[18];
	
	
	//Ѱ��   S50���Ŀ�����ȫ��Ψһ��  ���ڲ�����Ȧ  ���ö�ȡ����ȡ��Ƭ��Ϣ  ��Ƶ 13.56MHZ  
	status=MFRC522_Request(0x52, card_pydebuf); 
	
	if(status==0 )		//���������
	{
		
		GPIO_SetBits(GPIOA,GPIO_Pin_8);			//����
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//������
		GPIO_SetBits(GPIOF, GPIO_Pin_8);		//��������
		delay_ms(100);
		GPIO_ResetBits(GPIOF,GPIO_Pin_8);
		OLED_Clear(); 						//����
		OLED_ShowCH(12,0,"��ŵ��������");
		OLED_ShowCH(12,5,"ˢ�������ɹ�");
		status=MFRC522_Anticoll(card_numberbuf);			//����ײ����  �������ṩ�����Žϴ�Ŀ�Ƭ  ���Եõ�����  ����4�ֽ�
		
		MFRC522_SelectTag(card_numberbuf);							//ѡ��
		
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);//�鿨
		
		status=MFRC522_Read(4, card_readbuf);						//����
		
		delay_ms(500);											//��ʱ1s
		delay_ms(500);
		GPIO_SetBits(GPIOC,GPIO_Pin_13);	//�ر�����
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//����
		OLED_Clear(); 						//����
		OLED_ShowCH(12,0,"��ŵ��������");
	}
	
		
}
/*********************************************MFRC522ģ��********************************************/

/**********************************************����ģ��**********************************************/
void Bluetooth_Module(void)
{
	if( 1 == ble_recvflag )
	{
		//������ջ�����
		if(!strcmp((char *)ble_buffer,"lock_open#"))
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);		//����
			GPIO_SetBits(GPIOF, GPIO_Pin_8);		//��������
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//������
			delay_ms(100);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");
			OLED_ShowCH(12,5,"���������ɹ�");
			delay_ms(500);											//��ʱ1s
			delay_ms(500);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);	//�ر�����
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//����
			
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");

		}
		
		//����������ձ�־λ
		ble_recvflag = 0;
		
		//����������ջ�����
		memset((char *)ble_buffer,0,BLE_BUFFERSIZE);
		
		//������ռ�����
		ble_cnt = 0;
	}
}
/**********************************************����ģ��**********************************************/

/**********************************************����ģ��**********************************************/
//�����޸�����
void Change_Password(void)
{
	OLED_Clear(); 						//����
	OLED_ShowCH(12,0,"��ŵ��������");
	OLED_ShowCH(18,2,"����������");
	
	while(1)
	{
		GPIO_ResetBits(GPIOD , GPIO_Pin_6);
		GPIO_SetBits(GPIOD , GPIO_Pin_7);
		GPIO_SetBits(GPIOB , GPIO_Pin_7);
		GPIO_SetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
		{
			Password_buffer[Password_cnt++]='1';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '1');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
		{
			Password_buffer[Password_cnt++]='2';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '2');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
		{
			Password_buffer[Password_cnt++]='3';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '3');
			delay_ms(300);//��������
		}
		
		GPIO_SetBits(GPIOD , GPIO_Pin_6);
		GPIO_ResetBits(GPIOD , GPIO_Pin_7);
		GPIO_SetBits(GPIOB , GPIO_Pin_7);
		GPIO_SetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='4';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '4');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='5';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '5');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='6';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '6');
			delay_ms(300);//��������
		}
		
		GPIO_SetBits(GPIOD , GPIO_Pin_6);
		GPIO_SetBits(GPIOD , GPIO_Pin_7);
		GPIO_ResetBits(GPIOB , GPIO_Pin_7);
		GPIO_SetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='7';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '7');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='8';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '8');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='9';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '9');
			delay_ms(300);//��������
		}
		
		GPIO_SetBits(GPIOD , GPIO_Pin_6);
		GPIO_SetBits(GPIOD , GPIO_Pin_7);
		GPIO_SetBits(GPIOB , GPIO_Pin_7);
		GPIO_ResetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
		{
			Password_buffer[Password_cnt++]='0';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '0');
			delay_ms(300);//��������
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
		{
			Password_buffer[Password_cnt++]='#';
			delay_ms(300);//��������
		}
		

		
		if(Password_buffer[Password_cnt-1] == '#')
		{
			Password_cnt = 0;					//������ռ�����
			
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");
			OLED_ShowCH(12,5,"�����޸ĳɹ�");
			delay_ms(500);											//��ʱ1s
			delay_ms(500);
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");
			break;
		}
		
	}
}

//������������
void Key_Scan(void)
{
	GPIO_ResetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		key_buffer[key_cnt++]='1';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		key_buffer[key_cnt++]='2';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		key_buffer[key_cnt++]='3';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_6) )
//	{
//		key_buffer[key_cnt++]='A';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//��������
//	}

	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_ResetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='4';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='5';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='6';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6) )
//	{
//		key_buffer[key_cnt++]='B';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//��������
//	}
	
	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_ResetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='7';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='8';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='9';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOC , GPIO_Pin_9) )
//	{
//		key_buffer[key_cnt++]='C';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//��������
//	}
	
	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_ResetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
	{
//		key_buffer[key_cnt++]='*';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		Change_Password();
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
	{
		key_buffer[key_cnt++]='0';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//��������
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
	{
		key_buffer[key_cnt++]='#';
		delay_ms(300);//��������
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOC , GPIO_Pin_7) )
//	{
//		key_buffer[key_cnt++]='D';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//��������
//	}
	
	if(key_buffer[key_cnt-1] == '#')
	{
		if( !strcmp((char *)key_buffer , Password_buffer) )
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);		//����
			GPIO_SetBits(GPIOF, GPIO_Pin_8);		//��������
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//������
			delay_ms(100);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");
			OLED_ShowCH(12,5,"��������ɹ�");
			delay_ms(500);											//��ʱ1s
			delay_ms(500);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);	//�ر�����
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//����
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");
		}
		else
		{
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");
			OLED_ShowCH(12,4,"�����������");
			OLED_ShowCH(40,6,"������");
			delay_ms(500);											//��ʱ1s
			delay_ms(500);
			OLED_Clear(); 						//����
			OLED_ShowCH(12,0,"��ŵ��������");		
		}
		
		//��ս��ջ�����
		memset((char *)key_buffer,0,KEY_BUFFERSIZE);
		
		//������ռ�����
		key_cnt = 0;
	}
}



/**********************************************����ģ��**********************************************/

/**********************************************ָ��ģ��**********************************************/
void AS608_Module(void)
{
	key_num=KEY_Scan(0);	
	if(key_num)
	{
		
		if(key_num==1)
		{
			OLED_Clear();
			Add_FR();		//¼ָ��	
		}
		if(key_num==2)
		{
			OLED_Clear();
			Del_FR();		//ɾָ��
		}
	}

	press_FR();//ˢָ��			
}

/**********************************************ָ��ģ��**********************************************/
