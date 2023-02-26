#include "management.h"
#include "stddef.h"
#include "string.h"
#include "stdio.h"
#include "stdint.h"
#include "CPrintf.h"
#include "../HARDWARE/StepMotor/stepmotor.h"
#include "../HARDWARE/Beep/beep.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/WDG/wdg.h"
#include "../HARDWARE/button/button.h"
#include "../HMI/hmi_driver.h"
#include "../HMI/hmi_user_uart.h"
#include "../HMI/cmd_queue.h"
#include "LED/led.h"
#include "RelayMOS/RelayMOS.h"
#include "../PID/PID.h"
#include "miscellaneous.h"
#include "QRCode/QRCode.h"
#include "..\HARDWARE\Printer\printer.h"
#include "..\HARDWARE\Printer\DP_Print_inc.h"

#include "usb_lib.h"
#include "hw_config.h"
#include "usb_pwr.h"
#include "delay.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif
	
/************************************************************************/
/* 定义项目管理结构体                                                   */
/************************************************************************/
static ProjectMan_TypeDef ProjectMan;
ProjectMan_TypeDef *pProjectMan = &ProjectMan;

/************************************************************************/
/* 串口接收缓冲区                                                       */
/************************************************************************/
#define HMI_CMD_MAX_SIZE 200 //64        /*!<单条指令大小，根据需要调整，尽量设置大一些*/
#define DDC112_CMD_MAX_SIZE 5 //64        /*!<单条指令大小，根据需要调整，尽量设置大一些*/
#define USER_CMD_MAX_SIZE 10 //64        /*!<单条指令大小，根据需要调整，尽量设置大一些*/
#define USB_CMD_MAX_SIZE 20
	
uint8_t hmi_cmd_buffer[HMI_CMD_MAX_SIZE];
uint8_t ddc112_cmd_buffer[DDC112_CMD_MAX_SIZE];	
uint8_t user_cmd_buffer[USER_CMD_MAX_SIZE];	
uint8_t usb_cmd_buffer[USB_CMD_MAX_SIZE];	
	
#define EEPROM_DEFAULT 0x11223344

