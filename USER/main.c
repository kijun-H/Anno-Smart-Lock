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

char Password_buffer[5]="5212#";//存储密码的数组
int  Password_cnt=0;					  //作为密码的接收计数器
uint8_t Password_recvflag = 0;		  //作为密码的接收标志位
u8 ensure=0;
u8 key_num;
char str[256]={0};

void MFRC522_Module(void);
void Bluetooth_Module(void);
void Key_Scan(void);
void AS608_Module(void);
void BEEP_Config(void);

//程序入口
int main()
{
	
	//1.硬件初始化
	USART1_Config(9600);	
	USART2_Config(9600); 	 		//蓝牙初始化
	USART3_Config(57600);	 		//指纹初始化
	LOCK_Config();						//锁初始化
	BEEP_Config();						//蜂鸣器初始化
	Voice_Config();			 			//语音初始化
	KEY_Config();			 				//按键初始化
	MFRC522_Initializtion(); 	//RFID的初始化
	
	extern const u8 BMP1[];
	HZ= GB16_NUM();
	OLED_Init();			 //OLED初始化
	
	OLED_Clear(); 						//清屏
	OLED_ShowCH(12,0,"安诺智能门锁");
	
	
  while(1)
	{
		Key_Scan();							//按键模块
		Bluetooth_Module(); 		//蓝牙模块
		MFRC522_Module();				//RFID模块
	}
}



/*********************************************MFRC522模块********************************************/
void MFRC522_Module(void)
{
	u8  status;
	u8  card_pydebuf[2];
	u8  card_numberbuf[10];  //最后一个字节是校验字节
	u8  card_key0Abuf[6]={0xff,0xff,0xff,0xff,0xff,0xff};
	u8  card_readbuf[18];
	
	
	//寻卡   S50卡的卡号是全球唯一的  卡内部有线圈  利用读取卡获取卡片信息  低频 13.56MHZ  
	status=MFRC522_Request(0x52, card_pydebuf); 
	
	if(status==0 )		//如果读到卡
	{
		
		GPIO_SetBits(GPIOA,GPIO_Pin_8);			//开锁
		GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//打开语音
		GPIO_SetBits(GPIOF, GPIO_Pin_8);		//蜂鸣器响
		delay_ms(100);
		GPIO_ResetBits(GPIOF,GPIO_Pin_8);
		OLED_Clear(); 						//清屏
		OLED_ShowCH(12,0,"安诺智能门锁");
		OLED_ShowCH(12,5,"刷卡解锁成功");
		status=MFRC522_Anticoll(card_numberbuf);			//防碰撞处理  把能量提供给卡号较大的卡片  可以得到卡号  卡号4字节
		
		MFRC522_SelectTag(card_numberbuf);							//选卡
		
		status=MFRC522_Auth(0x60, 4, card_key0Abuf, card_numberbuf);//验卡
		
		status=MFRC522_Read(4, card_readbuf);						//读卡
		
		delay_ms(500);											//延时1s
		delay_ms(500);
		GPIO_SetBits(GPIOC,GPIO_Pin_13);	//关闭语音
		GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//关锁
		OLED_Clear(); 						//清屏
		OLED_ShowCH(12,0,"安诺智能门锁");
	}
	
		
}
/*********************************************MFRC522模块********************************************/

/**********************************************蓝牙模块**********************************************/
void Bluetooth_Module(void)
{
	if( 1 == ble_recvflag )
	{
		//处理接收缓冲区
		if(!strcmp((char *)ble_buffer,"lock_open#"))
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);		//开锁
			GPIO_SetBits(GPIOF, GPIO_Pin_8);		//蜂鸣器响
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//打开语音
			delay_ms(100);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");
			OLED_ShowCH(12,5,"蓝牙解锁成功");
			delay_ms(500);											//延时1s
			delay_ms(500);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);	//关闭语音
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//关锁
			
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");

		}
		
		//清除蓝牙接收标志位
		ble_recvflag = 0;
		
		//清空蓝牙接收缓冲区
		memset((char *)ble_buffer,0,BLE_BUFFERSIZE);
		
		//清除接收计数器
		ble_cnt = 0;
	}
}
/**********************************************蓝牙模块**********************************************/

