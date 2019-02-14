/*

*/

#ifndef __DRV8825_H__
#define __DRV8825_H__
#include "../HardwareCommon.h"

/*步进电机参数*/
#define MICROSTEP_FULL		0
#define MICROSTEP_1_2		1
#define MICROSTEP_1_4		2
#define MICROSTEP_1_8		3
#define MICROSTEP_1_16		4
#define MICROSTEP_1_32		5

#define DRV8825_MODE0 PAout(5)
#define DRV8825_MODE1 PAout(6)
#define DRV8825_MODE2 PAout(7)
#define DRV8825_SLEEP PBout(2)

void Drv8825_MicroStep_Set(uint8_t microStep);
void Drv8825_Init(void);

#endif
