#ifdef __cplusplus	  
extern "C" {
#endif

#include "project.h"
#include "management.h"
#include "miscellaneous.h"	
#include "CPrintf.h"
#include "stdio.h"
#include "stdint.h"
	
#include "../HARDWARE/DCMotor/DCMotor.h"
#include "../HARDWARE/StepMotor/StepMotor.h"
#include "../HARDWARE/Beep/beep.h"
#include "../HARDWARE/RelayMOS/RelayMOS.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/LED/led.h"
#include "../HARDWARE/DDC112/ddc112_interface.h"
	
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "timers.h"

#include "math.h"

void ProjectTask(void)
{
	uint32_t i, j;
	uint32_t temp;

	ResetDevice();//��λ������
	
	while(!pProjectMan->lcdNotifyResetFlag)
	{
		vTaskDelay(100);
		
//		if(j++ > 1000)
//			ResetDevice();//��λ������
	}
	
	//GraphChannelAdd(CURVEPAGE_INDEX, CURVE_GRAPHY, 0, 0xFFFF);

	//DDC112������ʼ��
	DDC112IF_RangeSetting(1);
	DDC112IF_IntegralTimeSetting(25); //14*500us
	DDC112IF_BufferSizeSetting(2);
//	DDC112IF_IntegralTimeSetting(11);
//	DDC112IF_BufferSizeSetting(2);	
	
	while(1)
	{

		vTaskDelay(10);

		LED2Task(); //LED2��˸ָʾPROJECT�߳���������

		switch(pProjectMan->projectMode)
		{
			case PROJECTMODE_HOME:
				Sample_Home();
				//if(pProjectMan->sampleStatus&SAMPLESTATUS_HOME)
					pProjectMan->projectMode = PROJECTMODE_NONE;
			
				if(pProjectMan->page == PARAMETERPAGE_INDEX)
					SetButtonValue(PARAMETERPAGE_INDEX, PARAMETER_HOME_BUTTON, 0);
			break;
			case PROJECTMODE_GOREADY:
				Sample_GoReady();
				//if(pProjectMan->sampleStatus&SAMPLESTATUS_READY)
					pProjectMan->projectMode = PROJECTMODE_NONE;
			
				if(pProjectMan->page == PARAMETERPAGE_INDEX)
					SetButtonValue(PARAMETERPAGE_INDEX, PARAMETER_GO_SP_BUTTON, 0);
			break;
			case PROJECTMODE_SAMPLE:
				Sample();
				//if(pProjectMan->sampleStatus&SAMPLESTATUS_SAMPLE)
					pProjectMan->projectMode = PROJECTMODE_NONE;
			
				if(pProjectMan->page == PARAMETERPAGE_INDEX)
					SetButtonValue(PARAMETERPAGE_INDEX, PARAMETER_GO_EP_BUTTON, 0);
			break;
			case PROJECTMODE_NORMAL:
				
				//vTaskDelay(5);
				//pProjectMan->page = TIPSPAGE_INDEX;
				//SetScreen(pProjectMan->page);
				SetTextValue(TIPSPAGE_INDEX, TIPS0_TIPS_EDIT, (uint8_t*)"����С���");
				SetScreen(TIPSPAGE_INDEX);
			
				//DDC112IF_RedLedMode(2);
				DDC112IF_PurpleLedEnable(1);
				Sample_Home();
				Sample_GoReady();
			
				vTaskDelay(500);
			
				DDC112IF_SampleEnable(1);
				pProjectMan->sampleDataCount = 0;			
				Sample();
				
				DDC112IF_SampleEnable(0);
				Sample_Home();
				DDC112IF_PurpleLedEnable(0);
				//DDC112IF_RedLedMode(0);
				
				cDebug("pProjectMan->sampleDataCount = %d\r\n", pProjectMan->sampleDataCount);
				
				for(j=0;j<pProjectMan->sampleDataCount;j++) //���תС��
					EndianConvert(&pProjectMan->sampleData[j]);

#if 0
				DDC112IF_SendPCStart();
				for(j=0;j<pProjectMan->sampleDataCount;j++)
				{
					//EndianConvert(&pProjectMan->sampleData[j]);					
					//cDebug("sampleData[%d] = 0x%06x\r\n", j, pProjectMan->sampleData[j]);
					
					DDC112IF_SendPC(j, pProjectMan->sampleData[j]);
				}
				DDC112IF_SendPCEnd();
#endif
				
				//pProjectMan->sampleDataCount = 0;
				
				//�˲�
#define FILTER_ORDER	12
				
				//PCֻ��Ҫ350���㣬�Ӳɼ����ĵ�����350���㷢PC���������350���㣬����
				if(pProjectMan->sampleDataCount < 350+2*FILTER_ORDER)
				{
					for(i=pProjectMan->sampleDataCount+1;i<350+2*FILTER_ORDER;i++)
						pProjectMan->sampleData[i] = pProjectMan->sampleData[pProjectMan->sampleDataCount];
					
					pProjectMan->sampleDataCount = 350+2*FILTER_ORDER;
					
//					//������Ч
//					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"���棺�ɼ�ģ�����");
//					SetScreen(TIPS1PAGE_INDEX);
				}
				//pProjectMan->sampleDataCount = 350+2*FILTER_ORDER;				

				for(j=0;j<pProjectMan->sampleDataCount;j++) //��һ���˲�
				{
					pProjectMan->filterData[j] = OrderFilter(pProjectMan->sampleData+j, (j>=FILTER_ORDER ? FILTER_ORDER : j+1));
					
					//cDebug("sampleData[%d] = 0x%06x\r\n", j, pProjectMan->sampleData[j]);
					//cDebug("filterData[%d] = 0x%06x\r\n", j, pProjectMan->filterData[j]);
				}
				
				for(j=0;j<pProjectMan->sampleDataCount;j++) //�ڶ����˲�
				{
					pProjectMan->sampleData[j] = OrderFilter(pProjectMan->filterData+j, (j>=FILTER_ORDER ? FILTER_ORDER : j+1));
					//pProjectMan->sampleData[j] = pProjectMan->filterData[j];
				}
				
				//����
				for(j=0;j<pProjectMan->sampleDataCount/2;j++)
				{
					temp = pProjectMan->sampleData[j];
					pProjectMan->sampleData[j] = pProjectMan->sampleData[pProjectMan->sampleDataCount - j];
					pProjectMan->sampleData[pProjectMan->sampleDataCount - j] = temp;
				}
				
				//ȡ�м�350����
				for(j=pProjectMan->sampleDataCount/2-176;j<pProjectMan->sampleDataCount/2+176;j++)
				{
					pProjectMan->sampleData[j-pProjectMan->sampleDataCount/2+176] = pProjectMan->sampleData[j];
				}
				
				pProjectMan->sampleDataCount = 350;
				
				
				
				
				//�Ҽ����
				//pProjectMan->lineNumber = 2;
				//pProjectMan->checkWindowAxisX = 30;
				temp = pProjectMan->sampleDataCount/pProjectMan->lineNumber;
				
//				pProjectMan->axisTXCur = FindMaximal(pProjectMan->sampleData, temp); //�ҵ�һ�������
//				pProjectMan->axisCXCur = FindMaximal(pProjectMan->sampleData+temp, temp); //�ҵڶ��������
//				pProjectMan->axisCXCur += temp;
				pProjectMan->axisCXCur = FindMaximin(pProjectMan->sampleData, temp); //�ҵ�һ�������
				pProjectMan->axisTXCur = FindMaximin(pProjectMan->sampleData+temp, temp); //�ҵڶ��������
				pProjectMan->axisTXCur += temp;
				
				cDebug("pProjectMan->axisTXCur = %d\r\n", pProjectMan->axisTXCur);
				cDebug("pProjectMan->axisCXCur = %d\r\n", pProjectMan->axisCXCur);
				
				//����С���˷�����
				LeastSquare(pProjectMan->sampleData, pProjectMan->sampleDataCount, &pProjectMan->LeastSquareA, &pProjectMan->LeastSquareB);
//				pProjectMan->cThreshod = pProjectMan->LeastSquareA*pProjectMan->axisCXCur + pProjectMan->LeastSquareB;
//				pProjectMan->tThreshod = pProjectMan->LeastSquareA*pProjectMan->axisTXCur + pProjectMan->LeastSquareB;
				
				
				for(j=0;j<pProjectMan->sampleDataCount;j++)
				{
					temp = pProjectMan->LeastSquareA * j + pProjectMan->LeastSquareB;
					if(pProjectMan->sampleData[j] <= temp)
						pProjectMan->filterData[j] = pProjectMan->sampleData[j];
					else
						pProjectMan->filterData[j] = temp;
				}
				
				//����С���˷�����
				LeastSquare(pProjectMan->filterData, pProjectMan->sampleDataCount, &pProjectMan->LeastSquareA, &pProjectMan->LeastSquareB);
				pProjectMan->cThreshod = pProjectMan->LeastSquareA*pProjectMan->axisCXCur + pProjectMan->LeastSquareB;
				pProjectMan->tThreshod = pProjectMan->LeastSquareA*pProjectMan->axisTXCur + pProjectMan->LeastSquareB;
				
				cDebug("pProjectMan->LeastSquareA = %f\r\n", pProjectMan->LeastSquareA);
				cDebug("pProjectMan->LeastSquareB = %f\r\n", pProjectMan->LeastSquareB);
				cDebug("pProjectMan->cThreshod = %d\r\n", pProjectMan->cThreshod);
				cDebug("pProjectMan->tThreshod = %d\r\n", pProjectMan->tThreshod);
				
				//�жϽ��
				if(pProjectMan->sampleData[pProjectMan->axisCXCur] < pProjectMan->cThreshod)
				{
					pProjectMan->gender = 0;
					
					//������Ч
					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"������Ч");
					SetScreen(TIPS1PAGE_INDEX);
				}
				else if((pProjectMan->sampleData[pProjectMan->axisTXCur] < pProjectMan->tThreshod)
					&& (abs(pProjectMan->axisCXCur - pProjectMan->axisCX) <= pProjectMan->checkWindowAxisX))
				{	
					pProjectMan->cValue = GetCTValue(pProjectMan->axisCXCur);
					
					//����
					pProjectMan->gender = 2;
				}		
				else if((abs(pProjectMan->axisTXCur - pProjectMan->axisTX) > pProjectMan->checkWindowAxisX)
						|| (abs(pProjectMan->axisCXCur - pProjectMan->axisCX) > pProjectMan->checkWindowAxisX))
				{
					//
					pProjectMan->gender = 0;
					
					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"ƫ�Ʒ�Χ����");
					SetScreen(TIPS1PAGE_INDEX);
				}
				else
				{
					pProjectMan->cValue = GetCTValue(pProjectMan->axisCXCur);
					pProjectMan->tValue = GetCTValue(pProjectMan->axisTXCur);
					
					pProjectMan->tcValue = (float)pProjectMan->tValue/(float)pProjectMan->cValue;
					
					//����
					pProjectMan->gender = 1;
				}
				
				
				
				//���¿ؼ�
				if(pProjectMan->gender == 0) //��Ч
				{
					SetTextValue(RESULTPAGE_INDEX, RESULT_T_VALUE_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_C_VALUE_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_T_C_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_UNIT_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_RESULT_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, (uint8_t*)"");

					//����
					if(pProjectMan->alarmEnable)
						SetBuzzer(100);
				}
				else //����
				{
					uint8_t tempStr[10];
					
					sprintf((char*)tempStr, "%d", pProjectMan->cValue);
					SetTextValue(RESULTPAGE_INDEX, RESULT_C_VALUE_EDIT, tempStr);
					
					SetTextValue(RESULTPAGE_INDEX, RESULT_UNIT_EDIT, pProjectMan->concentrationUnit);
					
					if(pProjectMan->gender == 2) //����
					{
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_VALUE_EDIT, (uint8_t*)"");
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_C_EDIT, (uint8_t*)"");
						
						SetTextValue(RESULTPAGE_INDEX, RESULT_RESULT_EDIT, (uint8_t*)"-");
						SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, (uint8_t*)"");
					}
					else //����
					{
						sprintf((char*)tempStr, "%d", pProjectMan->tValue);
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_VALUE_EDIT, tempStr);
						
						sprintf((char*)tempStr, "%.04f", pProjectMan->tcValue);
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_C_EDIT, tempStr);
						
						SetTextValue(RESULTPAGE_INDEX, RESULT_RESULT_EDIT, (uint8_t*)"+");
						
						//���Ũ��
						switch(pProjectMan->fittingModel)
						{
							case 0://ֱ�����  y = A*x + B
								pProjectMan->concentration = pProjectMan->fittingPara1*pProjectMan->tcValue
									+ pProjectMan->fittingPara2;
							break;
							case 1://���ζ���ʽ  y = A*x^2 + B*x + C
								pProjectMan->concentration = pProjectMan->fittingPara1*pow(pProjectMan->tcValue, 2)
									+ pProjectMan->fittingPara2*pProjectMan->tcValue + pProjectMan->fittingPara3;
							break;
							case 2://���ζ���ʽ  y = A*x^3 + B*x^2 + C*x + D
								pProjectMan->concentration = pProjectMan->fittingPara1*pow(pProjectMan->tcValue, 3)
									+ pProjectMan->fittingPara2*pow(pProjectMan->tcValue, 2)
									+ pProjectMan->fittingPara3*pProjectMan->tcValue + pProjectMan->fittingPara4;
							break;
							case 3://logit-logֱ�߻ع�  A*lg(y) + B = ln((x/C)/(1-x/C))
							{
								float p = pProjectMan->tcValue/pProjectMan->fittingPara3;
								float Y = log(p/(1-p)); //��ʽ�ұ�, ln��ʽ��C��������log��lg��ʽ��log10
								float lgy = (Y - pProjectMan->fittingPara2)/pProjectMan->fittingPara1;
								
								pProjectMan->concentration = pow(10, lgy);
							}
							break;
							case 4://�Ĳ�����ϻع�  y = (A - D)/(1 + (x/C)^B) + D
								pProjectMan->concentration = (pProjectMan->fittingPara1 - pProjectMan->fittingPara4)
									/ (1 + pow(pProjectMan->tcValue/pProjectMan->fittingPara3, pProjectMan->fittingPara2) + pProjectMan->fittingPara4);
							break;
							case 5://����������ֵ
								pProjectMan->concentration = pProjectMan->tcValue;
							break;
							case 6://��Ե����
								pProjectMan->concentration = pProjectMan->tcValue;
							break;
							default:
							break;
						}
						
						if(pProjectMan->checkRangeLow == 0 && pProjectMan->checkRangeHigh == 0)
						{
							sprintf((char*)tempStr, "%.04f", pProjectMan->concentration);
							SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, tempStr);
						}
						else
						{
							if(pProjectMan->concentration >= pProjectMan->checkRangeLow 
								&& pProjectMan->concentration <= pProjectMan->checkRangeHigh)
							{
								sprintf((char*)tempStr, "%.04f", pProjectMan->concentration);
								SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, tempStr);
							}
							else//�������Ũ��
							{
								pProjectMan->gender = 0;
								
								SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, (uint8_t*)"");
								
								SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"�������Ũ��");
								SetScreen(TIPS1PAGE_INDEX);
								
								//����
								if(pProjectMan->alarmEnable)
									SetBuzzer(100);
							}
						}
					}
						
					//�Զ���ӡ
					if(pProjectMan->printerEnable && pProjectMan->gender)
						ResultPrint();
					
					pProjectMan->page = RESULTPAGE_INDEX;
					SetScreen(pProjectMan->page);
				}

				//�����ֵ�±�
				pProjectMan->maximumBufferIndex = 0;
				for(i=1;i<pProjectMan->sampleDataCount;i++)
				{
					if(pProjectMan->sampleData[i] > pProjectMan->sampleData[pProjectMan->maximumBufferIndex])
						pProjectMan->maximumBufferIndex = i;
				}

				//
				//DDC112IF_SendPCStart();
				i = 0;
				for(j=0;j<350;j++)
				{
					//DDC112IF_SendPC(j, pProjectMan->sampleData[j]);
					
					//pProjectMan->curveData[i] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)0xFEFFF); //180ΪҺ�������߿ؼ��ĸ߶ȣ�0xFEFFFΪDDC112�ɼ������ֵ
					pProjectMan->curveData[i] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)(pProjectMan->sampleData[pProjectMan->maximumBufferIndex]));
					//cDebug("pProjectMan->curveData[%d] = 0x%02X\r\n", i, pProjectMan->curveData[i]);
					i++;
				}
				//DDC112IF_SendPCEnd();
				
				GraphChannelDataClear(CURVEPAGE_INDEX, CURVE_GRAPHY, 1);
				GraphChannelDataAdd(CURVEPAGE_INDEX, CURVE_GRAPHY, 1, (uint8_t*)(pProjectMan->curveData), 350);
			
				//if(pProjectMan->sampleStatus&SAMPLESTATUS_READY)
					pProjectMan->projectMode = PROJECTMODE_NONE;
				
				//����Ʒ���
				sprintf((char*)pProjectMan->sampleString, "");
				SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_SAMPLENUM_EDIT, pProjectMan->sampleString);
				
				//����Ƿ�ʧЧ
				if(strncmp((const char*)pProjectMan->invalideData, (const char*)pProjectMan->rtc, 10) <= 0)
				{
					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"���棺������ʧЧ��");
					SetScreen(TIPS1PAGE_INDEX);
					
					//����
					if(pProjectMan->alarmEnable)
						SetBuzzer(100);
				}
				
			break;
			case PROJECTMODE_NONE:
				StepMotor_Stop(0);
				pProjectMan->smStopFlag = 0;
			break;
			case PROJECTMODE_MANUAL:
				
			break;
			case PROJECTMODE_TEST:
				
				cDebug("Project Test!\r\n");
			
				DDC112IF_PurpleLedEnable(1);		
				DDC112IF_SampleEnable(1);
				pProjectMan->sampleDataCount = 0;			

				while(pProjectMan->sampleDataCount < 350)
					vTaskDelay(10);
				
				DDC112IF_SampleEnable(0);
				DDC112IF_PurpleLedEnable(0);
				
				cDebug("pProjectMan->sampleDataCount = %d\r\n", pProjectMan->sampleDataCount);
				
				for(j=0;j<pProjectMan->sampleDataCount;j++) //���תС��
					EndianConvert(&pProjectMan->sampleData[j]);	

