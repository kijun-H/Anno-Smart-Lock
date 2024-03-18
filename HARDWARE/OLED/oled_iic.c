/***********************************************************************************
* @file    main.c 
* @author  kijun
* @version V1
* @date    2024/03/14
* @brief   
***********************************************************************************/
#include "oled_iic.h"
#include "delay.h"
#include "oledfont.h"
#include "string.h"

unsigned int HZ=0;
//����GB16�ֿ��ﺺ�ָ���
unsigned int GB16_NUM(void)
{
  unsigned int HZ_NUM;
  unsigned char *PT;
  PT = hz_index;
  while(*PT++ != '\0')
  {
  	 HZ_NUM++;
  }

  return HZ_NUM/2;
} 

void IIC_GPIO_Config(void)
{
 	//��������  SDA--PC6  SCL--PC8
	GPIO_InitTypeDef GPIO_InitStructure;
		
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			 	//���ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 			//����ٶ�
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		 		//���츴��
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			 	//��������

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;					//���ű��
	GPIO_Init(GPIOC, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;					//���ű��
	GPIO_Init(GPIOC, &GPIO_InitStructure);
}



//SDA����Ϊ���ģʽ
static void IIC_SDASetOutputMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;			 	//���ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; 			//����ٶ�
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;		 		//���츴��
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;				//��������
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;					//���ű��
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

//SDA����Ϊ����ģʽ
static void IIC_SDASetInputMode(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOG, ENABLE);

	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;			 			//���ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;			 			//��������

	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;							//���ű��
	GPIO_Init(GPIOC, &GPIO_InitStructure);

}

/*
*********************************************************************************************************
*	�� �� ��: IIC_Start
*	����˵��: CPU����IIC���������ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Start(void)
{
	//1.SDA��������Ϊ���ģʽ
	IIC_SDASetOutputMode();
	
	//2.ȷ��SDA��SCLΪ����״̬  �ߵ�ƽ
	IIC_SDA(1);
	IIC_SCL(1);
	delay_us(5);
	
	//3.��SCL�ߵ�ƽ�ڼ䣬��SDA��ƽ����
	IIC_SDA(0);
	delay_us(5);
	
	//4.��SCLʱ���ߵĵ�ƽ����
	IIC_SCL(0);
	delay_us(5);
}

/*
*********************************************************************************************************
*	�� �� ��: IIC_Start
*	����˵��: CPU����IIC����ֹͣ�ź�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void IIC_Stop(void)
{
	//1.SDA��������Ϊ���ģʽ
	IIC_SDASetOutputMode();
	
	//2.ȷ��SDA��SCL������͵�ƽ
	IIC_SDA(0);
	IIC_SCL(0);
	delay_us(5);
	
	//3.��SCLʱ���ߵĵ�ƽ����
	IIC_SCL(1);
	delay_us(5);
	
	//4.��SDAʱ���ߵĵ�ƽ����
	IIC_SDA(1);
	delay_us(5);
}

/*
*********************************************************************************************************
*	�� �� ��: IIC_WaitAck
*	����˵��: CPU����һ��ʱ�ӣ�����ȡ������ACKӦ���ź�
*	��    �Σ���
*	�� �� ֵ: ����0��ʾ��ȷӦ��1��ʾ��������Ӧ
*********************************************************************************************************
*/
bool IIC_WaitAck(void)
{
	bool status; //�洢Ӧ��״̬
	
	//1.����SDA������Ϊ����ģʽ
	IIC_SDASetInputMode();
	
	//2.��SCLʱ���ߵĵ�ƽ���ͣ���ʱ�ӻ�����׼��
	IIC_SCL(0);
	delay_us(5);
	
	//3.��SCLʱ���ߵĵ�ƽ���ߣ���ʱ�������Զ�ȡ
	IIC_SCL(1);
	delay_us(5);
	
	//4.��������ȡSDA�����ߵĵ�ƽ״̬ �͵�ƽ��ʾӦ��
	if( IIC_READ )
		status = false;
	else
		status = true;
	
	delay_us(5);
	
	//5.��SCLʱ���ߵĵ�ƽ����
	IIC_SCL(0);
	delay_us(5);
	
	return status;
}

