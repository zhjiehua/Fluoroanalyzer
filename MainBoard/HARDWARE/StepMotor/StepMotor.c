#include "beep.h"
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

#include "sys.h"
#include "StepMotor/Drv8825.h"
#include "StepMotor/StepMotor.h"
#include "Beep/beep.h"
#include "LED/LED.h"
#include "usart.h"
#include "PhotoelectricSensor/PhSensor.h"
#include "stdlib.h"
#include "CPrintf.h"
#include "../Logic/management.h"

#include "FreeRTOS.h"
#include "task.h"

static StepMotor_TypeDef stepMotor[STEPMOTOR_COUNT];
StepMotor_TypeDef *pStepMotor = stepMotor;

const StepMotorPin_TypeDef StepMotorPin[] = 
{
    {GPIOB, GPIO_Pin_0, TIM3, TIM_Channel_3, TIM3_IRQn, GPIOA, GPIO_Pin_8, GPIOA, GPIO_Pin_15},//"StepMotor1"
    {GPIOB, GPIO_Pin_1, TIM1, TIM_Channel_3, TIM1_UP_IRQn, GPIOB, GPIO_Pin_4, GPIOB, GPIO_Pin_3},//"StepMotor2"
};

//��λ�ٶ�
const SpeedLevel_TypeDef speedLevel[] = {
	{STEPMOTOR_FREQ(0.09), 0.10*SPEED_CONST},   //0  0.10
	{STEPMOTOR_FREQ(0.15), 0.15*SPEED_CONST},
	{STEPMOTOR_FREQ(0.20), 0.20*SPEED_CONST},
	{STEPMOTOR_FREQ(0.25), 0.25*SPEED_CONST},
	{STEPMOTOR_FREQ(0.40), 0.40*SPEED_CONST},
	{STEPMOTOR_FREQ(0.60), 0.60*SPEED_CONST},	//5
	{STEPMOTOR_FREQ(0.80), 0.80*SPEED_CONST},
	{STEPMOTOR_FREQ(1.00), 1.00*SPEED_CONST},
	{STEPMOTOR_FREQ(1.20), 1.20*SPEED_CONST},
	{STEPMOTOR_FREQ(1.40), 1.40*SPEED_CONST},
	{STEPMOTOR_FREQ(1.60), 1.60*SPEED_CONST},	//10
	{STEPMOTOR_FREQ(1.80), 1.80*SPEED_CONST},
	{STEPMOTOR_FREQ(2.00), 2.00*SPEED_CONST},
	{STEPMOTOR_FREQ(2.20), 2.20*SPEED_CONST},
};

//���ò����������
void StepMotor_SetDir(uint8_t num, Direction_TypeDef dir)
{
	if(dir == CW)
	{
		GPIO_ResetBits(StepMotorPin[num].DIR_GPIOx, StepMotorPin[num].DIR_GPIO_Pin);
		stepMotor[num].direction = CW;
	}
	else
	{
		GPIO_SetBits(StepMotorPin[num].DIR_GPIOx, StepMotorPin[num].DIR_GPIO_Pin);
		stepMotor[num].direction = CCW;
	}
}

//���ò�������ٶ�
void StepMotor_SetSpeed(uint8_t num, uint8_t speedIndex)
{
	stepMotor[num].desSpeedIndex = speedIndex;	 //����Ŀ���ٶ�
}

//���ò������������
//������ô�������������������ģʽ
void StepMotor_SetPluse(uint8_t num, uint32_t pulseCount)
{
	uint8_t i;
	uint32_t decCount = 0;
	
	stepMotor[num].pulseCount = pulseCount;	 //����������
	stepMotor[num].status |= 0x80;	//�����������ģʽ
	
	for(i=0;i<stepMotor[num].desSpeedIndex;i++)
	{
		decCount += stepMotor[num].pSpeedLevel[i].speedConst;
	}
	if(2*decCount > pulseCount)
	{
		stepMotor[num].pulseDec = pulseCount/3;
		stepMotor[num].desSpeedIndex--;
	}
	else
		stepMotor[num].pulseDec = pulseCount - decCount;	 //���ü��ٵ�
	
	//cDebug("pulseCount = %d\r\n", pulseCount);
	//cDebug("decCount = %d\r\n", decCount);
}

