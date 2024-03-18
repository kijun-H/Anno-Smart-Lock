#ifndef __KEY_H
#define __KEY_H	 
//#include "sys.h"	 
#include "stm32f4xx.h"

extern u8 key_num;

void KEY_Init(void);//IO初始化
u8 KEY_Scan(u8 mode);//按键扫描函数					    
#endif