/**********************************************按键模块**********************************************/
//按键修改密码
void Change_Password(void)
{
	OLED_Clear(); 						//清屏
	OLED_ShowCH(12,0,"安诺智能门锁");
	OLED_ShowCH(18,2,"输入新密码");
	
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
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
		{
			Password_buffer[Password_cnt++]='2';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '2');
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
		{
			Password_buffer[Password_cnt++]='3';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '3');
			delay_ms(300);//消抖处理
		}
		
		GPIO_SetBits(GPIOD , GPIO_Pin_6);
		GPIO_ResetBits(GPIOD , GPIO_Pin_7);
		GPIO_SetBits(GPIOB , GPIO_Pin_7);
		GPIO_SetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='4';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '4');
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='5';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '5');
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='6';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '6');
			delay_ms(300);//消抖处理
		}
		
		GPIO_SetBits(GPIOD , GPIO_Pin_6);
		GPIO_SetBits(GPIOD , GPIO_Pin_7);
		GPIO_ResetBits(GPIOB , GPIO_Pin_7);
		GPIO_SetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='7';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '7');
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='8';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '8');
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
		{
			Password_buffer[Password_cnt++]='9';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '9');
			delay_ms(300);//消抖处理
		}
		
		GPIO_SetBits(GPIOD , GPIO_Pin_6);
		GPIO_SetBits(GPIOD , GPIO_Pin_7);
		GPIO_SetBits(GPIOB , GPIO_Pin_7);
		GPIO_ResetBits(GPIOA , GPIO_Pin_4);
		
		if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
		{
			Password_buffer[Password_cnt++]='0';
			OLED_ShowChar(16 + 16 * Password_cnt, 4, '0');
			delay_ms(300);//消抖处理
		}
		if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
		{
			Password_buffer[Password_cnt++]='#';
			delay_ms(300);//消抖处理
		}
		

		
		if(Password_buffer[Password_cnt-1] == '#')
		{
			Password_cnt = 0;					//清除接收计数器
			
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");
			OLED_ShowCH(12,5,"密码修改成功");
			delay_ms(500);											//延时1s
			delay_ms(500);
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");
			break;
		}
		
	}
}

//按键输入密码
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
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		key_buffer[key_cnt++]='2';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_6) )
	{
		key_buffer[key_cnt++]='3';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOE, GPIO_Pin_6) )
//	{
//		key_buffer[key_cnt++]='A';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//消抖处理
//	}

	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_ResetBits(GPIOD , GPIO_Pin_7);
	GPIO_SetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='4';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='5';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOD, GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='6';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOB, GPIO_Pin_6) )
//	{
//		key_buffer[key_cnt++]='B';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//消抖处理
//	}
	
	GPIO_SetBits(GPIOD , GPIO_Pin_6);
	GPIO_SetBits(GPIOD , GPIO_Pin_7);
	GPIO_ResetBits(GPIOB , GPIO_Pin_7);
	GPIO_SetBits(GPIOA , GPIO_Pin_4);
	
	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_11) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='7';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='8';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOB , GPIO_Pin_7) )
	{
		key_buffer[key_cnt++]='9';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOC , GPIO_Pin_9) )
//	{
//		key_buffer[key_cnt++]='C';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//消抖处理
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
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_5) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
	{
		key_buffer[key_cnt++]='0';
		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
		delay_ms(300);//消抖处理
	}
	if( !GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOA , GPIO_Pin_4) )
	{
		key_buffer[key_cnt++]='#';
		delay_ms(300);//消抖处理
	}
//	if( !GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_6) && !GPIO_ReadOutputDataBit(GPIOC , GPIO_Pin_7) )
//	{
//		key_buffer[key_cnt++]='D';
//		OLED_ShowChar(16 + 16 * key_cnt, 4, '*');
//		delay_ms(300);//消抖处理
//	}
	
	if(key_buffer[key_cnt-1] == '#')
	{
		if( !strcmp((char *)key_buffer , Password_buffer) )
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_8);		//开锁
			GPIO_SetBits(GPIOF, GPIO_Pin_8);		//蜂鸣器响
			GPIO_ResetBits(GPIOC,GPIO_Pin_13);	//打开语音
			delay_ms(100);
			GPIO_ResetBits(GPIOF,GPIO_Pin_8);
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");
			OLED_ShowCH(12,5,"密码解锁成功");
			delay_ms(500);											//延时1s
			delay_ms(500);
			GPIO_SetBits(GPIOC,GPIO_Pin_13);	//关闭语音
			GPIO_ResetBits(GPIOA,GPIO_Pin_8);		//关锁
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");
		}
		else
		{
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");
			OLED_ShowCH(12,4,"密码输入错误");
			OLED_ShowCH(40,6,"请重试");
			delay_ms(500);											//延时1s
			delay_ms(500);
			OLED_Clear(); 						//清屏
			OLED_ShowCH(12,0,"安诺智能门锁");		
		}
		
		//清空接收缓冲区
		memset((char *)key_buffer,0,KEY_BUFFERSIZE);
		
		//清除接收计数器
		key_cnt = 0;
	}
}



/**********************************************按键模块**********************************************/

/**********************************************指纹模块**********************************************/
void AS608_Module(void)
{
	key_num=KEY_Scan(0);	
	if(key_num)
	{
		
		if(key_num==1)
		{
			OLED_Clear();
			Add_FR();		//录指纹	
		}
		if(key_num==2)
		{
			OLED_Clear();
			Del_FR();		//删指纹
		}
	}

	press_FR();//刷指纹			
}

/**********************************************指纹模块**********************************************/