//���ò����������
void StepMotor_SetCMD(uint8_t num, Status sta)
{
	if(sta == ENABLE)
	{
        if(((stepMotor[num].status)&0x01) == 0)
        {
            stepMotor[num].speedStatus = SPEED_ACC;  //����
            stepMotor[num].curSpeedIndex = 0;  //��ʼ�ٶ�Ϊ��С�ٶ�
			stepMotor[num].status |= 0x01;
            TIM_SetCounter(StepMotorPin[num].TIMx, stepMotor[num].pSpeedLevel[0].speed);
            TIM_SetCompare3(StepMotorPin[num].TIMx, (stepMotor[num].pSpeedLevel[0].speed)/2);
			GPIO_ResetBits(StepMotorPin[num].EN_GPIOx, StepMotorPin[num].EN_GPIO_Pin);
            TIM_Cmd(StepMotorPin[num].TIMx, ENABLE);//������ʱ��
        }
		else if(stepMotor[num].curSpeedIndex <= stepMotor[num].desSpeedIndex)
        {
            stepMotor[num].speedStatus = SPEED_ACC;  //����
        }
        else
        {
            stepMotor[num].speedStatus = SPEED_DEC;
        }		
	}
	else
	{
        if(((stepMotor[num].status)&0x01) != 0)
        {
            stepMotor[num].desSpeedIndex = 0;
            stepMotor[num].speedStatus = SPEED_DEC;  //���٣����ٵ���С�ٶ�
        }

        //TIM_Cmd(StepMotorPin[0].TIMx, ENABLE);
		//TR1 = 0;  //ֹͣ��ʱ��1
		//pStepMotor->control = 0x00;
	}
}

//���ò������ֹͣ����
void StepMotor_Stop(uint8_t num)
{
    //if((((stepMotor[num].status)&0x01) != 0) && stepMotor[num].speedStatus != SPEED_POSOFFSET)
	if((((stepMotor[num].status)&0x01) != 0) && stepMotor[num].speedStatus != SPEED_STOP)
        stepMotor[num].speedStatus = SPEED_STOP;
	
	//TF1 = 1; //���붨ʱ��1�ж�		
}

//��鲽������Ƿ��Ѿ�ֹͣ
//����1,��ʾ�Ѿ�ֹͣ
uint8_t StepMotor_IsStop(uint8_t num)
{
	return !(stepMotor[num].status&0x01);
}

