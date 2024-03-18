#ifndef __PASSWORDKEY_H
#define __PASSWORDKEY_H

#define  KEY_BUFFERSIZE  256									//接收缓冲区的大小

extern __IO char key_buffer[KEY_BUFFERSIZE];    //作为密码的接收缓冲区
extern __IO int  key_cnt;					  //作为密码的接收计数器
extern __IO uint8_t key_recvflag;		  //作为密码的接收标志位

void KEY_Config(void);

#endif
