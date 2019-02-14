#include "common.h"
#include "uart.h"
#include "io.h"
#include "cmd_process.h"
#include "stepmotor.h"

void Sort(uint32_t *pBuffer, uint8_t count)
{
	uint8_t i, j;
	uint32_t temp;
	for(i=0;i<count-1;i++)
	{
		for(j=i+1;j<count;j++)
		{
			if(pBuffer[i] > pBuffer[j])
			{
				temp = pBuffer[j];
				pBuffer[j] = pBuffer[i];
				pBuffer[i] = temp;
			}
		}
	}

	//printfDDC112Data(pBuffer, count*4);
}

uint32_t GetAverage(uint32_t *pBuffer, uint8_t count, uint8_t discardCount)
{
	uint8_t i;
	uint32_t average = 0;

	for(i=discardCount;i<(count-discardCount);i++)
		average += pBuffer[i];

	average /= (count-2*discardCount);
	return average;
}





