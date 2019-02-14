#ifndef _CMD_PROCESS_H
#define _CMD_PROCESS_H
#include "hmi_driver.h"

#define CMD_RANGE_SETTING			    0X81  //���ò���
#define CMD_REDLED_SETTING				0X82  //���LED����
#define CMD_PURPLELED_SETTING			0X83  //�Ϲ�LED����
#define CMD_SAMPLE_START			    0X84  //��ʼ�ɼ�
#define CMD_SAMPLE_STOP     			0X85  //ֹͣ�ɼ�
#define CMD_INTEGRALTIME_SETTING  		0X86  //���û���ʱ��
#define CMD_BUFFERSIZE_SETTING  		0X87  //���û�������С���û��������˽�����һ����Ч����

#define CMD_SAMPLE     			        0X8A  //��ȡ�ɼ�ֵ

#define PTR2U16(PTR) ((((uint8 *)(PTR))[0]<<8)|((uint8 *)(PTR))[1])  //�ӻ�����ȡ16λ����
#define PTR2U32(PTR) ((((uint8 *)(PTR))[0]<<24)|(((uint8 *)(PTR))[1]<<16)|(((uint8 *)(PTR))[2]<<8)|((uint8 *)(PTR))[3])  //�ӻ�����ȡ32λ����

void ProcessMessage( uint8 *msg, uint16 size );

#endif
