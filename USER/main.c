#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
#include "led.h"
#include "timer.h"
#include "UART3.h"
#include "ESP8266.h"

void sys_init()
{
	delay_init(168);
	uart_init(115200);	//调试串口
	Initial_UART2(115200);	//链接WiFi模块
	Initial_UART3(115200);
	
	LED_Init();
	TIM3_Int_Init(2 - 1, 8400 - 1);	//定时器时钟84M，分频系数8400，所以84M/8400=10Khz的计数频率，计数次为0.2ms 
}

int main(void)
{
	sys_init();
	while(1)
	{
	}
}
