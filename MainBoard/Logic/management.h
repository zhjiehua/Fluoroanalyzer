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

//采集缓冲区大小
#define SAMPLE_BUFFER_SIZE 			400  //400
#define MAXIMAL_BUFFER_SIZE 		100

//字符串长度
#define PROJECT_LEN					20
#define BATCH_LEN					20
#define SAMPLE_LEN					20
#define INVALIDEDATA_LEN		10
#define CONCENUNIT_LEN		10

//定义EEPROM地址功能
#define POWERONTEST_BASEADDR		0
//内部参数页面	
#define SAMPLESTART_BASEADDR		(POWERONTEST_BASEADDR+4)
#define SAMPLESTOP_BASEADDR			(SAMPLESTART_BASEADDR+4)
//设置页面
#define QRCODE_EN_BASEADDR			(SAMPLESTOP_BASEADDR+4)
#define PRINTER_EN_BASEADDR			(QRCODE_EN_BASEADDR+1)
#define ALARM_EN_BASEADDR			(PRINTER_EN_BASEADDR+1)
#define BEEPER_EN_BASEADDR			(ALARM_EN_BASEADDR+1)
//字符串参数
#define PROJECT_STRING_BASEADDR		(BEEPER_EN_BASEADDR+1)
#define BATCH_STRING_BASEADDR		(PROJECT_STRING_BASEADDR+PROJECT_LEN)
#define SAMPLE_STRING_BASEADDR		(BATCH_STRING_BASEADDR+BATCH_LEN) 
#define INVALIDEDATA_STRING_BASEADDR		(SAMPLE_STRING_BASEADDR+SAMPLE_LEN)
#define CONCENUNIT_STRING_BASEADDR		(INVALIDEDATA_STRING_BASEADDR+INVALIDEDATA_LEN)

//CT线位置
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
#define USABLETIMES_BASEADDR		(TOTALOUTPUT_BASEADDR+4)//可用次数不能更新
#define RESTOREDEFAULT_BASEADDR		(USABLETIMES_BASEADDR+4)

#define CALIBFACTOR_BASEADDR		(RESTOREDEFAULT_BASEADDR+4)

#define EEPROM_ENDADDR				(CALIBFACTOR_BASEADDR+4)

//sampleStatus MASK
#define SAMPLESTATUS_HOME			0x01
#define SAMPLESTATUS_READY			0x02
#define SAMPLESTATUS_SAMPLE			0x04

/************************************************************************/
/* 项目管理结构体                                                       */
/************************************************************************/
typedef struct
{
	uint8_t projectStatus; //bit7表示测试程序正在运行，bit6~bit0对应项目程序代号

	uint8_t page;
	uint8_t scanInputFlag; //二维码扫描标志。0：样品编号；1：批次编号；2：项目编号。
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
	
	uint8_t rtc[20]; //检测日期和时间
	float result; //检测结果

//==================================================	
	uint8_t projectString[PROJECT_LEN];//检测项目
	uint8_t batchString[BATCH_LEN];//批号
	uint8_t sampleString[SAMPLE_LEN];//样品ID
	
	uint8_t invalideData[INVALIDEDATA_LEN];//失效日期
	
	uint8_t lineNumber; //检测线数，一般都是2线（C线和T线）
	uint16_t axisCX; //C线位置
	uint16_t axisTX; //T线位置
	uint8_t checkWindowAxisX; //检测窗口，CT线如果在理论值窗口内，则不报错
	
	uint8_t sampleType; //样本类型
	uint8_t concentrationUnit[CONCENUNIT_LEN]; //浓度单位
	
	float checkRangeLow;
	float checkRangeHigh; //检测范围
	float referRangeLow;
	float referRangeHigh; //参考范围
	
	uint8_t fittingType; //标定方式 T、T/C、T/(T+C)
	uint8_t fittingModel; //拟合模型
	float fittingPara1; //拟合模型参数
	float fittingPara2;
	float fittingPara3;
	float fittingPara4;
//==================================================	
	//
	
	uint8_t projectMode; //模式，
	uint8_t smStopFlag; //步进电机停止标志位
	
	uint8_t sampleState;//采集用状态机
	uint8_t sampleStatus; //采集状态
	uint8_t sampleHomeSpeed; //回原点速度
	uint8_t sampleGoReadySpeed; //准备到达起始点时的速度
	uint8_t sampleSpeed; //采集速度
	
	uint8_t qrCodeEnable; //
	uint8_t printerEnable; //是否自动打印
	uint8_t alarmEnable; //
	uint8_t beeperEnable; //
	float	calibFactor;//增益校准
	
	uint32_t sampleStartPoint;
	uint32_t sampleStopPoint;
	
	uint32_t sampleData[SAMPLE_BUFFER_SIZE];
	uint16_t sampleDataCount;
	
	uint32_t filterData[SAMPLE_BUFFER_SIZE];
	uint8_t curveData[350];
	uint16_t maximumBufferIndex; //最大值下标
	
	uint32_t peakBuffer[200]; //峰缓冲区，求cValue和tValue时用到
	uint8_t peakBufferCount;
	
	uint16_t axisTXCur; //当前T线位置
	uint16_t axisCXCur;	//当前C线位置
	
	double LeastSquareA, LeastSquareB;//最小二乘法参数 ax+b
	
	uint32_t cThreshod;
	uint32_t tThreshod;
	uint32_t cValue;
	uint32_t tValue;
	float tcValue;
	uint32_t gender;//0：无效；1：阳性；2：阴性
	float concentration;//浓度
	
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