void InitUI(void)
{
	//初始化项目结构体
//	uint16_t i;
//	uint32_t j;
	uint32_t dat;

  HMIUartInit(115200);
	
	GetChipID(pProjectMan->chipID);
	pProjectMan->serial = pProjectMan->chipID[0]+pProjectMan->chipID[1]+pProjectMan->chipID[2];
	srand(SysTick->VAL);
	pProjectMan->randomCode = rand();
	
#if 1    
	AT24CXX_Read(POWERONTEST_BASEADDR, (uint8_t*)(&dat), sizeof(uint32_t));//是否第一次开机，读3次
	if(dat != EEPROM_DEFAULT)
		AT24CXX_Read(POWERONTEST_BASEADDR, (uint8_t*)(&dat), sizeof(uint32_t));
	if(dat != EEPROM_DEFAULT)
		AT24CXX_Read(POWERONTEST_BASEADDR, (uint8_t*)(&dat), sizeof(uint32_t));

	if(dat != EEPROM_DEFAULT) //是，初始化EEPROM中的数据
	{
		cDebug("Reset default data!\r\n");
		
		pProjectMan->totalOutput = 0;		
		pProjectMan->usableTimes = 1;
		
		pProjectMan->qrCodeEnable = 1;
		pProjectMan->printerEnable = 1;
		pProjectMan->alarmEnable = 1;
		pProjectMan->beeperEnable = 1;
		
		pProjectMan->sampleStartPoint = 44800;
		pProjectMan->sampleStopPoint = 12150;		

		pProjectMan->calibFactor = 1.0;
		
#define PROJECT_STRING_DEFAULT	"project01"
#define BATCH_STRING_DEFAULT	"batch01"
#define SAMPLE_STRING_DEFAULT	"sample01"
#define CONCENUNIT_STRING_DEFAULT	"ug/g"
		
		sprintf((char*)pProjectMan->projectString, "%s%c", PROJECT_STRING_DEFAULT, '\0');
		sprintf((char*)pProjectMan->batchString, "%s%c", BATCH_STRING_DEFAULT, '\0');
		//sprintf((char*)pProjectMan->sampleString, "%s%c", SAMPLE_STRING_DEFAULT, '\0');
		sprintf((char*)pProjectMan->concentrationUnit, "%s%c", CONCENUNIT_STRING_DEFAULT, '\0');
		
		pProjectMan->lineNumber = 2;
		pProjectMan->axisCX = 78;
		pProjectMan->axisTX = 255;
		pProjectMan->checkWindowAxisX = 10;
		pProjectMan->sampleType = 3;
		
		pProjectMan->checkRangeLow = 0.20;
		pProjectMan->checkRangeHigh = 5.60;
		pProjectMan->referRangeLow = 1.20;
		pProjectMan->referRangeHigh = 3.50;
		
		pProjectMan->fittingType = 1;
//		pProjectMan->fittingModel = 4;
//		pProjectMan->fittingPara1 = 100;
//		pProjectMan->fittingPara2 = 100;
//		pProjectMan->fittingPara3 = 100;
//		pProjectMan->fittingPara4 = 100;
		pProjectMan->fittingModel = 0;
		pProjectMan->fittingPara1 = 1;
		pProjectMan->fittingPara2 = 0;
		pProjectMan->fittingPara3 = 100;
		pProjectMan->fittingPara4 = 100;
		
#ifndef STM32SIM
		//保存数据		
		AT24CXX_Write(TOTALOUTPUT_BASEADDR, (uint8_t*)&pProjectMan->totalOutput, 4);		
		AT24CXX_Write(USABLETIMES_BASEADDR, (uint8_t*)&pProjectMan->usableTimes, 2);
		
		AT24CXX_Write(QRCODE_EN_BASEADDR, (uint8_t*)&pProjectMan->qrCodeEnable, 1);
		AT24CXX_Write(PRINTER_EN_BASEADDR, (uint8_t*)&pProjectMan->printerEnable, 1);
		AT24CXX_Write(ALARM_EN_BASEADDR, (uint8_t*)&pProjectMan->alarmEnable, 1);
		AT24CXX_Write(BEEPER_EN_BASEADDR, (uint8_t*)&pProjectMan->beeperEnable, 1);
		
		AT24CXX_Write(SAMPLESTART_BASEADDR, (uint8_t*)&pProjectMan->sampleStartPoint, 4);
		AT24CXX_Write(SAMPLESTOP_BASEADDR, (uint8_t*)&pProjectMan->sampleStopPoint, 4);		
		
		AT24CXX_Write(CALIBFACTOR_BASEADDR, (uint8_t*)&pProjectMan->calibFactor, 4);
		
		AT24CXX_Write(PROJECT_STRING_BASEADDR, (uint8_t*)pProjectMan->projectString, strlen((const char*)pProjectMan->projectString)+1);
		AT24CXX_Write(BATCH_STRING_BASEADDR, (uint8_t*)pProjectMan->batchString, strlen((const char*)pProjectMan->batchString)+1);
		AT24CXX_Write(SAMPLE_STRING_BASEADDR, (uint8_t*)pProjectMan->sampleString, strlen((const char*)pProjectMan->sampleString)+1);
		AT24CXX_Write(CONCENUNIT_STRING_BASEADDR, (uint8_t*)pProjectMan->concentrationUnit, strlen((const char*)pProjectMan->concentrationUnit)+1);
		AT24CXX_Write(INVALIDEDATA_STRING_BASEADDR, (uint8_t*)pProjectMan->invalideData, strlen((const char*)pProjectMan->invalideData)+1);
		
		AT24CXX_Write(SCANLINE_NUM_BASEADDR, (uint8_t*)&pProjectMan->lineNumber, sizeof(pProjectMan->lineNumber));
		AT24CXX_Write(AXIS_CX_BASEADDR, (uint8_t*)&pProjectMan->axisCX, sizeof(pProjectMan->axisCX));
		AT24CXX_Write(AXIS_TX_BASEADDR, (uint8_t*)&pProjectMan->axisTX, sizeof(pProjectMan->axisTX));
		AT24CXX_Write(CHECKWINDOW_BASEADDR, (uint8_t*)&pProjectMan->checkWindowAxisX, sizeof(pProjectMan->checkWindowAxisX));
		AT24CXX_Write(SAMPLETYPE_BASEADDR, (uint8_t*)&pProjectMan->sampleType, sizeof(pProjectMan->sampleType));
		AT24CXX_Write(CHECKRANGELOW_BASEADDR, (uint8_t*)&pProjectMan->checkRangeLow, sizeof(pProjectMan->checkRangeLow));
		AT24CXX_Write(CHECKRANGEHIGH_BASEADDR, (uint8_t*)&pProjectMan->checkRangeHigh, sizeof(pProjectMan->checkRangeHigh));
		AT24CXX_Write(REFERRANGELOW_BASEADDR, (uint8_t*)&pProjectMan->referRangeLow, sizeof(pProjectMan->referRangeLow));
		AT24CXX_Write(REFERRANGEHIGH_BASEADDR, (uint8_t*)&pProjectMan->referRangeHigh, sizeof(pProjectMan->referRangeHigh));
		AT24CXX_Write(FITTINGTYPE_BASEADDR, (uint8_t*)&pProjectMan->fittingType, sizeof(pProjectMan->fittingType));
		AT24CXX_Write(FITTINGMODEL_BASEADDR, (uint8_t*)&pProjectMan->fittingModel, sizeof(pProjectMan->fittingModel));
		AT24CXX_Write(FITTINGPARA1_BASEADDR, (uint8_t*)&pProjectMan->fittingPara1, sizeof(pProjectMan->fittingPara1));
		AT24CXX_Write(FITTINGPARA2_BASEADDR, (uint8_t*)&pProjectMan->fittingPara2, sizeof(pProjectMan->fittingPara2));
		AT24CXX_Write(FITTINGPARA3_BASEADDR, (uint8_t*)&pProjectMan->fittingPara3, sizeof(pProjectMan->fittingPara3));
		AT24CXX_Write(FITTINGPARA4_BASEADDR, (uint8_t*)&pProjectMan->fittingPara4, sizeof(pProjectMan->fittingPara4));
		
		dat = EEPROM_DEFAULT;
		AT24CXX_Write(POWERONTEST_BASEADDR, (uint8_t*)&dat, sizeof(uint32_t));
#endif

		pProjectMan->page = LOGOPAGE_INDEX;//跳转到LOGO页面
		SetScreen(pProjectMan->page);
		
		//二维码模块初始化
		//QRCodeModelInit();
	}
	else //否，从EEPROM中读取数据
	{
		cDebug("Reading data from EEPROM……\r\n");

		//vTaskDelay(1000);
		
//#ifndef STM32SIM
#if 1
		//读取参数		
		AT24CXX_Read(TOTALOUTPUT_BASEADDR, (uint8_t*)&pProjectMan->totalOutput, 4);		
		AT24CXX_Read(USABLETIMES_BASEADDR, (uint8_t*)&pProjectMan->usableTimes, 2);
		
		AT24CXX_Read(QRCODE_EN_BASEADDR, (uint8_t*)&pProjectMan->qrCodeEnable, 1);
		AT24CXX_Read(PRINTER_EN_BASEADDR, (uint8_t*)&pProjectMan->printerEnable, 1);
		AT24CXX_Read(ALARM_EN_BASEADDR, (uint8_t*)&pProjectMan->alarmEnable, 1);
		AT24CXX_Read(BEEPER_EN_BASEADDR, (uint8_t*)&pProjectMan->beeperEnable, 1);
		
		AT24CXX_Read(SAMPLESTART_BASEADDR, (uint8_t*)&pProjectMan->sampleStartPoint, 4);
		AT24CXX_Read(SAMPLESTOP_BASEADDR, (uint8_t*)&pProjectMan->sampleStopPoint, 4);

		AT24CXX_Read(CALIBFACTOR_BASEADDR, (uint8_t*)&pProjectMan->calibFactor, 4);
		
		AT24CXX_Read(PROJECT_STRING_BASEADDR, (uint8_t*)pProjectMan->projectString, PROJECT_LEN);
		AT24CXX_Read(BATCH_STRING_BASEADDR, (uint8_t*)pProjectMan->batchString, BATCH_LEN);
		//AT24CXX_Read(SAMPLE_STRING_BASEADDR, (uint8_t*)pProjectMan->sampleString, SAMPLE_LEN);
		AT24CXX_Read(CONCENUNIT_STRING_BASEADDR, (uint8_t*)pProjectMan->concentrationUnit, CONCENUNIT_LEN);
		AT24CXX_Read(INVALIDEDATA_STRING_BASEADDR, (uint8_t*)pProjectMan->invalideData, INVALIDEDATA_LEN);
		
		AT24CXX_Read(SCANLINE_NUM_BASEADDR, (uint8_t*)&pProjectMan->lineNumber, sizeof(pProjectMan->lineNumber));
		AT24CXX_Read(AXIS_CX_BASEADDR, (uint8_t*)&pProjectMan->axisCX, sizeof(pProjectMan->axisCX));
		AT24CXX_Read(AXIS_TX_BASEADDR, (uint8_t*)&pProjectMan->axisTX, sizeof(pProjectMan->axisTX));
		AT24CXX_Read(CHECKWINDOW_BASEADDR, (uint8_t*)&pProjectMan->checkWindowAxisX, sizeof(pProjectMan->checkWindowAxisX));
		AT24CXX_Read(SAMPLETYPE_BASEADDR, (uint8_t*)&pProjectMan->sampleType, sizeof(pProjectMan->sampleType));
		AT24CXX_Read(CHECKRANGELOW_BASEADDR, (uint8_t*)&pProjectMan->checkRangeLow, sizeof(pProjectMan->checkRangeLow));
		AT24CXX_Read(CHECKRANGEHIGH_BASEADDR, (uint8_t*)&pProjectMan->checkRangeHigh, sizeof(pProjectMan->checkRangeHigh));
		AT24CXX_Read(REFERRANGELOW_BASEADDR, (uint8_t*)&pProjectMan->referRangeLow, sizeof(pProjectMan->referRangeLow));
		AT24CXX_Read(REFERRANGEHIGH_BASEADDR, (uint8_t*)&pProjectMan->referRangeHigh, sizeof(pProjectMan->referRangeHigh));
		AT24CXX_Read(FITTINGTYPE_BASEADDR, (uint8_t*)&pProjectMan->fittingType, sizeof(pProjectMan->fittingType));
		AT24CXX_Read(FITTINGMODEL_BASEADDR, (uint8_t*)&pProjectMan->fittingModel, sizeof(pProjectMan->fittingModel));
		AT24CXX_Read(FITTINGPARA1_BASEADDR, (uint8_t*)&pProjectMan->fittingPara1, sizeof(pProjectMan->fittingPara1));
		AT24CXX_Read(FITTINGPARA2_BASEADDR, (uint8_t*)&pProjectMan->fittingPara2, sizeof(pProjectMan->fittingPara2));
		AT24CXX_Read(FITTINGPARA3_BASEADDR, (uint8_t*)&pProjectMan->fittingPara3, sizeof(pProjectMan->fittingPara3));
		AT24CXX_Read(FITTINGPARA4_BASEADDR, (uint8_t*)&pProjectMan->fittingPara4, sizeof(pProjectMan->fittingPara4));
#endif
	}
#endif
	
	/*清空串口接收缓冲区*/
	queue_init(&hmi_que, hmi_que_buffer, HMI_QUEUE_MAX_SIZE);	
	queue_init(&ddc112_que, ddc112_que_buffer, DDC112_QUEUE_MAX_SIZE);
	queue_init(&user_que, user_que_buffer, USER_QUEUE_MAX_SIZE);
	queue_init(&usb_que, usb_que_buffer, USB_QUEUE_MAX_SIZE);
	
	pProjectMan->sampleSpeed = 0;
	pProjectMan->sampleGoReadySpeed = 5; //5
	pProjectMan->sampleHomeSpeed = 5;  //5
	
	pProjectMan->scanInputFlag = 0;
	
	pProjectMan->curveXOffset = 0;
	pProjectMan->curveYOffset = 0;
	pProjectMan->curveXMul = 100;
	pProjectMan->curveYMul = 100;
	
//	j = 0;
//	for(i=0;i<350;i++)
//	{
//		if(i>350/2)
//			j--;
//		else
//			j++;
//		pProjectMan->curveData[i] = j*180/175;
//	}
	
	cDebug("pProjectMan->totalOutput = %d\r\n", pProjectMan->totalOutput);
	cDebug("pProjectMan->usableTimes = %d\r\n", pProjectMan->usableTimes);
	
	cDebug("pProjectMan->qrCodeEnable = %d\r\n", pProjectMan->qrCodeEnable);
	cDebug("pProjectMan->printerEnable = %d\r\n", pProjectMan->printerEnable);
	cDebug("pProjectMan->alarmEnable = %d\r\n", pProjectMan->alarmEnable);
	cDebug("pProjectMan->beeperEnable = %d\r\n", pProjectMan->beeperEnable);
	
	cDebug("pProjectMan->sampleStartPoint = %d\r\n", pProjectMan->sampleStartPoint);
	cDebug("pProjectMan->sampleStopPoint = %d\r\n", pProjectMan->sampleStopPoint);
	
	cDebug("pProjectMan->calibFactor = %f\r\n", pProjectMan->calibFactor);
	
	cDebug("pProjectMan->projectString = %s\r\n", pProjectMan->projectString);
	cDebug("pProjectMan->batchString = %s\r\n", pProjectMan->batchString);
	cDebug("pProjectMan->sampleString = %s\r\n", pProjectMan->sampleString);
	cDebug("pProjectMan->invalideData = %s\r\n", pProjectMan->invalideData);
	cDebug("pProjectMan->concentrationUnit = %s\r\n", pProjectMan->concentrationUnit);
	
	cDebug("pProjectMan->lineNumber = %d\r\n", pProjectMan->lineNumber);
	cDebug("pProjectMan->axisCX = %d\r\n", pProjectMan->axisCX);
	cDebug("pProjectMan->axisTX = %d\r\n", pProjectMan->axisTX);
	cDebug("pProjectMan->checkWindowAxisX = %d\r\n", pProjectMan->checkWindowAxisX);
	cDebug("pProjectMan->sampleType = %d\r\n", pProjectMan->sampleType);
	cDebug("pProjectMan->checkRangeLow = %f\r\n", pProjectMan->checkRangeLow);
	cDebug("pProjectMan->checkRangeHigh = %f\r\n", pProjectMan->checkRangeHigh);
	cDebug("pProjectMan->referRangeLow = %f\r\n", pProjectMan->referRangeLow);
	cDebug("pProjectMan->referRangeHigh = %f\r\n", pProjectMan->referRangeHigh);
	
	cDebug("pProjectMan->fittingType = %d\r\n", pProjectMan->fittingType);
	cDebug("pProjectMan->fittingModel = %d\r\n", pProjectMan->fittingModel);
	cDebug("pProjectMan->fittingPara1 = %f\r\n", pProjectMan->fittingPara1);
	cDebug("pProjectMan->fittingPara2 = %f\r\n", pProjectMan->fittingPara2);
	cDebug("pProjectMan->fittingPara3 = %f\r\n", pProjectMan->fittingPara3);
	cDebug("pProjectMan->fittingPara4 = %f\r\n", pProjectMan->fittingPara4);
	
	cDebug("EEPROM_ENDADDR = %d\r\n", EEPROM_ENDADDR);
	
	cDebug("\r\nInitUI success\r\n");
}

