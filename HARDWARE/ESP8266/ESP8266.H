#ifndef __UART2_H
#define __UART2_H

#define MAX_LENGTH 1024

typedef enum
{
	WAIT,
	SETOK,
	BUSY,
	SETERROR,
}ESP8266STATUS;

void Initial_UART2(unsigned long baudrate);
void USART2_IRQHandler(void);
void UART2_Put_Char(unsigned char DataToSend);
void UART2_Put_String(unsigned char* Str);
int CopeSerial2Data(unsigned char ucData);
int esp8266Init(void);
ESP8266STATUS esp8266Test(void);
ESP8266STATUS esp8266SendBuff(unsigned char* Str);
void esp8266IO_Init(void);
int esp8266ReInit(u32 time);
#endif