//���������ʼ��
void StepMotor_Init(void)
{
    /*********************��ʼ�������������***************************************/
    uint8_t i;
    GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB|RCC_APB2Periph_AFIO|RCC_APB2Periph_TIM1, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    for(i=0;i<SIZEOF(StepMotorPin);i++)
    {
        GPIO_InitStructure.GPIO_Pin = StepMotorPin[i].EN_GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //�������
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //�ٶ�Ϊ50MHz
        GPIO_Init(StepMotorPin[i].EN_GPIOx, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = StepMotorPin[i].DIR_GPIO_Pin;
        GPIO_Init(StepMotorPin[i].DIR_GPIOx, &GPIO_InitStructure);
        
        GPIO_InitStructure.GPIO_Pin = StepMotorPin[i].PWM_GPIO_Pin;
        GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		 //�����������
        GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	 //�ٶ�Ϊ50MHz
        GPIO_Init(StepMotorPin[i].PWM_GPIOx, &GPIO_InitStructure);
        
        //STEPMOTOR_DEFAULT_PERIOD
        TIM_TimeBaseStructure.TIM_Period = STEPMOTOR_DEFAULT_PERIOD; //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ,��72000000/900=80KHz
        TIM_TimeBaseStructure.TIM_Prescaler = 0; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ 
        TIM_TimeBaseStructure.TIM_ClockDivision = 1; //����ʱ�ӷָ�:TDTS = Tck_tim
        TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;  //TIM���ϼ���ģʽ
        TIM_TimeBaseInit(StepMotorPin[i].TIMx, &TIM_TimeBaseStructure);
        
        TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2; //ѡ��ʱ��ģʽ:TIM�����ȵ���ģʽ2  TIM_OCMode_Timing
        TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //�Ƚ����ʹ��
        TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //�������:TIM����Ƚϼ��Ը�
        TIM_OCInitStructure.TIM_Pulse = STEPMOTOR_DEFAULT_PERIOD/2; //���ô�װ�벶��ȽϼĴ���������ֵ
        
        switch(StepMotorPin[i].TIM_Channel)
        {
            case TIM_Channel_1:
                TIM_OC1Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC1PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_2:
                TIM_OC2Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC2PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_3:
                TIM_OC3Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC3PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
            case TIM_Channel_4:
                TIM_OC4Init(StepMotorPin[i].TIMx, &TIM_OCInitStructure);
                TIM_OC4PreloadConfig(StepMotorPin[i].TIMx, TIM_OCPreload_Enable);
                break;
        }
        
		GPIO_SetBits(StepMotorPin[i].EN_GPIOx, StepMotorPin[i].EN_GPIO_Pin);
        //GPIO_ResetBits(StepMotorPin[i].EN_GPIOx, StepMotorPin[i].EN_GPIO_Pin);  //ʹ�ܲ���������
        GPIO_SetBits(StepMotorPin[i].DIR_GPIOx, StepMotorPin[i].DIR_GPIO_Pin);
        
        NVIC_InitStructure.NVIC_IRQChannel = StepMotorPin[i].NVIC_IRQChannel;
        NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2 ;//��ռ���ȼ�3
        NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;		//�����ȼ�3
        NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
        NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
        
        TIM_ARRPreloadConfig(StepMotorPin[i].TIMx, ENABLE);
        TIM_ITConfig(StepMotorPin[i].TIMx, TIM_IT_Update, ENABLE);
        //TIM_Cmd(StepMotorPin[i].TIMx, ENABLE);
		TIM_Cmd(StepMotorPin[i].TIMx, DISABLE);
		
		/****************��ʼ����������ṹ��***********************************/
		stepMotor[i].offset = STEPMOTOR_OFFSET;
		//stepMotor[i].offset = pProjectMan->posCali1;
		stepMotor[i].speedStatus = SPEED_NONE;
		stepMotor[i].pSpeedLevel = speedLevel;
		stepMotor[i].curSpeedIndex = 0;
		stepMotor[i].desSpeedIndex = 0;
		stepMotor[i].status = 0;
		stepMotor[i].direction = CW;
		
		stepMotor[i].control = 0x00;
    }
    //��ʹ�ò������2�ж�
    TIM_ITConfig(StepMotorPin[1].TIMx, TIM_IT_Update, DISABLE);
    
//    for(i=0;i<SIZEOF(speedLevel);i++)
//    {
//        printf("speedLevel[%d].speed = %d\t\n", i, speedLevel[i].speed);
//        printf("speedLevel[%d].speedConst = %d\t\n", i, speedLevel[i].speedConst);
//    }
	
	//DRV8825��ʼ��
	Drv8825_Init();
}

void TIM1_UP_IRQHandler(void)
{
    uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
    if(TIM_GetITStatus(TIM1, TIM_IT_Update) == SET)
    {
        TIM_ClearITPendingBit(TIM1, TIM_IT_Update);
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}

/* Timer5 interrupt routine */
void TIM3_IRQHandler(void)
{
	static uint16_t cnt = 0;
	static uint16_t offset = 0;
	static uint32_t pluseCount = 0;
    uint32_t oldBasePri = portSET_INTERRUPT_MASK_FROM_ISR();
    
    if(TIM_GetITStatus(TIM3, TIM_IT_Update) == SET)
    {
        switch(stepMotor[0].speedStatus)
        {				  
            case SPEED_ACC: //����
            {
                cnt++;
                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
                {
                    cnt = 0;
                    stepMotor[0].curSpeedIndex++;
                }
        
                if(stepMotor[0].curSpeedIndex >= stepMotor[0].desSpeedIndex) //�������
                {
                    stepMotor[0].curSpeedIndex = stepMotor[0].desSpeedIndex;
                    stepMotor[0].speedStatus = SPEED_NONE;
                }		
            }
            break;
            case SPEED_DEC: //����
            {
                cnt++;
                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
                {
                    cnt = 0;
                    stepMotor[0].curSpeedIndex--;
                }
        
                if(stepMotor[0].curSpeedIndex <= stepMotor[0].desSpeedIndex)  //���ٵ�Ŀ���ٶ�
                {
                    stepMotor[0].curSpeedIndex = stepMotor[0].desSpeedIndex;
                    stepMotor[0].speedStatus = SPEED_NONE;
                    
                    cnt = 0;
                    //TR1 = 0;  //ֹͣ��ʱ��1
                    //TIM_Cmd(StepMotorPin[0].TIMx, DISABLE);
                }
            }
            break;
            case SPEED_STOP: 
            {
//                cnt++;
//                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
//                {
//                    cnt = 0;
//                    stepMotor[0].curSpeedIndex--;
//                }
//        
//                if(stepMotor[0].curSpeedIndex <= 0)  //���ٵ���С�ٶ�
//                {
//                    stepMotor[0].curSpeedIndex = 0;			
//                    cnt = 0;
//                }
//                stepMotor[0].speedStatus = SPEED_POSOFFSET;
//            }
//            break;
//            case SPEED_POSOFFSET:
//            {
                offset++;
                if(offset++ >= stepMotor[0].offset)
                {
					offset = 0;
                    cnt = 0;
                    stepMotor[0].curSpeedIndex = 0;
                    stepMotor[0].speedStatus = SPEED_NONE;
					//stepMotor[0].status &= ~0x01;
					
					pluseCount = 0;
					stepMotor[0].status &= ~0x81; //ֹͣ�����ǿ���˳��������ģʽ
					
                    //ֹͣ��ʱ��1
					GPIO_SetBits(StepMotorPin[0].EN_GPIOx, StepMotorPin[0].EN_GPIO_Pin);
                    TIM_Cmd(TIM3, DISABLE);
                    //cDebug("interrupt---stop the timer5\n");
                    break;	
                }

                cnt++;
                if(cnt >= stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speedConst)
                {
                    cnt = 0;
                    stepMotor[0].curSpeedIndex--;
                }
        
                if(stepMotor[0].curSpeedIndex <= 0)  //���ٵ���С�ٶ�
                {
                    stepMotor[0].curSpeedIndex = 0;
                    cnt = 0;
                }
            }
            break;
            case SPEED_NONE:
            break;
            default:
            break;
        }

		if(stepMotor[0].status & 0x80)
		{
			pluseCount++;
			
			if(pluseCount >= stepMotor[0].pulseDec && stepMotor[0].speedStatus != SPEED_DEC)
			{
				stepMotor[0].desSpeedIndex = 0;
				stepMotor[0].speedStatus = SPEED_DEC;
				//cDebug("stepMotor[0].pulseDec = %d\r\n", stepMotor[0].pulseDec);
			}
			
			if(pluseCount >= stepMotor[0].pulseCount)
			{
				pluseCount = 0;
				offset = 0;
				cnt = 0;
				
				stepMotor[0].status &= ~0x81;
				stepMotor[0].curSpeedIndex = 0;
				stepMotor[0].speedStatus = SPEED_NONE;
				
				//ֹͣ��ʱ��1
				GPIO_SetBits(StepMotorPin[0].EN_GPIOx, StepMotorPin[0].EN_GPIO_Pin);
				TIM_Cmd(TIM3, DISABLE);
			}
		}
        //���¶�ʱ��
        TIM_SetCounter(TIM3, stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speed);
        TIM_SetCompare3(TIM3, (stepMotor[0].pSpeedLevel[stepMotor[0].curSpeedIndex].speed)/2);
        
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
    }
    portCLEAR_INTERRUPT_MASK_FROM_ISR(oldBasePri);
}
