#ifndef __OLED_IIC_H
#define	__OLED_IIC_H

//#include "sys.h"
#include <inttypes.h>
#include <stdbool.h>
#include "stm32f4xx.h"

//使用IO口来模拟IIC时序，从而控制从器件
#define  IIC_SDA(n)  (n) ? GPIO_SetBits(GPIOC,GPIO_Pin_6) : GPIO_ResetBits(GPIOC,GPIO_Pin_6)
#define  IIC_SCL(n)  (n) ? GPIO_SetBits(GPIOC,GPIO_Pin_8) : GPIO_ResetBits(GPIOC,GPIO_Pin_8)

#define  IIC_READ		 GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_6)

extern unsigned int HZ;

unsigned int GB16_NUM(void);
void IIC_GPIO_Config(void);
void IIC_Start(void);
void IIC_Stop(void);
bool IIC_WaitAck(void);
void Write_IIC_Byte(uint8_t data);
void Write_IIC_Command(u8 IIC_Command);
void OLED_Fill(u8 fill_Data);
void OLED_Set_Pos(u8 x, u8 y);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_Clear(void);
void OLED_ShowChar(u8 x, u8 y, u8 chr);
void OLED_ShowNum(u8 x, u8 y, u32 num, u8 len,u8 mode);
void OLED_ShowCH(u8 x, u8 y,u8 *chs);
void OLED_Init(void);
void OLED_DrawBMP(u8 x0,u8 y0,u8 x1,u8 y1,u8 BMP[]);
#endif
