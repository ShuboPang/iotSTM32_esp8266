#include "sys.h"
#include "string.h"
#include "ESP8266.h"
#include "led.h"

static unsigned char TxBuffer[256] = {0};
static unsigned char TxCounter = 0;
static unsigned char RxBuffer[256] = {0};
static unsigned char RxCounter = 0;
static unsigned char RxLength = 0;
static unsigned char count = 0;
static unsigned char esp8266IsInit = 0;
static unsigned char ipAdress[] = { "192.168.4.8" };
static unsigned char port[] = { "8008" };

void Initial_UART2(unsigned long baudrate)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2 | RCC_AHB1Periph_GPIOA, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	//串口2对应引脚复用映射
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2); //GPIOA2复用为USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2); //GPIOA3复用为USART2

	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART2, &USART_InitStructure);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	USART_ClearFlag(USART2, USART_FLAG_TC);
	USART_Cmd(USART2, ENABLE);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
void USART2_IRQHandler(void)
{
	if (USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{
		USART_SendData(USART2, TxBuffer[TxCounter++]);
		USART_ClearITPendingBit(USART2, USART_IT_TXE);
		if (TxCounter == count) 
			USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	}
	else if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		CopeSerial2Data((unsigned char)USART2->DR);//处理数据
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
	}

	USART_ClearITPendingBit(USART2, USART_IT_ORE);
}


void UART2_Put_Char(unsigned char DataToSend)
{
	TxBuffer[count++] = DataToSend;
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
}

void UART2_Put_String(unsigned char* Str)
{
	while (*Str)
	{
		if (*Str == '\r')UART2_Put_Char(0x0d);
		else if (*Str == '\n')UART2_Put_Char(0x0a);
		else UART2_Put_Char(*Str);
		Str++;
	}
}

int CopeSerial2Data(unsigned char ucData)
{

	if (ucData == '\r' || ucData == '\n')
	{
		RxLength = RxCounter;
		RxBuffer[RxCounter++] = '\0';
		RxCounter = 0;
		return 1;
	}
	if(ucData == '\0')
	{
		RxCounter = 0;
		RxLength = RxCounter;
		return 1;
	}
	RxBuffer[RxCounter++] = ucData;
	LED_Pulse(2);
	return 0;
	
}

void esp8266IO_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);//使能GPIOA时钟

	//GPIOF9,F10初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化

	GPIO_SetBits(GPIOA, GPIO_Pin_4);//GPIOA4设置高，esp8266上电

}

int esp8266ReInit(u32 time) 
{
	static u32 i = 0;
	if (i  == 0)
	{
		esp8266IsInit = 0;
		GPIO_ResetBits(GPIOA, GPIO_Pin_4);// esp8266断电
		return 0;
	}
	else if (i >= time * 2)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_4);//GPIOA4 esp8266上电
		i = 0;
		return 1;
	}
}



ESP8266STATUS esp8266Test(void)
{
	static char step = 0;
	char send[MAX_LENGTH] = { 0 };
	static int errorCount = 0;
	switch (step)
	{
	case 0:
		//发送数据
		sprintf(send, "AT\r\n");
		UART2_Put_String(send);
		errorCount = 0;
		step++;
		break;
	case 1:
		if (RxCounter == 0 && RxLength)
		{
			
			if (strcmp(RxBuffer, "OK") == 0)
			{
				step = 0;
				return SETOK;
			}
			if (strcmp(RxBuffer, "ERROR") == 0)
			{
				step = 0;
				return SETERROR;
			}
			if (RxBuffer[0] == 'b' && RxBuffer[1] == 'u')
			{
				return BUSY;
			}
		}
		else 
		{
			errorCount++;
			if (errorCount > 1000) 
			{
				step = 0;
			}
		}
		break;
	default:
		break;
	}
	return WAIT;
}

ESP8266STATUS esp8266SetClient(void)
{
	static char step = 0;
	char send[MAX_LENGTH] = { 0 };
	static int errorCount = 0;
	static int succedCount = 0;
	switch (step)
	{
	case 0:
		//发送数据
		sprintf(send, "AT+CIPSTART=\"TCP\",\"%s\",%s\r\n",ipAdress,port);
		UART2_Put_String(send);
		errorCount = 0;
		step++;
		break;
	case 1:
		if (RxCounter == 0 && RxLength)
		{
			if (strcmp(RxBuffer, "OK") == 0)
			{
				step = 0;
				return SETOK;
			}
			else if (strcmp(RxBuffer, "ERROR") == 0)
			{
				step = 0;
				return SETOK;
			}
			else if (RxBuffer[0] == 'b' && RxBuffer[1] == 'u')
			{
				return BUSY;
			}
			else if (RxBuffer[0] == 'A' && RxBuffer[1] == 'R')
			{
				return SETOK;
			}
			succedCount++;
			if (succedCount > 3) 
			{
				succedCount = 0;
				return SETOK;
			}
		}
		else
		{
			errorCount++;
			if (errorCount > 1000)
			{
				step = 0;
			}
		}
		break;
	default:
		break;
	}
	return WAIT;
}

