#include "miscellaneous.h"
#include "stm32f10x.h"
#include "Queue.h"
#include "./DDC112/ddc112_interface.h"
#include "CPrintf.h"
#include "management.h"
#include "../HARDWARE/StepMotor/StepMotor.h"

void GetChipID(uint32_t *chipID)
{
#ifdef STM32SIM
	chipID[0] = *(__IO uint32_t*)(0X1FFFF7F0); //高字节
	chipID[1] = *(__IO uint32_t*)(0X1FFFF7EC);
	chipID[2] = *(__IO uint32_t*)(0X1FFFF7E8); //低字节
#else
	chipID[0] = 0X0FFFF7F0;
	chipID[1] = 0X0FFFF7EC;
	chipID[2] = 0X1FFFF7E8;
#endif
	//STM32_FLASH_SIZE = *(__IO uint32_t*)(0X1FFFF7E0);
}

uint8_t BCDToInt(uint8_t bcd) //BCD转10进制
{
	return (0xff & (bcd>>4))*10 +(0xf & bcd);
}

uint8_t IntToBCD(uint8_t i) //10进制转BCD
{
	return (((i/10)<<4)+((i%10)&0x0f));
}

////小端转大端
//uint32_t EndianConvertLToB(uint32_t InputNum)
//{
//    uint8_t *p = (uint8_t*)&InputNum;
//    return(((uint32_t)*p<<24)+((uint32_t)*(p+1)<<16)+((uint32_t)*(p+2)<<8)+((uint32_t)*(p+3)));
//}

////大端转小端
//uint32_t EndianConvertBToL(uint32_t InputNum)
//{
//    uint8_t *p = (uint8_t*)&InputNum;
//    return(((uint32_t)*p)+((uint32_t)*(p+1)<<8)+((uint32_t)*(p+2)<<16)+((uint32_t)*(p+3)<<24));
//}

//小端转大端或大端转小端
void EndianConvert(uint32_t *pdata)
{
    uint8_t *p = (uint8_t*)pdata;
	uint8_t temp = *(p+3);
	*(p+3) = *p;
	*p = temp;
	temp = *(p+2);
	*(p+2) = *(p+1);
	*(p+1) = temp;
}

//多阶滤波
uint32_t OrderFilter(uint32_t *buffer, uint32_t order)
{
	int32_t i;
	uint32_t average = 0;
	uint32_t averageCnt = 0;
	for (i=0; i<order; i++)
	{
		average += (buffer[-i] * (i + 1));
		averageCnt += (i + 1);
	}
	average /= averageCnt;
	return average;
}

//最小二乘法基线拟合
void LeastSquare(uint32_t *buffer, uint32_t buf_len, double *a, double *b)
{
	uint32_t i;
	double xSum =0, ySum =0, xySum =0, xxSum = 0;//这几个变量一定要设成float
	for (i = 0; i < buf_len; i++)
	{
		xSum += i;
		ySum += buffer[i];
		xySum += i*buffer[i];
		xxSum += i*i;
	}

	*a = (xySum - xSum*ySum / buf_len) / (xxSum - xSum*xSum / buf_len);
	*b = (xxSum*ySum / buf_len - xySum*xSum / buf_len) / (xxSum - xSum*xSum / buf_len);
}

uint32_t FindThreshod(uint32_t *buffer, uint32_t bufLen, uint32_t pointA, uint32_t pointB, uint32_t win, float weight)
{
	uint32_t i;
	uint64_t sum = 0;
	
	for(i=0;i<bufLen;i++)
	{
		if(((i < (pointA+win)) && (i > (pointA-win)))
			|| ((i < (pointB+win)) && (i > (pointB-win))))
			sum += buffer[i]*weight;
		else
			sum += buffer[i];
	}
	
	return (uint32_t)(sum/bufLen);
}

//求平均值
uint32_t Average(uint32_t *buffer, uint32_t bufLen)
{
	uint32_t i;
	uint64_t sum = 0;
	
	for(i=0;i<bufLen;i++)
		sum += buffer[i];
	
	return (uint32_t)(sum/bufLen);
	//return sum;
}

//排序
void Sort(Point_TypeDef *buffer, uint32_t bufLen)
{
	uint32_t i, j;
	uint32_t temp;
	
	for(i=0;i<bufLen-1;i++)
	{
		for(j=i+1;j<bufLen;j++)
		{
			if(buffer[i].y < buffer[j].y)
			{
				temp = buffer[i].y;
				buffer[i].y = buffer[j].y;
				buffer[j].y = temp;
				
				temp = buffer[i].x;
				buffer[i].x = buffer[j].x;
				buffer[j].x = temp;
			}
		}
	}
}