void ResultPrint(void)
{
	char string[50];

#if 0

	Printer_ChineseModeSetting();
	Printer_DefaultLineSpaceSetting();
	Printer_PrintBottonUpSetting(0); //正序
	
	Printer_AlignTypeSelect(1);//中间对齐
	Printer_SendStrings((uint8_t*)"荧光免疫层析检验报告单");
	Printer_PrintLF();
	
	Printer_AlignTypeSelect(0);//左对齐

	sprintf(string, "批号：%s ", pProjectMan->batchString);
	Printer_SendStrings((uint8_t*)string);
	Printer_PrintLF();
	
	sprintf(string, "样品ID：%s ", pProjectMan->sampleString);
	Printer_SendStrings((uint8_t*)string);
	Printer_PrintLF();
	
	sprintf(string, "检测项目：%s ", pProjectMan->projectString);
	Printer_SendStrings((uint8_t*)string);
	Printer_PrintLF();
	
	sprintf(string, "检测结果：%1.4f ", pProjectMan->concentration);
	Printer_SendStrings((uint8_t*)string);
	if(pProjectMan->gender == 1)
		Printer_SendStrings((uint8_t*)"阳性");
	else
		Printer_SendStrings((uint8_t*)"阴性");
	Printer_PrintLF();
	
	sprintf(string, "参考范围：%1.4f-%1.4f", pProjectMan->referRangeLow, pProjectMan->referRangeHigh);
	Printer_SendStrings((uint8_t*)string);
	Printer_PrintLF();
	
	Printer_SendStrings((uint8_t*)"检测时间：");
	Printer_SendStrings((uint8_t*)(pProjectMan->rtc));
	Printer_PrintLF();
	
	Printer_PrintPaperSkipNLine(7);

#else // 广州优库电子 EM5822
    InitializePrint();

    TestPrintPage();return;


//    {
//        unsigned char optbit;
//        EnableASCII9X17(optbit);
//        Set_Print_Mode(optbit);
//    }
//    SetDefaultLineInterval();
//    SelCountryCharacter(15);
//    SelChineseChar();
//    Set_ChineseCode(1);

    Sel_Align_Way(1); //中间对齐
    Print_ASCII((uint8_t*)"Hello Printer EM5822");
    print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
    return;

    Sel_Align_Way(1); //中间对齐
    Print_ASCII((uint8_t*)"荧光免疫层析检验报告单");
    print_And_Line();

    Sel_Align_Way(0); //左对齐

    sprintf(string, "批号：%s ", pProjectMan->batchString);
	Print_ASCII((uint8_t*)string);
	Printer_PrintLF();
	
	sprintf(string, "样品ID：%s ", pProjectMan->sampleString);
	Print_ASCII((uint8_t*)string);
	print_And_Line();
	
	sprintf(string, "检测项目：%s ", pProjectMan->projectString);
	Print_ASCII((uint8_t*)string);
	print_And_Line();
	
	sprintf(string, "检测结果：%1.4f ", pProjectMan->concentration);
	Print_ASCII((uint8_t*)string);
	if(pProjectMan->gender == 1)
		Print_ASCII((uint8_t*)"阳性");
	else
		Print_ASCII((uint8_t*)"阴性");
	print_And_Line();
	
	sprintf(string, "参考范围：%1.4f-%1.4f", pProjectMan->referRangeLow, pProjectMan->referRangeHigh);
	Print_ASCII((uint8_t*)string);
	print_And_Line();
	
	Print_ASCII((uint8_t*)"检测时间：");
	Print_ASCII((uint8_t*)(pProjectMan->rtc));
	print_And_Line();
	
	print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
    print_And_Line();
#endif
}

