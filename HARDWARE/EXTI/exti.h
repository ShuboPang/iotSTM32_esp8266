#ifndef __EXTI_H
#define __EXIT_H	 
#include "sys.h"  	
//////////////////////////////////////////////////////////////////////////////////	 
 
// STM32F407开发板
//外部中断 驱动代码	   
//STM32F4工程-库函数版本					  
////////////////////////////////////////////////////////////////////////////////// 	 


void Dis_Init(void);
void EXTIX_Init(void);	//外部中断初始化	
void stopDisRun(void);
void Timer_Config(void);
u8 IsDisRunning(void);
void GetDistanceDelay(void);
u8 getEcho(void);
void EXTIX_Init_Down(void);
#endif