//找最大值
uint32_t FindMaximin(uint32_t *buffer, uint32_t bufLen)
{
	uint32_t i;
	uint32_t ret = 0;
	for(i=1;i<bufLen;i++)
	{
		if(buffer[i] > buffer[ret])
			ret = i;
	}
	return ret;
}

//找极大值，返回极大值点下标
uint32_t FindMaximal(uint32_t *buffer, uint32_t bufLen)
{
	uint32_t i, j;
	int32_t slopePre = 0, slopeCur = 0;
	uint32_t maximalArray[MAXIMAL_BUFFER_SIZE];
	uint32_t ret;
	
	j = 0;
	slopePre = (int32_t)buffer[1] - (int32_t)buffer[0];
	for(i=1;i<bufLen-1;i++)
	{
		slopeCur = (int32_t)buffer[i+1] - (int32_t)buffer[i];
		
		if(slopeCur <= 0 && slopePre > 0)//极大值
		{
			//cDebug("maximalArray[%d].x = %d\r\n", j, i);
			
			maximalArray[j] = i;
			j++;
			if(j >= MAXIMAL_BUFFER_SIZE)
			{
				cDebug("Warnning : maximalArray[] is overflow!\r\n");
				break;
			}
		}

		slopePre = slopeCur;
	}
	
	//在极大值中找最大值
	ret = 0;
	for(i=1;i<j;i++)
	{
		if(buffer[maximalArray[i]] > buffer[ret])
			ret = maximalArray[i];
	}
	
	return ret;
}

//找极大值点
uint8_t FindMaximalPoint(uint32_t *buffer, uint32_t bufLen, uint32_t *pos, uint16_t *posLen)
{
	uint32_t i, j, k;
	int32_t slopePre = 0, slopeCur = 0;
	Point_TypeDef maximalArray[MAXIMAL_BUFFER_SIZE];
	Point_TypeDef maximalFilterArray[MAXIMAL_BUFFER_SIZE];
	//Point_TypeDef minimalArray[MAXIMAL_BUFFER_SIZE];
	//uint32_t a, b;//基线拟合参数
	uint32_t average;//平均值，做阈值
	
	j = 0;
	k = 0;
	slopePre = (int32_t)buffer[1] - (int32_t)buffer[0];
	for(i=1;i<bufLen-1;i++)
	{
		slopeCur = (int32_t)buffer[i+1] - (int32_t)buffer[i];
		
		if(slopeCur <= 0 && slopePre > 0)//极大值
		{
			cDebug("maximalArray[%d].x = %d\r\n", j, i);
			
			maximalArray[j].x = i;
			maximalArray[j].y = buffer[i];
			j++;
			if(j >= MAXIMAL_BUFFER_SIZE)
				return 1;
		}
//		else if(slopeCur > 0 && slopePre <= 0) //极小值
//		{
//			minimalArray[k].x = i;
//			minimalArray[k].y = buffer[i];
//			k++;
//			if(k >= 50)
//				return 1;
//		}

		slopePre = slopeCur;
	}

	//Sort(maximalArray, j); //排序	
	//LeastSquare(buffer, bufLen, &a, &b);
	
	//average = Average(buffer, bufLen);//求平均值，做判断峰值的阈值
	average = buffer[pProjectMan->axisTX - pProjectMan->axisCX];
	
	cDebug("maximalArray len = %d\r\n", j);
	//cDebug("average = %d\r\n", average);
	
//	*posLen = 0;
//	for(i=0;i<j;i++)
//	{
//		if(maximalArray[i].y > average)
//		{
//			*(pos + *posLen) = maximalArray[i].x;
//			(*posLen)++;
//		}
//	}

	
	k = 0;
	for(i=0;i<j;i++)
	{
		if(maximalArray[i].y > average)
		{
			maximalFilterArray[k].x = maximalArray[i].x;
			maximalFilterArray[k].y = maximalArray[i].y;
			
			cDebug("maximalFilterArray[%d].x = %d\r\n", k, maximalFilterArray[k].x);
			
			k++;
		}
	}
	j = k;
	
	cDebug("maximalFilterArray len = %d\r\n", j);
	
	//消抖
	for(i=0;i<j-1;i++)
	{
		uint32_t max = 0;
		uint8_t cnt = 0;
		k = 0;
		while(maximalFilterArray[i+k+1].x - maximalFilterArray[i+k].x < 50)
		{
			k++;
		}
		if(k > 0)
		{
			max = maximalFilterArray[i+cnt].x;
			for(cnt=0;cnt<k;cnt++)
			{
				if(maximalFilterArray[i+cnt+1].y > maximalFilterArray[i+cnt].y)
					max = maximalFilterArray[i+cnt+1].x;
			}
			
			for(cnt=0;cnt<=k;cnt++)
			{
				if(maximalFilterArray[i+cnt].x != max)
					maximalFilterArray[i+cnt].y = average;
			}
			
			i += k;
		}
	}

	*posLen = 0;
	for(i=0;i<j;i++)
	{
		if(maximalFilterArray[i].y > average)
		{
			*(pos + *posLen) = maximalFilterArray[i].x;
			(*posLen)++;
		}
	}
	
	return 0;
}

