#ifndef __BLE_H
#define __BLE_H

#include "stm32f4xx.h"

#define  BLE_BUFFERSIZE  256									//接收缓冲区的大小

extern __IO char ble_buffer[BLE_BUFFERSIZE];  //作为蓝牙模块的接收缓冲区
extern __IO int  ble_cnt;					  //作为蓝牙模块的接收计数器
extern __IO uint8_t ble_recvflag;		  //作为蓝牙模块的接收标志位

void USART2_Config(u32 baud);

//串口2中断服务函数
void USART2_IRQHandler(void);

#endif
