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
	uart_init(115200);	//���Դ���
	Initial_UART2(115200);	//����WiFiģ��
	Initial_UART3(115200);
	
	LED_Init();
	TIM3_Int_Init(2 - 1, 8400 - 1);	//��ʱ��ʱ��84M����Ƶϵ��8400������84M/8400=10Khz�ļ���Ƶ�ʣ�������Ϊ0.2ms 
}

int main(void)
{
	sys_init();
	while(1)
	{
	}
}