//��IIC����д����
void Write_IIC_Byte(uint8_t data)
{
	uint8_t i = 0;
	
	//1.SDA��������Ϊ���ģʽ
	IIC_SDASetOutputMode();

	//2.��SDA�����ߺ�SCLʱ���ߵ�ƽ����
	IIC_SDA(0);
	IIC_SCL(0);
	delay_us(5);
	
	//3.ѭ������8bit����
	for(i=0;i<8;i++)
	{
		//4.�������������ݵ����λ MSB     ��λ�ȳ�  
		if( data & (1<<(7-i)) )
			IIC_SDA(1);
		else
			IIC_SDA(0);

		delay_us(5);
		
		//5.��SCLʱ���ߵĵ�ƽ���ߣ���ʱ�ӻ����Զ�ȡ
		IIC_SCL(1);
		delay_us(5);
		
		//6.��SCLʱ���ߵĵ�ƽ���ͣ���ʱ��������׼��
		IIC_SCL(0);
		delay_us(5);
	}
}

//д����
void Write_IIC_Command(u8 IIC_Command)
{
	IIC_Start();
	Write_IIC_Byte(0x78);//OLED��ַ
	IIC_WaitAck();
	Write_IIC_Byte(0x00);//�Ĵ�����ַ
	IIC_WaitAck();
	Write_IIC_Byte(IIC_Command);
	IIC_WaitAck();
	IIC_Stop();
}

//д����
void Write_IIC_Data(u8 IIC_Data)
{
	IIC_Start();
	Write_IIC_Byte(0x78);//OLED��ַ
	IIC_WaitAck();
	Write_IIC_Byte(0x40);//�Ĵ�����ַ
	IIC_WaitAck();
	Write_IIC_Byte(IIC_Data);
	IIC_WaitAck();
	IIC_Stop();
}

//OLEDȫ�����
void OLED_Fill(u8 fill_Data)
{
	u8 m,n;
	for(m=0;m<8;m++)
	{
		Write_IIC_Command(0xb0+m);		//page0-page1
		Write_IIC_Command(0x00);		//low column start address
		Write_IIC_Command(0x10);		//high column start address
		for(n=0;n<130;n++)
		{
			Write_IIC_Data(fill_Data);
		}
	}
}

//������ʼ������
void OLED_Set_Pos(u8 x, u8 y) 
{ 
	Write_IIC_Command(0xb0+y);
	Write_IIC_Command((((x+2)&0xf0)>>4)|0x10);
	Write_IIC_Command(((x+2)&0x0f)|0x01);
}

//��OLED�������л���
void OLED_ON(void)
{
	Write_IIC_Command(0X8D);  //���õ�ɱ�
	Write_IIC_Command(0X14);  //������ɱ�
	Write_IIC_Command(0XAF);  //OLED����
}

//��OLED���� -- ����ģʽ��,OLED���Ĳ���10uA
void OLED_OFF(void)
{
	Write_IIC_Command(0X8D);  //���õ�ɱ�
	Write_IIC_Command(0X10);  //�رյ�ɱ�
	Write_IIC_Command(0XAE);  //OLED����
}

//����
void OLED_Clear(void)
{
	OLED_Fill(0x00);
}

//��ָ��λ����ʾһ���ַ�,���������ַ�
//x:0~127
//y:0~63
void OLED_ShowChar(u8 x, u8 y, u8 chr)
{
  u8 c = 0, i = 0;
  c = chr - ' '; //�õ�ƫ�ƺ��ֵ
  if(x > 130 - 1)
  {
    x = 0;
    y = y + 2;
  }
	OLED_Set_Pos(x, y);
	for(i = 0; i < 8; i++)
		Write_IIC_Data(zf[c * 16 + i]);
	OLED_Set_Pos(x, y + 1);
	for(i = 0; i < 8; i++)
		Write_IIC_Data(zf[c * 16 + i + 8]);

}
//m^n����
u32 oled_pow(u8 m, u8 n)
{
  u32 result = 1;
  while(n--)result *= m;
  return result;
}
//��ʾ������
//x,y :�������
//len :���ֵ�λ��
//num:��ֵ(0~4294967295);
//mode:   Ϊ1:��ʾ0   Ϊ0:��ʾ�ո�
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 mode)
{
  u8 t, temp;
  for(t = 0; t < len; t++)
  {
    temp = (num / oled_pow(10, len - t - 1)) % 10;
		if(temp == 0)
		{
			if(mode)
				OLED_ShowChar(x + 8*t, y, '0');
			else
				OLED_ShowChar(x + 8*t, y, ' ');
			continue;
		}
		else
			OLED_ShowChar(x + 8*t, y, temp + '0');
  }
}