//找极大点
uint8_t MaxPoint(uint32_t *buffer, uint32_t buf_len, uint32_t *pos, uint32_t pos_len, int32_t threshold)
{
	uint32_t i, j = 0;
	uint8_t flag = 0;
	uint8_t num = 0;
	uint32 posArray[350];
	uint32_t len;

	for (i = 0; i < buf_len; i++)
	{
		if (!flag)
		{
			if (buffer[i] > threshold)
			{
				posArray[j++] = i;
				flag = 1;
			}
		}
		else
		{
			if (buffer[i] < threshold)
			{
				posArray[j++] = i;
				flag = 0;
			}
		}
	}

	len = j / 2 * 2;
	for (i = 0; i < len; i+=2)
	{
		if (posArray[i + 1] - posArray[i] > 10)
		{
			for (j = posArray[i]; j < posArray[i + 1]; j++)
			{
				if (buffer[j] > buffer[j - 1])
				{
					pos[num] = j;
				}
			}
			num++;
			if(num >= pos_len)
				break;
		}
	}

	return num;
}


uint32_t GetCTValue(uint16_t pos)
{
	int16_t i = 0;
	uint16_t cnt = 0;
	uint32_t temp;
	pProjectMan->peakBufferCount = 0;
	while(cnt++ <= pProjectMan->sampleDataCount/4)
	//while(1)
	{
			temp = pProjectMan->LeastSquareA*(pos + i) + pProjectMan->LeastSquareB;
			if(pProjectMan->sampleData[pos + i] >= temp) i--;
			else break;
	}
	i++;
	cDebug("GetCTValue = %d ~ ", i);
	
	cnt = 0;
	while(cnt++ <= pProjectMan->sampleDataCount/2)
	{
			temp = pProjectMan->LeastSquareA*(pos + i) + pProjectMan->LeastSquareB;
			if(pProjectMan->sampleData[pos + i] >= temp)
			{
				pProjectMan->peakBuffer[pProjectMan->peakBufferCount] = pProjectMan->sampleData[pos + i] - temp;
				pProjectMan->peakBufferCount++;
				i++;
			}
			else break;
	}
	cDebug("%d\r\n", i);
	return Average(pProjectMan->peakBuffer, pProjectMan->peakBufferCount);
}



//解释用户串口数据
void User_Data_Parse(uint8_t *buffer, uint8_t len)
{
	//uint8_t i;
	uint8_t cmd = buffer[0];
	switch(cmd)
	{
		case DDC112_SETRANGE: //设置DDC112测量范围
			DDC112IF_RangeSetting(buffer[1]);
			cDebug("DDC112IF_RangeSetting %d\r\n", buffer[1]);
		break;
		case DDC112_SM_CW: //
			StepMotor_SetSpeed(0, pProjectMan->sampleHomeSpeed);
			StepMotor_SetDir(0, CW);
			StepMotor_SetCMD(0, ENABLE);
			pProjectMan->projectMode = PROJECTMODE_MANUAL;
		break;
		case DDC112_SM_CCW: //
			StepMotor_SetSpeed(0, pProjectMan->sampleHomeSpeed);
			StepMotor_SetDir(0, CCW);
			StepMotor_SetCMD(0, ENABLE);
			pProjectMan->projectMode = PROJECTMODE_MANUAL;
		break;
		case DDC112_SM_STOP: //
			StepMotor_Stop(0);
			pProjectMan->projectMode = PROJECTMODE_NONE;
			//cDebug("DDC112_SM_STOP\r\n");
		break;
		case DDC112_SETMODE_TEST: //
			pProjectMan->projectMode = PROJECTMODE_TEST;
		break;
		case DDC112_SETINTEGRALTIME: //0x05
			DDC112IF_IntegralTimeSetting(buffer[1]);
			cDebug("DDC112IF_IntegralTimeSetting %d\r\n", buffer[1]);
		break;
		case DDC112_SETBUFFERSIZE: //0x06
			DDC112IF_BufferSizeSetting(buffer[1]);
			cDebug("DDC112IF_BufferSizeSetting %d\r\n", buffer[1]);
		break;
		default:
		break;
	}
}
