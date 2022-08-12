/*
 * UARTDataConverter.c
 *
 *  Created on: Aug 9, 2022
 *      Author: Aung Tun Linn
 */
#include <stdlib.h>
#include <string.h>
#include "main.h"
#include "ringbuffer.h"
extern struct systemInfo_s SystemInfo;
/*
 * 	char CPUString[16];
	char GPUString[16];
	char RAMString[16];
	char TimeString[16];
	uint16_t u16CPUUsage;
	uint16_t u16GPUUsage;
	uint16_t u16RAMUsage;
	uint16_t u16CPUTemp;
	uint16_t u16GPUTemp;
	uint8_t u8SysTimeHr;
	uint8_t u8SysTimeMin;
	uint8_t u8SysTimeSec;
 * */
float GetTheValueFromString(const char * inputStr, const char * findthisStr, char terminator)
{
	char * found = strstr(inputStr,findthisStr);
	char str[16] = "";
	int idx = 0;
	if( found != NULL)
	{
		//skip up to':'
		found = strstr((const char*)found, (const char *)":");
		if(found != NULL )
		{
			while(*(++found)!=terminator)
			{
				str[idx++] = *found;
			}
			return atof(str);
		}
	}
	return 0.0;
}
void TranslateToCPUTemperature(RingBuffer  *inputRingBuff)
{

	uint8_t inputString[128] = "";
	char resultString[10] = "";
	char * found;
	int resultCnt=0;
	int length = RingBuffer_GetDataLength(inputRingBuff);

	//Copy ring buffer into inputString
	RingBuffer_Read(inputRingBuff, &inputString[0], length );

	SystemInfo.fCPUTemp    = GetTheValueFromString((const char*)inputString, "CPUTemp:", ',');
	SystemInfo.u16CPUUsage = (uint16_t)GetTheValueFromString((const char*)inputString, "CPULoad:", ',');
	SystemInfo.fGPUTemp    = GetTheValueFromString((const char*)inputString, "GPUTemp:", ',');
	SystemInfo.u16GPUUsage = (uint16_t)GetTheValueFromString((const char*)inputString, "GPULoad:", ',');
	SystemInfo.u16RAMUsage = (uint16_t)GetTheValueFromString((const char*)inputString, "RAM:", ',');

	//Search "Time:"
	found = strstr((const char*)&inputString[0],(const char *)"Time:");
	if( found != NULL )
	{
	    //skip up to ":" and copy up to "-" for Hour
		found = strstr((const char*)found, (const char *)":");
		while(*(++found)!='-')
		{
			resultString[resultCnt++] = *found;
		}
		SystemInfo.u8SysTimeHr = (uint8_t)atoi(resultString);

		memset(resultString,0, sizeof(resultString));
		resultCnt = 0;
		//skip up to "-" and copy up to "-" for min
		while(*(++found)!='-')
		{
			resultString[resultCnt++] = *found;
		}
		SystemInfo.u8SysTimeMin = (uint8_t)atoi(resultString);

		memset(resultString,0, sizeof(resultString));
		resultCnt = 0;
		//skip up to "-" and copy up to "\n" for min
		while(*(++found)!='\n')
		{
			resultString[resultCnt++] = *found;
		}
		SystemInfo.u8SysTimeSec = (uint8_t)atoi(resultString);
	}
}