void Sample_Home(void)
{
#if 0
	switch(pProjectMan->sampleState)
	{
		case 0:
			if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)) //在原点
			{
				pProjectMan->sampleStatus = 0;
				pProjectMan->sampleStatus |= SAMPLESTATUS_HOME;
			}
			else //回原点
			{
				StepMotor_SetSpeed(0, pProjectMan->sampleHomeSpeed);
				StepMotor_SetDir(0, CCW);
				StepMotor_SetCMD(0, ENABLE);
				
				pProjectMan->sampleState = 1;
			}
		break;
		case 1:
			if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13))
			{
				StepMotor_Stop(0);
				pProjectMan->sampleState = 0;
				
				pProjectMan->sampleStatus = 0;
				pProjectMan->sampleStatus |= SAMPLESTATUS_HOME;
			}
		break;
		default:
		break;
	}
#endif
	
	if(!GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13)) //在原点
    {
        cDebug("It is in home!");
		return;
    }
	else //回原点
	{
        cDebug("Start home...");
		StepMotor_SetSpeed(0, pProjectMan->sampleHomeSpeed);
		StepMotor_SetDir(0, CCW);
		StepMotor_SetCMD(0, ENABLE);
		
		while(GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_13) && !pProjectMan->smStopFlag)
			vTaskDelay(10);
		StepMotor_Stop(0);
        cDebug("Start home...finish!");
	}
	
	cDebug("Home finish!\r\n");
}

