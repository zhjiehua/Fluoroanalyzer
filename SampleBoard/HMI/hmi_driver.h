#ifndef _HMI_DRIVER_
#define _HMI_DRIVER_

#define FIRMWARE_VER 917   //ÆÁÄ»¹Ì¼þ°æ±¾ºÅ

#define DATASAFTY_CHECK_ENABLE 1


#include "hmi_user_uart.h"

uint8 CheckDataSafty(uint8 *buffer, uint16 n);

#endif
