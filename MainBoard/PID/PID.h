#ifndef __PID_H_
#define __PID_H_
#include "stdlib.h"
#include "CPrintf.h"
#define qDebug cDebug

//�����2��PID�㷨Ϊ����PID�㷨
#define PID_POSITION_TYPE	0	//λ����PID
#define PID_INCREAMENT_TYPE 1	//������PID

//����ļ���PID�����㷨������λ����PIDΪ����ģ��
#define PID_INTEGRAL_SAPARATION_TYPE	0	//���ַ���ʽ�����ڽ��Ŀ����뵱ǰ�����̫���³�������
#define PID_INTEGRAL_ANTISATURATION_TYPE	0	//�����ͻ���ʽ����ֹ���ض�����뱥��������ʧ��
//#define PID_TRAPEZOID_INTEGRAL_TYPE		0	//���λ���ʽ����߻��־��ȣ�λ���͸�Ϊp->SumError += (p->CurrError + p->PrevError)/2;
#define PID_INTEGRAL_VARIAL_TYPE		0	//�����ʽ��ƫ��Խ�󣬻���Խ����ƫ��ԽС������Խ��

//#define KP	0.3
//#define KI	0.6
//#define KD	0.5

#define INTEGRAL_SAPARATION_THRESHOLD	150 //���ַ�����ֵ
#define INTEGRAL_VARIATION_LOW_THRESHOLD	180 //���ַ�����ֵ
#define INTEGRAL_VARIATION_HIGH_THRESHOLD	200 //���ַ�����ֵ

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long long uint64_t;

//typedef char int8_t;
//typedef short int16_t;
//typedef int int32_t;
//typedef long long int64_t;
typedef float float32_t;
typedef double float64_t;

typedef struct
{
	float32_t Proportion;//��������
	float32_t Integral;//���ֳ���
	float32_t Derivative;//΢�ֳ���

	int32_t CurrError;//Error[0]
	int32_t PrevError;//Error[-1]
#if PID_POSITION_TYPE
	int64_t SumError;//�ۼ����
#elif PID_INCREAMENT_TYPE
	int32_t LastError;//Error[-2]
#endif

#if PID_INTEGRAL_SAPARATION_TYPE
	int8_t IntSaparFlag; //���ַ����־
	int64_t SumError;//�ۼ����
#endif

#if PID_INTEGRAL_ANTISATURATION_TYPE
	int32_t PointMax; //���ֵ
	int32_t PointMin; //��Сֵ
	int8_t IntSaparFlag; //���ַ����־
	int64_t SumError;//�ۼ����
#endif

#if PID_INTEGRAL_VARIAL_TYPE
	int8_t IntVarFactor; //���ַ����־
	int64_t SumError;//�ۼ����
#endif

	int32_t	SetPoint;//Ŀ���
	int32_t ActualPoint;//ʵ�ʲɼ��㣬����

	int32_t Value;//PID���������ֵ
	int32_t (*Value2Point)(int32_t v);//PIDֵ��ʵ��������ӳ���ϵ�����
}PID_TypeDef;

extern PID_TypeDef *pPID;
void PID_Init(PID_TypeDef *p, float32_t Kp, float32_t Ki, float32_t Kd);
void PID_UpdateSetPoint(PID_TypeDef *p, int32_t Point);//����Ŀ���
void PID_UpdateActualPoint(PID_TypeDef *p, int32_t Point);//���²ɼ���
int32_t PID_Calc(PID_TypeDef *p); //PID���ĺ���

#endif
