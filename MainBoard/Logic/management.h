#ifndef _MANAGEMENT_H_
#define _MANAGEMENT_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"
#include "project.h"
#include "../UILogic/pageCommon.h"
#include "../HMI/cmd_queue.h"
#include "../HMI/cmd_process.h"
#include "../HMI/hmi_user_uart.h"
#include "../PID/PID.h"
	
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"
#include "queue.h"
#include "semphr.h"
#include "event_groups.h"

//�ɼ���������С
#define SAMPLE_BUFFER_SIZE 			400  //400
#define MAXIMAL_BUFFER_SIZE 		100

//�ַ�������
#define PROJECT_LEN					20
#define BATCH_LEN					20
#define SAMPLE_LEN					20
#define INVALIDEDATA_LEN		10
#define CONCENUNIT_LEN		10

//����EEPROM��ַ����
#define POWERONTEST_BASEADDR		0
//�ڲ�����ҳ��	
#define SAMPLESTART_BASEADDR		(POWERONTEST_BASEADDR+4)
#define SAMPLESTOP_BASEADDR			(SAMPLESTART_BASEADDR+4)
//����ҳ��
#define QRCODE_EN_BASEADDR			(SAMPLESTOP_BASEADDR+4)
#define PRINTER_EN_BASEADDR			(QRCODE_EN_BASEADDR+1)
#define ALARM_EN_BASEADDR			(PRINTER_EN_BASEADDR+1)
#define BEEPER_EN_BASEADDR			(ALARM_EN_BASEADDR+1)
//�ַ�������
#define PROJECT_STRING_BASEADDR		(BEEPER_EN_BASEADDR+1)
#define BATCH_STRING_BASEADDR		(PROJECT_STRING_BASEADDR+PROJECT_LEN)
#define SAMPLE_STRING_BASEADDR		(BATCH_STRING_BASEADDR+BATCH_LEN) 
#define INVALIDEDATA_STRING_BASEADDR		(SAMPLE_STRING_BASEADDR+SAMPLE_LEN)
#define CONCENUNIT_STRING_BASEADDR		(INVALIDEDATA_STRING_BASEADDR+INVALIDEDATA_LEN)

//CT��λ��
#define SCANLINE_NUM_BASEADDR			(CONCENUNIT_STRING_BASEADDR+CONCENUNIT_LEN)
#define AXIS_CX_BASEADDR			(SCANLINE_NUM_BASEADDR+1)
#define AXIS_TX_BASEADDR			(AXIS_CX_BASEADDR+2)
#define CHECKWINDOW_BASEADDR			(AXIS_TX_BASEADDR+2)
#define SAMPLETYPE_BASEADDR			(CHECKWINDOW_BASEADDR+1)
#define CHECKRANGELOW_BASEADDR			(SAMPLETYPE_BASEADDR+1)
#define CHECKRANGEHIGH_BASEADDR			(CHECKRANGELOW_BASEADDR+sizeof(float))
#define REFERRANGELOW_BASEADDR			(CHECKRANGEHIGH_BASEADDR+sizeof(float))
#define REFERRANGEHIGH_BASEADDR			(REFERRANGELOW_BASEADDR+sizeof(float))
#define FITTINGTYPE_BASEADDR			(REFERRANGEHIGH_BASEADDR+sizeof(float))
#define FITTINGMODEL_BASEADDR			(FITTINGTYPE_BASEADDR+1)
#define FITTINGPARA1_BASEADDR			(FITTINGMODEL_BASEADDR+1)
#define FITTINGPARA2_BASEADDR			(FITTINGPARA1_BASEADDR+sizeof(float))
#define FITTINGPARA3_BASEADDR			(FITTINGPARA2_BASEADDR+sizeof(float))	
#define FITTINGPARA4_BASEADDR			(FITTINGPARA3_BASEADDR+sizeof(float))	
	
//
#define TOTALOUTPUT_BASEADDR		(FITTINGPARA4_BASEADDR+sizeof(float))
#define USABLETIMES_BASEADDR		(TOTALOUTPUT_BASEADDR+4)//���ô������ܸ���
#define RESTOREDEFAULT_BASEADDR		(USABLETIMES_BASEADDR+4)

#define CALIBFACTOR_BASEADDR		(RESTOREDEFAULT_BASEADDR+4)

#define EEPROM_ENDADDR				(CALIBFACTOR_BASEADDR+4)

