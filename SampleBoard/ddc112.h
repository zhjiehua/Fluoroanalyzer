#ifndef _DDC112_H_
#define _DDC112_H_

#include "common.h"

uint8_t DDC112_Read(uint32_t *pReadData);
void DDC112_SendData(uint8_t head, uint8_t *pData, uint8_t count);

#endif