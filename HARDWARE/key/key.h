#ifndef __KEY_H
#define __KEY_H	 
//#include "sys.h"	 
#include "stm32f4xx.h"

extern u8 key_num;

void KEY_Init(void);//IO��ʼ��
u8 KEY_Scan(u8 mode);//����ɨ�躯��					    
#endif
