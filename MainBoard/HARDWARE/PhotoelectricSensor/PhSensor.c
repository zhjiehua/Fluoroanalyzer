#include "PhSensor.h"
#include "usart.h"
#include "../UILogic/pageCommon.h"
#include "../DCMotor/DCMotor.h"

#include "FreeRTOS.h"
#include "task.h"

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//��������������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	   

const PhSensorPin_TypeDef phSensorPin[] = 
{
    {GPIOC, GPIO_Pin_12},//"EXIN1"
    {GPIOD, GPIO_Pin_0},//"EXIN2"
    {GPIOD, GPIO_Pin_1},//"EXIN3"
    {GPIOD, GPIO_Pin_2},//"EXIN4"
    {GPIOD, GPIO_Pin_3},//"EXIN5"
    {GPIOD, GPIO_Pin_4},//"EXIN6"
    {GPIOD, GPIO_Pin_5},//"EXIN7"
    {GPIOD, GPIO_Pin_6},//"EXIN8"
    {GPIOD, GPIO_Pin_7},//"EXIN9"
    {GPIOB, GPIO_Pin_3},//"EXIN10"  ��Ҫ�ض���
    {GPIOB, GPIO_Pin_4},//"EXIN11"
    {GPIOB, GPIO_Pin_5},//"EXIN12"
    {GPIOB, GPIO_Pin_6},//"EXIN13"
    {GPIOB, GPIO_Pin_7},//"EXIN14"
    {GPIOB, GPIO_Pin_8},//"EXIN15"
    {GPIOB, GPIO_Pin_9},//"EXIN16"
};

typedef struct
{
	uint8_t sensorIndex;		//�������ӿ�
	uint8_t uartLCDInputIndex;	//LCD�����״̬�ؼ����
}SensorMap_TypeDef;

const SensorMap_TypeDef sensorMap[16] = 
{
	{1, 5},
	{2, 6},
	{3, 13},
	{4, 14},
	{5, 11},
	{6, 12},
	{7, 255},
	{8, 10},
	{9, 9},
	{10, 255},
	{11, 7},
	{12, 8},
	{13, 3},
	{14, 4},
	{15, 1},
	{16, 2},
};

//������״̬
static PhSensor_TypeDef phSensor;
PhSensor_TypeDef *pPhSensor = &phSensor;

void PhSensor_SingleScan(PhSensorEnum_TypeDef num)
{
	uint8_t preFlag = 0;
	uint8_t curFlag = 0;
	
	if(phSensor.curStatusSingle & (PHSENSOR1_MASK << num))
	{
		phSensor.preStatusSingle |= (PHSENSOR1_MASK << num);
		preFlag = 1;
	}
	else
		phSensor.preStatusSingle &= ~(PHSENSOR1_MASK << num);
	
	if(GPIO_ReadInputDataBit(phSensorPin[num].GPIOx, phSensorPin[num].GPIO_Pin))
	{
        phSensor.curStatusSingle |= (PHSENSOR1_MASK << num);
		curFlag = 1;
	}
	else
		phSensor.curStatusSingle &= ~(PHSENSOR1_MASK << num);
	
	if((curFlag ^ preFlag) & preFlag)
		phSensor.fallingEdgeSingle |= ((uint32_t)(0x0001) << num);
	else
		phSensor.fallingEdgeSingle &= ~((uint32_t)(0x0001) << num);
	
	if((curFlag ^ preFlag) & curFlag)
		phSensor.rasingEdgeSingle |= ((uint32_t)(0x0001) << num);
	else
		phSensor.rasingEdgeSingle &= ~((uint32_t)(0x0001) << num);
}

uint8_t PhSensor_SingleCheck(PhSensorEnum_TypeDef num)
{
	return (!!(phSensor.curStatusSingle & (PHSENSOR1_MASK << num)));
}

uint8_t PhSensor_SingleCheckEdge(PhSensorEnum_TypeDef num, CheckEdge_TypeDef edge)
{
	if(edge == FALLINGEDGE)
		return (!!(phSensor.fallingEdgeSingle & (PHSENSOR1_MASK << num)));
	else
		return (!!(phSensor.rasingEdgeSingle & (PHSENSOR1_MASK << num)));
}

void PhSensor_Scan(void)
{
    uint8_t i;
	phSensor.preStatus = phSensor.curStatus;
	
    for(i=0;i<SIZEOF(phSensorPin);i++)
    {
        if(GPIO_ReadInputDataBit(phSensorPin[i].GPIOx, phSensorPin[i].GPIO_Pin))
            phSensor.curStatus |= PHSENSOR1_MASK << i;
        else
			phSensor.curStatus &= ~(PHSENSOR1_MASK << i);
    }
    
	phSensor.fallingEdge = (phSensor.curStatus ^ phSensor.preStatus) & phSensor.preStatus;
	phSensor.rasingEdge = (phSensor.curStatus ^ phSensor.preStatus) & phSensor.curStatus;
}

uint8_t PhSensor_CheckEdge(PhSensorEnum_TypeDef num, CheckEdge_TypeDef edge)
{
	if(edge == FALLINGEDGE)
		return (!!(phSensor.fallingEdge & (PHSENSOR1_MASK << num)));
	else
		return (!!(phSensor.rasingEdge & (PHSENSOR1_MASK << num)));
}

uint8_t PhSensor_Check(PhSensorEnum_TypeDef num)
{
	return (!!(phSensor.curStatus & (PHSENSOR1_MASK << num)));
}

//��ʼ��PB8Ϊ�����.��ʹ������ڵ�ʱ��		    
//��������ʼ��
void PhSensor_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;
    uint8_t i;
    
    for(i=0;i<SIZEOF(phSensorPin);i++)
    {
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA << ((phSensorPin[i].GPIOx-GPIOA)/(GPIOB-GPIOA)), ENABLE);
        
        GPIO_InitStructure.GPIO_Pin = phSensorPin[i].GPIO_Pin;	//EXINx
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 	//��������
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
        GPIO_Init(phSensorPin[i].GPIOx, &GPIO_InitStructure);
        
        phSensor.checkEdge[i] = FALLINGEDGE;
    }
	
	GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
	
    phSensor.preStatus = 0x00;
	phSensor.curStatus = 0x00;
	phSensor.fallingEdge = 0x00;
    phSensor.rasingEdge = 0x00;
	
	phSensor.preStatusSingle = 0x00;
	phSensor.curStatusSingle = 0x00;
	phSensor.fallingEdgeSingle = 0x00;
    phSensor.rasingEdgeSingle = 0x00;
}

void PhSensorScanTask(void)
{
    uint8_t i;

	PhSensor_Scan();
	
	for(i=0;i<16;i++)
	{
//			if(PhSensor_CheckEdge((PhSensorEnum_TypeDef)i, FALLINGEDGE))
//				printf("--phSensor%d is pressed!\n", i);

		SetButtonValue(INPUTPAGE_INDEX, sensorMap[i].uartLCDInputIndex, 
			(sensorMap[i].uartLCDInputIndex == 6) ? (PhSensor_Check((PhSensorEnum_TypeDef)i)) : (!PhSensor_Check((PhSensorEnum_TypeDef)i)));
	}
}
