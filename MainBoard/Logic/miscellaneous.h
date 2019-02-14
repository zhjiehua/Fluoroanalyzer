#ifndef _MISCELLANEOUS_H_
#define _MISCELLANEOUS_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"	

typedef struct
{
	uint32_t x;
	uint32_t y;
}Point_TypeDef;
	
void GetChipID(uint32_t *chipID);

uint8_t BCDToInt(uint8_t bcd);
uint8_t IntToBCD(uint8_t i);	

//uint32_t EndianConvertLToB(uint32_t InputNum);
//uint32_t EndianConvertBToL(uint32_t InputNum);
	
void EndianConvert(uint32_t *pdata);

uint32_t OrderFilter(uint32_t *buffer, uint32_t order);
void LeastSquare(uint32_t *buffer, uint32_t buf_len, double *a, double *b);

uint32_t FindThreshod(uint32_t *buffer, uint32_t bufLen, uint32_t pointA, uint32_t pointB, uint32_t win, float weight);
	
uint32_t Average(uint32_t *buffer, uint32_t bufLen);
void Sort(Point_TypeDef *buffer, uint32_t bufLen);

uint32_t FindMaximin(uint32_t *buffer, uint32_t bufLen);
uint32_t FindMaximal(uint32_t *buffer, uint32_t bufLen);

uint8_t FindMaximalPoint(uint32_t *buffer, uint32_t bufLen, uint32_t *pos, uint16_t *posLen);

uint32_t GetCTValue(uint16_t pos);

#define DDC112_SETRANGE 	0x00 //设置DDC112测量范围
#define DDC112_SM_CW 		0x01 //步进电机正转
#define DDC112_SM_CCW 		0x02 //步进电机反转
#define DDC112_SM_STOP 		0x03 //步进电机停止
#define DDC112_SETMODE_TEST 0x04 //进入测试模式
#define DDC112_SETINTEGRALTIME 	0x05 //设置DDC112积分时间
#define DDC112_SETBUFFERSIZE 	0x06 //设置DDC112缓冲区大小 ，1 <= bufferSize <= 20

void User_Data_Parse(uint8_t *buffer, uint8_t len);
	
#ifdef __cplusplus
}
#endif
	
#endif
