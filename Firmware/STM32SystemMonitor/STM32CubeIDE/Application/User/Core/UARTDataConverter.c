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

void TranslateToCPUTemperature(RingBuffer  *inputRingBuff)
{

	uint8_t inputString[128] = "";
	char resultString[10] = "";
	char * found;
	int resultCnt=0;
	int length = RingBuffer_GetDataLength(inputRingBuff);

	//Copy ring buffer into inputString
	RingBuffer_Read(inputRingBuff, &inputString[0], length );

	//Clear the strings
	memset(SystemInfo.CPUTemp,0, sizeof(SystemInfo.CPUTemp));
	memset(SystemInfo.GPUTemp,0, sizeof(SystemInfo.GPUTemp));
	memset(SystemInfo.GPUTemp,0, sizeof(SystemInfo.RAMUsage));
	memset(SystemInfo.SystemTime,0, sizeof(SystemInfo.SystemTime));

	//Search "CPU Total:"
	found = strstr((const char*)&inputString[0],(const char *)"CPU Total:");
	if( found != NULL )
	{
		//skip up to ":" and copy up to ","
		found = strstr((const char*)found, (const char *)":");
		while(*(++found)!=',')
		{
			SystemInfo.CPUTemp[resultCnt++] = *found;
		}
		SystemInfo.u16CPUUsage = (uint16_t)atof(SystemInfo.CPUTemp);
		//clear resultString for next usage
		memset(resultString, 0, sizeof(resultCnt));
	}

	//Search "GPU Core:"
	resultCnt = 0;
	found = strstr((const char*)&inputString[0],(const char *)"GPU Core:");
	if( found != NULL )
	{
    //skip up to ":" and copy up to ","
		found = strstr((const char*)found, (const char *)":");
		while(*(++found)!=',')
		{
			SystemInfo.GPUTemp[resultCnt++] = *found;
		}
		SystemInfo.u16GPUUsage = (uint16_t)atof(SystemInfo.GPUTemp);
	}

	//Search "Memory :"
    resultCnt = 0;
	found = strstr((const char*)&inputString[0],(const char *)"Memory:");
	if( found != NULL )
	{
    //skip up to ":" and copy up to ","
		found = strstr((const char*)found, (const char *)":");
		while(*(++found)!=',')
		{
			SystemInfo.RAMUsage[resultCnt++] = *found;
		}
		SystemInfo.u16RAMUsage = (uint16_t)atof(SystemInfo.RAMUsage);
	}

	//Search "Time:"
    resultCnt = 0;
	found = strstr((const char*)&inputString[0],(const char *)"Time:");
	if( found != NULL )
	{
	    //skip up to ":" and copy up to "-" for Hour
		found = strstr((const char*)found, (const char *)":");
		while(*(++found)!='-')
		{
			SystemInfo.SystemTime[resultCnt++] = *found;
		}
		SystemInfo.u8SysTimeHr = (uint8_t)atoi(SystemInfo.SystemTime);
		memset(SystemInfo.SystemTime,0, sizeof(SystemInfo.SystemTime));
		resultCnt = 0;
		//skip up to "-" and copy up to "-" for min
		while(*(++found)!='-')
		{
			SystemInfo.SystemTime[resultCnt++] = *found;
		}
		SystemInfo.u8SysTimeMin = (uint8_t)atoi(SystemInfo.SystemTime);
		memset(SystemInfo.SystemTime,0, sizeof(SystemInfo.SystemTime));
		resultCnt = 0;
		//skip up to "-" and copy up to "\n" for min
		while(*(++found)!='\n')
		{
			SystemInfo.SystemTime[resultCnt++] = *found;
		}
		SystemInfo.u8SysTimeSec = (uint8_t)atoi(SystemInfo.SystemTime);
		memset(SystemInfo.SystemTime,0, sizeof(SystemInfo.SystemTime));
	}
}