void Sample_GoReady(void)
{
#if 0
	switch(pProjectMan->sampleState)
	{
		case 0:
			StepMotor_SetSpeed(0, pProjectMan->sampleGoReadySpeed);
			StepMotor_SetDir(0, CW);
			StepMotor_SetPluse(0, pProjectMan->sampleStartPoint);
			StepMotor_SetCMD(0, ENABLE);
			
			pProjectMan->sampleState = 1;
		break;
		case 1:
			if(StepMotor_IsStop(0))
			{
				pProjectMan->sampleState = 0;
				pProjectMan->sampleStatus |= SAMPLESTATUS_READY;
				pProjectMan->sampleStatus &= ~SAMPLESTATUS_HOME;
			}
		break;
		default:
		break;
	}
#endif
	
	StepMotor_SetSpeed(0, pProjectMan->sampleGoReadySpeed);
	StepMotor_SetDir(0, CW);
	StepMotor_SetPluse(0, pProjectMan->sampleStartPoint);
	StepMotor_SetCMD(0, ENABLE);
	
	while(!StepMotor_IsStop(0) && !pProjectMan->smStopFlag)
		vTaskDelay(10);
	
	cDebug("GoReady finish!\r\n");
}

void Sample(void)
{
#if 0
	switch(pProjectMan->sampleState)
	{
		case 0:
			StepMotor_SetSpeed(0, pProjectMan->sampleSpeed);
			StepMotor_SetDir(0, CW);
			StepMotor_SetPluse(0, pProjectMan->sampleStopPoint);
			StepMotor_SetCMD(0, ENABLE);
			
			pProjectMan->sampleState = 1;
		break;
		case 1:
			if(StepMotor_IsStop(0))
			{
				pProjectMan->sampleState = 0;
				pProjectMan->sampleStatus |= SAMPLESTATUS_SAMPLE;
				pProjectMan->sampleStatus &= ~SAMPLESTATUS_HOME;
			}
		break;
		default:
		break;
	}
#endif
	
	StepMotor_SetSpeed(0, pProjectMan->sampleSpeed);
	StepMotor_SetDir(0, CW);
	StepMotor_SetPluse(0, pProjectMan->sampleStopPoint);
	StepMotor_SetCMD(0, ENABLE);
	
	while(!StepMotor_IsStop(0) && !pProjectMan->smStopFlag)
		vTaskDelay(10);	
}