//��ʾ��Ӣ���ַ�
void OLED_ShowCH(u8 x, u8 y,u8 *chs)
{
  u32 i=0;
	u32 j;
	char* m;
	while (*chs != '\0')
	{
		if (*chs > 0xa0)				//�������붼�Ǵ���0xa0
		{
			for (i=0 ;i < HZ;i++)
			{	
				if(x>112)
				{
					x=0;
					y=y+2;
				}
				if ((*chs == hz_index[i*2]) && (*(chs+1) == hz_index[i*2+1]))
				{
					OLED_Set_Pos(x, y);
					for(j=0;j<16;j++)
						Write_IIC_Data(hz[i*32+j]);
					OLED_Set_Pos(x,y+1);
					for(j=0;j<16;j++)
						Write_IIC_Data(hz[i*32+j+16]);
					x +=16;
					break;
				}
			}
			chs+=2;
		}
		else
		{
			if(x>122)
			{
				x=0;
				y=y+2;
			}
			m=strchr(zf_index,*chs);
			if (m!=NULL)
			{
				OLED_Set_Pos(x, y);
				for(j = 0; j < 8; j++)
					Write_IIC_Data(zf[((u8)*m-' ') * 16 + j]);
				OLED_Set_Pos(x, y + 1);
				for(j = 0; j < 8; j++)
					Write_IIC_Data(zf[((u8)*m-' ') * 16 + j + 8]);
				x += 8;
			}
			chs++;
		}
	}
}

//��ʼ��SSD1306
void OLED_Init(void)
{
	IIC_GPIO_Config();
  Write_IIC_Command(0xAE); //--display off
  Write_IIC_Command(0x00); //---set low column address
  Write_IIC_Command(0x10); //---set high column address
  Write_IIC_Command(0x40); //--set start line address
  Write_IIC_Command(0xB0); //--set page address
  Write_IIC_Command(0x81); // contract control
  Write_IIC_Command(0xFF); //--128
  Write_IIC_Command(0xA1); //set segment remap
  Write_IIC_Command(0xA6); //--normal / reverse
  Write_IIC_Command(0xA8); //--set multiplex ratio(1 to 64)
  Write_IIC_Command(0x3F); //--1/32 duty
  Write_IIC_Command(0xC8); //Com scan direction
  Write_IIC_Command(0xD3); //-set display offset
  Write_IIC_Command(0x00); //
  Write_IIC_Command(0xD5); //set osc division
  Write_IIC_Command(0x80); //
  Write_IIC_Command(0xD8); //set area color mode off
  Write_IIC_Command(0x05); //
  Write_IIC_Command(0xD9); //Set Pre-Charge Period
  Write_IIC_Command(0xF1); //
  Write_IIC_Command(0xDA); //set com pin configuartion
  Write_IIC_Command(0x12); //
  Write_IIC_Command(0xDB); //set Vcomh
  Write_IIC_Command(0x30); //
  Write_IIC_Command(0x8D); //set charge pump enable
  Write_IIC_Command(0x14); //
  Write_IIC_Command(0xAF); //--turn on oled panel
}

// x0,y0 -- ��ʼ������(x0:0~127, y0:0~7); x1,y1 -- ���Խ���(������)������(x1:1~128,y1:1~8)
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[])
{
	u16 j=0;
	u8 x,y;

  if(y1%8==0)
		y = y1/8;
  else
		y = y1/8 + 1;
	for(y=y0;y<y1;y++)
	{
		OLED_Set_Pos(x0,y);
    for(x=x0;x<x1;x++)
		{
			Write_IIC_Data(BMP[j++]);
		}
	}
}
