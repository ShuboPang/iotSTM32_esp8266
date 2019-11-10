#include "communication.h"
#include "usart.h"


void sendTo(u32 times)
{
	static u8 count = 0;
	static u8 i = 0;
	i++;
	if (count == 0)
	{
		setSendBuff();
	}
	if (i >= times)
	{
		printf("#%d,%d,%d,%d,%d,%d,%d,%d", s_buff[count][0],s_buff[count][1], s_buff[count][2],
										 s_buff[count][3], s_buff[count][4], s_buff[count][5],
										 s_buff[count][6], s_buff[count][7]);
		count++;
		if (count >= MAX_COUNT)
		{
			count = 0;
		}
		i = 0;
	}
}

void recv()
{
	
}