ESP8266STATUS esp8266SetMode(void)
{
	static char step = 0;
	char send[MAX_LENGTH] = { 0 };
	static int errorCount = 0;
	switch (step)
	{
	case 0:
		//发送数据
		sprintf(send, "AT+CIPMODE=1\r\n");
		UART2_Put_String(send);
		errorCount = 0;
		step++;
		break;
	case 1:
		if (RxCounter == 0 && RxLength)
		{

			if (strcmp(RxBuffer, "OK") == 0)
			{
				step = 0;
				return SETOK;
			}
			if (strcmp(RxBuffer, "ERROR") == 0)
			{
				step = 0;
				return SETERROR;
			}
			if (RxBuffer[0] == 'b' && RxBuffer[0] == 'u')
			{
				return BUSY;
			}
		}
		else
		{
			errorCount++;
			if (errorCount > 1000)
			{
				step = 0;
			}
		}
		break;
	default:
		break;
	}
	return WAIT;
}

ESP8266STATUS esp8266SetSend(void)
{
	static char step = 0;
	char send[MAX_LENGTH] = { 0 };
	static int errorCount = 0;
	switch (step)
	{
	case 0:
		//发送数据
		sprintf(send, "AT+CIPSEND\r\n");
		UART2_Put_String(send);
		errorCount = 0;
		step++;
		break;
	case 1:
		if (RxCounter == 0 && RxLength)
		{
			if (strcmp(RxBuffer, "OK") == 0)
			{
				step = 0;
				return SETOK;
			}
			else if (strcmp(RxBuffer, ">") == 0)
			{
				step = 0;
				return SETOK;
			}
			else if (strcmp(RxBuffer, "ERROR") == 0)
			{
				step = 0;
				return SETERROR;
			}
			else if (RxBuffer[0] == 'b' && RxBuffer[0] == 'u')
			{
				return BUSY;
			}
			return SETOK;
		}
		else
		{
			errorCount++;
			if (errorCount > 1000)
			{
				step = 0;
			}
		}
		break;
	default:
		break;
	}
	return WAIT;
}

ESP8266STATUS esp8266SendBuff(unsigned char* Str)
{
	static char step = 0;
	char send[MAX_LENGTH] = { 0 };
	static int errorCount = 0;
	switch (step)
	{
	case 0:
		//发送数据
		sprintf(send, "Str=%s\r\n", Str);
		UART2_Put_String(send);
		errorCount = 0;
		step++;
		break;
	case 1:
		if (RxCounter == 0 && RxLength)
		{
			if (strcmp(RxBuffer, "OK") == 0)
			{
				step = 0;
				return SETOK;
			}
			else if (strcmp(RxBuffer, ">") == 0)
			{
				step = 0;
				return SETOK;
			}
			else if (strcmp(RxBuffer, "ERROR") == 0)
			{
				step = 0;
				return SETERROR;
			}
			else if (RxBuffer[0] == 'b' && RxBuffer[0] == 'u')
			{
				return BUSY;
			}
			return SETOK;
		}
		else
		{
			errorCount++;
			if (errorCount > 1000)
			{
				//esp8266ReInit(1);
				step = 0;
			}
		}
		break;
	default:
		break;
	}
	return WAIT;
}


int esp8266Init(void) 
{
	static int step = 0;
	if (esp8266IsInit) 
	{
		step = 0;
		return esp8266IsInit;
	}
	ESP8266STATUS ret;
	switch (step)
	{
	case 0:
		//测试设备是否在线
		esp8266IO_Init();
		ret = esp8266Test();
		if (ret == SETOK)
		{
			step++;
		}
		break;
	case 1:
		//配置ip地址
		ret = esp8266SetClient();
		if (ret == SETOK)
		{
			step++;
		}
		break;
	case 2:
		//设置透传模式
		ret = esp8266SetMode();
		if (ret == SETOK)
		{
			step++;
		}
		break;
	case 3:
		//准备开始发送数据
		ret = esp8266SetSend();
		if(ret == SETOK)
		{
			step = 0;
			//初始化结束
			esp8266IsInit = 1;
		}
		break;
	default:
		break;
	}
	if (ret == SETERROR) 
	{
		step = 0;
	}
	return 0;
}
