#ifndef _QRCODEDATA_H_
#define _QRCODEDATA_H_

#include "sys.h"

#define QRCODE_CMD_PROJECT		'P'
#define QRCODE_CMD_BATCH		'B'
#define QRCODE_CMD_SAMPLE		'S'

#define QRCODE_CMD_ALL		'!' //所有参数

void QRCode_Data_Parse(uint8_t *buffer, uint8_t len);

#endif