void DDC112_CMDParse(uint8_t *buf, uint8_t len)
{
	if(buf[0] == 0x8A)
	{
		if(pProjectMan->sampleDataCount < SAMPLE_BUFFER_SIZE-1)
		{
			pProjectMan->sampleData[pProjectMan->sampleDataCount] = *(uint32_t*)(buf+1);
			pProjectMan->sampleDataCount++;
		}
	}
}

void UITask(void)
{
	qsize size;

	USB_Port_Set(0); 	//USB先断开
	delay_ms(700);
	USB_Port_Set(1);	//USB再次连接
	Set_USBClock();   
	USB_Interrupts_Config();    
	USB_Init();	

	while(1)
	{
			//处理UI数据
		size = queue_find_cmd(&hmi_que, hmi_cmd_buffer, HMI_CMD_MAX_SIZE); //从缓冲区获取一条指令
		if(size > 0)//接收到指令
			ProcessMessage((PCTRL_MSG)hmi_cmd_buffer, size);//处理指令
		
		//处理ddc112数据
		size = queue_find_cmd(&ddc112_que, ddc112_cmd_buffer, DDC112_CMD_MAX_SIZE); //从缓冲区获取一条指令
		if(size > 0)//接收到指令
			DDC112_CMDParse(ddc112_cmd_buffer, size);
		
		//处理USB数据
		size = queue_find_cmd(&usb_que, usb_cmd_buffer, USB_CMD_MAX_SIZE); //从缓冲区获取一条指令
		if(size > 0)//接收到指令
			ProcessMessage((PCTRL_MSG)usb_cmd_buffer, size);//处理指令
		
		//处理用户数据
		size = queue_find_cmd(&user_que, user_cmd_buffer, USER_CMD_MAX_SIZE); //从缓冲区获取一条指令
		if(size > 0)//接收到指令
			ProcessMessage((PCTRL_MSG)user_cmd_buffer, size);//处理指令
	
		vTaskDelay(10);

		IWDG_Feed();//喂狗
			
		LED1Task(); //LED1闪烁指示UI线程正在运行
		
		if(button[0].flag)
		{
			button[0].flag = 0;
			
			if(pProjectMan->projectMode != PROJECTMODE_NORMAL)
			{
				if(pProjectMan->page != FASTTESTPAGE_INDEX)
				{
					SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_PROJECT_EDIT, pProjectMan->projectString);
					SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_BATCHNUM_EDIT, pProjectMan->batchString);
					SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_SAMPLENUM_EDIT, pProjectMan->sampleString);
					
					pProjectMan->scanInputFlag = 0;
					if(pProjectMan->qrCodeEnable)
						QRCode_StartRecognize();

					pProjectMan->page = FASTTESTPAGE_INDEX;
					SetScreen(pProjectMan->page);
				}
				else
				{
					ReadRTC();			
					pProjectMan->projectMode = PROJECTMODE_NORMAL;
					pProjectMan->page = RESULTPAGE_INDEX;
					SetScreen(pProjectMan->page);
				}
			}//if(pProjectMan->projectMode == PROJECTMODE_NORMAL)
		}//if(button[0].flag)
	}//whilt(1)
}

#ifdef __cplusplus
}
#endif
