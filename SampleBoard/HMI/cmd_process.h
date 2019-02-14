#ifndef _CMD_PROCESS_H
#define _CMD_PROCESS_H
#include "hmi_driver.h"

#define CMD_RANGE_SETTING			    0X81  //设置参数
#define CMD_REDLED_SETTING				0X82  //红光LED设置
#define CMD_PURPLELED_SETTING			0X83  //紫光LED设置
#define CMD_SAMPLE_START			    0X84  //开始采集
#define CMD_SAMPLE_STOP     			0X85  //停止采集
#define CMD_INTEGRALTIME_SETTING  		0X86  //设置积分时间
#define CMD_BUFFERSIZE_SETTING  		0X87  //设置缓冲区大小，该缓冲区满了将产生一个有效数据

#define CMD_SAMPLE     			        0X8A  //读取采集值

#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //从缓冲区取16位数据
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //从缓冲区取32位数据

void ProcessMessage( uint8 *msg, uint16 size );

#endif