//sampleStatus MASK
#define SAMPLESTATUS_HOME			0x01
#define SAMPLESTATUS_READY			0x02
#define SAMPLESTATUS_SAMPLE			0x04

/************************************************************************/
/* ��Ŀ����ṹ��                                                       */
/************************************************************************/
typedef struct
{
	uint8_t projectStatus; //bit7��ʾ���Գ����������У�bit6~bit0��Ӧ��Ŀ�������

	uint8_t page;
	uint8_t scanInputFlag; //��ά��ɨ���־��0����Ʒ��ţ�1�����α�ţ�2����Ŀ��š�
	SemaphoreHandle_t lcdUartSem;	

	uint32_t totalOutput;
	uint8_t inInputPageFlag;
	uint8_t inStatusPageFlag;
	uint8_t lcdNotifyResetFlag;
	
	uint32_t chipID[3];
	uint32_t serial;
	uint32_t randomCode;
	uint32_t activeCode;
	uint16_t usableTimes;
	
	uint8_t rtc[20]; //������ں�ʱ��
	float result; //�����

//==================================================	
	uint8_t projectString[PROJECT_LEN];//�����Ŀ
	uint8_t batchString[BATCH_LEN];//����
	uint8_t sampleString[SAMPLE_LEN];//��ƷID
	
	uint8_t invalideData[INVALIDEDATA_LEN];//ʧЧ����
	
	uint8_t lineNumber; //���������һ�㶼��2�ߣ�C�ߺ�T�ߣ�
	uint16_t axisCX; //C��λ��
	uint16_t axisTX; //T��λ��
	uint8_t checkWindowAxisX; //��ⴰ�ڣ�CT�����������ֵ�����ڣ��򲻱���
	
	uint8_t sampleType; //��������
	uint8_t concentrationUnit[CONCENUNIT_LEN]; //Ũ�ȵ�λ
	
	float checkRangeLow;
	float checkRangeHigh; //��ⷶΧ
	float referRangeLow;
	float referRangeHigh; //�ο���Χ
	
	uint8_t fittingType; //�궨��ʽ T��T/C��T/(T+C)
	uint8_t fittingModel; //���ģ��
	float fittingPara1; //���ģ�Ͳ���
	float fittingPara2;
	float fittingPara3;
	float fittingPara4;
//==================================================	
	//
	
	uint8_t projectMode; //ģʽ��
	uint8_t smStopFlag; //�������ֹͣ��־λ
	
	uint8_t sampleState;//�ɼ���״̬��
	uint8_t sampleStatus; //�ɼ�״̬
	uint8_t sampleHomeSpeed; //��ԭ���ٶ�
	uint8_t sampleGoReadySpeed; //׼��������ʼ��ʱ���ٶ�
	uint8_t sampleSpeed; //�ɼ��ٶ�
	
	uint8_t qrCodeEnable; //
	uint8_t printerEnable; //�Ƿ��Զ���ӡ
	uint8_t alarmEnable; //
	uint8_t beeperEnable; //
	float	calibFactor;//����У׼
	
	uint32_t sampleStartPoint;
	uint32_t sampleStopPoint;
	
	uint32_t sampleData[SAMPLE_BUFFER_SIZE];
	uint16_t sampleDataCount;
	
	uint32_t filterData[SAMPLE_BUFFER_SIZE];
	uint8_t curveData[350];
	uint16_t maximumBufferIndex; //���ֵ�±�
	
	uint32_t peakBuffer[200]; //�建��������cValue��tValueʱ�õ�
	uint8_t peakBufferCount;
	
	uint16_t axisTXCur; //��ǰT��λ��
	uint16_t axisCXCur;	//��ǰC��λ��
	
	double LeastSquareA, LeastSquareB;//��С���˷����� ax+b
	
	uint32_t cThreshod;
	uint32_t tThreshod;
	uint32_t cValue;
	uint32_t tValue;
	float tcValue;
	uint32_t gender;//0����Ч��1�����ԣ�2������
	float concentration;//Ũ��
	
	uint16_t curveXOffset;
	uint16_t curveYOffset;
	uint16_t curveXMul;
	uint16_t curveYMul;
}ProjectMan_TypeDef;

/************************************************************************/
/*                                                                      */
/************************************************************************/

extern ProjectMan_TypeDef *pProjectMan;

extern uint8 hmi_cmd_buffer[];

void InitUI(void);

void ResultPrint(void);

void Sample_Home(void);
void Sample_GoReady(void);
void Sample(void);

void UITask(void);
	
#ifdef __cplusplus
}
#endif

#endif