#if 1
				i = 0;
				DDC112IF_SendPCStart();
				for(j=0;j<pProjectMan->sampleDataCount;j++)
				{
					//cDebug("sampleData[%d] = 0x%06x\r\n", j, pProjectMan->sampleData[j]);

					DDC112IF_SendPC(j, pProjectMan->sampleData[j]);
					
					//pProjectMan->curveData[i] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)0xFEFFF); //180ΪҺ�������߿ؼ��ĸ߶ȣ�0xFEFFFΪDDC112�ɼ������ֵ
					pProjectMan->curveData[i++] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)(pProjectMan->sampleData[pProjectMan->maximumBufferIndex]));
				}
				DDC112IF_SendPCEnd();
#endif
				GraphChannelDataClear(CURVEPAGE_INDEX, CURVE_GRAPHY, 1);
				GraphChannelDataAdd(CURVEPAGE_INDEX, CURVE_GRAPHY, 1, (uint8_t*)(pProjectMan->curveData), 350);
				
				pProjectMan->projectMode = PROJECTMODE_NONE;
				
				if(pProjectMan->page == MANUALPAGE_INDEX)
					SetButtonValue(MANUALPAGE_INDEX, MANUAL_SAMPLE_BUTTON, 0);
			break;
			default:
			break;			
		}
	}
}

#ifdef __cplusplus
}
#endif
