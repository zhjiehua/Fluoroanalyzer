/*

*/

#ifndef __STEPMOTOR_H__
#define __STEPMOTOR_H__
#include "sys.h"
#include "../HardwareCommon.h"
#include "../PhotoelectricSensor/PhSensor.h"

/*�����������*/
#define STEPMOTOR_ANGLEPERSTEP  1.8  //������������  
#define STEPMOTOR_FREDIV  64//64  //���������Ƶϵ��
#define STEPMOTOR_PULSEPERROUND  ((360/STEPMOTOR_ANGLEPERSTEP)*STEPMOTOR_FREDIV)  //�������ÿת��Ҫ���ٸ�����

//�������1r/s��Ҫ���õĶ�ʱ������
#define STEPMOTOR_FREQ_ROUNDPERSEC (72000000/STEPMOTOR_PULSEPERROUND)

//һ�㲽����������ٶ�Ϊ600r/min=10r/s������1000r/min=16.7r/s��������ؼ����½�����������һ�㲻��������ô�ߵ��ٶ�
#define STEPMOTOR_FREQ(r) (72000000/(STEPMOTOR_PULSEPERROUND*(r)))

#define STEPMOTOR_MAIN 0
#define STEPMOTOR_SLAVE 1

#define STEPMOTOR_COUNT 2
#define STEPMOTOR_DEFAULT_PERIOD STEPMOTOR_FREQ(0.1)

#define STEP1_EN    PAout(15) 
#define STEP1_DIR   PAout(8)
#define STEP1_PWM   PBout(0)

#define STEP2_EN    PBout(3) 
#define STEP2_DIR   PBout(4)
#define STEP2_PWM   PBout(1)

#define STEPMOTOR_OFFSET  0  //��Һ��Ҫ��׼�ۿף���Ҫƫ��

#define SPEED_CONST  1500 //���ڼӼ��ٵĲ���ʱ�䳣��

typedef struct
{
	uint16_t speed;
	uint16_t speedConst;
}SpeedLevel_TypeDef;

typedef enum
{
	SPEED_NONE,	//��ֹ����������
	SPEED_ACC,  //����
	SPEED_DEC,	//����
	SPEED_STOP, //��������ֹͣ�����������
	SPEED_POSOFFSET,  //��������ڴ�����λ��ֹͣ������������װ�̻ζ�ʱ�ᵼ�´�������⵽����
}SpeedStatus_TypeDef;

typedef struct
{
    GPIO_TypeDef* PWM_GPIOx;
    uint16_t PWM_GPIO_Pin;
    
    TIM_TypeDef* TIMx;   
    uint16_t TIM_Channel;
    
    uint8_t NVIC_IRQChannel;
    
    /*****************************/
    GPIO_TypeDef* DIR_GPIOx;
    uint16_t DIR_GPIO_Pin;
    
    /*****************************/
    GPIO_TypeDef* EN_GPIOx;
    uint16_t EN_GPIO_Pin;

}StepMotorPin_TypeDef;

typedef struct
{
	uint16_t offset;  //λ��ƫ�ƣ�����У��ת�̣�Ĭ��450
	uint32_t pulseCount;	//�߶��ٸ�����
	uint32_t pulseDec; //����ٸ����忪ʼ����
	
	Direction_TypeDef direction;  //��¼����

	SpeedStatus_TypeDef speedStatus;
	const SpeedLevel_TypeDef *pSpeedLevel;  //�ٶȼ�������
	uint16_t curSpeedIndex; //��ǰ�ٶȣ����ڼӼ���
	uint16_t desSpeedIndex; //Ŀ���ٶ�

    uint8_t status;		//bit8 : 1,�������弼��ģʽ; 0,������ͨģʽ
						//bit1 : 1,�������������; 0,����Ѿ�ֹͣ
	
	uint8_t control;  //�������ʹ�ܿ���
}StepMotor_TypeDef;

extern StepMotor_TypeDef *pStepMotor;
extern const SpeedLevel_TypeDef speedLevel[];
extern const StepMotorPin_TypeDef StepMotorPin[];

void StepMotor_SetDir(uint8_t num, Direction_TypeDef dir);
void StepMotor_SetSpeed(uint8_t num, uint8_t speedIndex);
void StepMotor_SetPluse(uint8_t num, uint32_t pulseCount);
void StepMotor_SetCMD(uint8_t num, Status sta);
void StepMotor_Stop(uint8_t num);
uint8_t StepMotor_IsStop(uint8_t num);

void StepMotor_Init(void);

#endif
