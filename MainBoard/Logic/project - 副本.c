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

	ResetDevice();//复位串口屏
	
	while(!pProjectMan->lcdNotifyResetFlag)
	{
		vTaskDelay(100);
		
//		if(j++ > 1000)
//			ResetDevice();//复位串口屏
	}
	
	//GraphChannelAdd(CURVEPAGE_INDEX, CURVE_GRAPHY, 0, 0xFFFF);

	//DDC112参数初始化
	DDC112IF_RangeSetting(1);
	DDC112IF_IntegralTimeSetting(25); //14*500us
	DDC112IF_BufferSizeSetting(2);
//	DDC112IF_IntegralTimeSetting(11);
//	DDC112IF_BufferSizeSetting(2);	
	
	while(1)
	{

		vTaskDelay(10);

		LED2Task(); //LED2闪烁指示PROJECT线程正在运行

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
				SetTextValue(TIPSPAGE_INDEX, TIPS0_TIPS_EDIT, (uint8_t*)"检测中……");
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
				
				for(j=0;j<pProjectMan->sampleDataCount;j++) //大端转小端
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
				
				//滤波
#define FILTER_ORDER	12
				
				//PC只需要350个点，从采集到的点中数350个点发PC，如果不够350个点，则补齐
				if(pProjectMan->sampleDataCount < 350+2*FILTER_ORDER)
				{
					for(i=pProjectMan->sampleDataCount+1;i<350+2*FILTER_ORDER;i++)
						pProjectMan->sampleData[i] = pProjectMan->sampleData[pProjectMan->sampleDataCount];
					
					pProjectMan->sampleDataCount = 350+2*FILTER_ORDER;
					
//					//数据无效
//					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"警告：采集模块错误！");
//					SetScreen(TIPS1PAGE_INDEX);
				}
				//pProjectMan->sampleDataCount = 350+2*FILTER_ORDER;				

				for(j=0;j<pProjectMan->sampleDataCount;j++) //第一次滤波
				{
					pProjectMan->filterData[j] = OrderFilter(pProjectMan->sampleData+j, (j>=FILTER_ORDER ? FILTER_ORDER : j+1));
					
					//cDebug("sampleData[%d] = 0x%06x\r\n", j, pProjectMan->sampleData[j]);
					//cDebug("filterData[%d] = 0x%06x\r\n", j, pProjectMan->filterData[j]);
				}
				
				for(j=0;j<pProjectMan->sampleDataCount;j++) //第二次滤波
				{
					pProjectMan->sampleData[j] = OrderFilter(pProjectMan->filterData+j, (j>=FILTER_ORDER ? FILTER_ORDER : j+1));
					//pProjectMan->sampleData[j] = pProjectMan->filterData[j];
				}
				
				//倒序
				for(j=0;j<pProjectMan->sampleDataCount/2;j++)
				{
					temp = pProjectMan->sampleData[j];
					pProjectMan->sampleData[j] = pProjectMan->sampleData[pProjectMan->sampleDataCount - j];
					pProjectMan->sampleData[pProjectMan->sampleDataCount - j] = temp;
				}
				
				//取中间350个点
				for(j=pProjectMan->sampleDataCount/2-176;j<pProjectMan->sampleDataCount/2+176;j++)
				{
					pProjectMan->sampleData[j-pProjectMan->sampleDataCount/2+176] = pProjectMan->sampleData[j];
				}
				
				pProjectMan->sampleDataCount = 350;
				
				
				
				
				//找极大点
				//pProjectMan->lineNumber = 2;
				//pProjectMan->checkWindowAxisX = 30;
				temp = pProjectMan->sampleDataCount/pProjectMan->lineNumber;
				
//				pProjectMan->axisTXCur = FindMaximal(pProjectMan->sampleData, temp); //找第一个极大点
//				pProjectMan->axisCXCur = FindMaximal(pProjectMan->sampleData+temp, temp); //找第二个极大点
//				pProjectMan->axisCXCur += temp;
				pProjectMan->axisCXCur = FindMaximin(pProjectMan->sampleData, temp); //找第一个极大点
				pProjectMan->axisTXCur = FindMaximin(pProjectMan->sampleData+temp, temp); //找第二个极大点
				pProjectMan->axisTXCur += temp;
				
				cDebug("pProjectMan->axisTXCur = %d\r\n", pProjectMan->axisTXCur);
				cDebug("pProjectMan->axisCXCur = %d\r\n", pProjectMan->axisCXCur);
				
				//求最小二乘法参数
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
				
				//求最小二乘法参数
				LeastSquare(pProjectMan->filterData, pProjectMan->sampleDataCount, &pProjectMan->LeastSquareA, &pProjectMan->LeastSquareB);
				pProjectMan->cThreshod = pProjectMan->LeastSquareA*pProjectMan->axisCXCur + pProjectMan->LeastSquareB;
				pProjectMan->tThreshod = pProjectMan->LeastSquareA*pProjectMan->axisTXCur + pProjectMan->LeastSquareB;
				
				cDebug("pProjectMan->LeastSquareA = %f\r\n", pProjectMan->LeastSquareA);
				cDebug("pProjectMan->LeastSquareB = %f\r\n", pProjectMan->LeastSquareB);
				cDebug("pProjectMan->cThreshod = %d\r\n", pProjectMan->cThreshod);
				cDebug("pProjectMan->tThreshod = %d\r\n", pProjectMan->tThreshod);
				
				//判断结果
				if(pProjectMan->sampleData[pProjectMan->axisCXCur] < pProjectMan->cThreshod)
				{
					pProjectMan->gender = 0;
					
					//数据无效
					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"数据无效");
					SetScreen(TIPS1PAGE_INDEX);
				}
				else if((pProjectMan->sampleData[pProjectMan->axisTXCur] < pProjectMan->tThreshod)
					&& (abs(pProjectMan->axisCXCur - pProjectMan->axisCX) <= pProjectMan->checkWindowAxisX))
				{	
					pProjectMan->cValue = GetCTValue(pProjectMan->axisCXCur);
					
					//阴性
					pProjectMan->gender = 2;
				}		
				else if((abs(pProjectMan->axisTXCur - pProjectMan->axisTX) > pProjectMan->checkWindowAxisX)
						|| (abs(pProjectMan->axisCXCur - pProjectMan->axisCX) > pProjectMan->checkWindowAxisX))
				{
					//
					pProjectMan->gender = 0;
					
					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"偏移范围过大");
					SetScreen(TIPS1PAGE_INDEX);
				}
				else
				{
					pProjectMan->cValue = GetCTValue(pProjectMan->axisCXCur);
					pProjectMan->tValue = GetCTValue(pProjectMan->axisTXCur);
					
					pProjectMan->tcValue = (float)pProjectMan->tValue/(float)pProjectMan->cValue;
					
					//阳性
					pProjectMan->gender = 1;
				}
				
				
				
				//更新控件
				if(pProjectMan->gender == 0) //无效
				{
					SetTextValue(RESULTPAGE_INDEX, RESULT_T_VALUE_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_C_VALUE_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_T_C_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_UNIT_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_RESULT_EDIT, (uint8_t*)"");
					SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, (uint8_t*)"");

					//报警
					if(pProjectMan->alarmEnable)
						SetBuzzer(100);
				}
				else //正常
				{
					uint8_t tempStr[10];
					
					sprintf((char*)tempStr, "%d", pProjectMan->cValue);
					SetTextValue(RESULTPAGE_INDEX, RESULT_C_VALUE_EDIT, tempStr);
					
					SetTextValue(RESULTPAGE_INDEX, RESULT_UNIT_EDIT, pProjectMan->concentrationUnit);
					
					if(pProjectMan->gender == 2) //阴性
					{
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_VALUE_EDIT, (uint8_t*)"");
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_C_EDIT, (uint8_t*)"");
						
						SetTextValue(RESULTPAGE_INDEX, RESULT_RESULT_EDIT, (uint8_t*)"-");
						SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, (uint8_t*)"");
					}
					else //阳性
					{
						sprintf((char*)tempStr, "%d", pProjectMan->tValue);
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_VALUE_EDIT, tempStr);
						
						sprintf((char*)tempStr, "%.04f", pProjectMan->tcValue);
						SetTextValue(RESULTPAGE_INDEX, RESULT_T_C_EDIT, tempStr);
						
						SetTextValue(RESULTPAGE_INDEX, RESULT_RESULT_EDIT, (uint8_t*)"+");
						
						//输出浓度
						switch(pProjectMan->fittingModel)
						{
							case 0://直线拟合  y = A*x + B
								pProjectMan->concentration = pProjectMan->fittingPara1*pProjectMan->tcValue
									+ pProjectMan->fittingPara2;
							break;
							case 1://二次多项式  y = A*x^2 + B*x + C
								pProjectMan->concentration = pProjectMan->fittingPara1*pow(pProjectMan->tcValue, 2)
									+ pProjectMan->fittingPara2*pProjectMan->tcValue + pProjectMan->fittingPara3;
							break;
							case 2://三次多项式  y = A*x^3 + B*x^2 + C*x + D
								pProjectMan->concentration = pProjectMan->fittingPara1*pow(pProjectMan->tcValue, 3)
									+ pProjectMan->fittingPara2*pow(pProjectMan->tcValue, 2)
									+ pProjectMan->fittingPara3*pProjectMan->tcValue + pProjectMan->fittingPara4;
							break;
							case 3://logit-log直线回归  A*lg(y) + B = ln((x/C)/(1-x/C))
							{
								float p = pProjectMan->tcValue/pProjectMan->fittingPara3;
								float Y = log(p/(1-p)); //等式右边, ln公式在C语言中用log，lg公式用log10
								float lgy = (Y - pProjectMan->fittingPara2)/pProjectMan->fittingPara1;
								
								pProjectMan->concentration = pow(10, lgy);
							}
							break;
							case 4://四参数拟合回归  y = (A - D)/(1 + (x/C)^B) + D
								pProjectMan->concentration = (pProjectMan->fittingPara1 - pProjectMan->fittingPara4)
									/ (1 + pow(pProjectMan->tcValue/pProjectMan->fittingPara3, pProjectMan->fittingPara2) + pProjectMan->fittingPara4);
							break;
							case 5://三次样条插值
								pProjectMan->concentration = pProjectMan->tcValue;
							break;
							case 6://点对点计算
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
							else//超出检测浓度
							{
								pProjectMan->gender = 0;
								
								SetTextValue(RESULTPAGE_INDEX, RESULT_CONCENT_EDIT, (uint8_t*)"");
								
								SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"超出检测浓度");
								SetScreen(TIPS1PAGE_INDEX);
								
								//报警
								if(pProjectMan->alarmEnable)
									SetBuzzer(100);
							}
						}
					}
						
					//自动打印
					if(pProjectMan->printerEnable && pProjectMan->gender)
						ResultPrint();
					
					pProjectMan->page = RESULTPAGE_INDEX;
					SetScreen(pProjectMan->page);
				}

				//找最大值下标
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
					
					//pProjectMan->curveData[i] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)0xFEFFF); //180为液晶屏曲线控件的高度，0xFEFFF为DDC112采集的最大值
					pProjectMan->curveData[i] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)(pProjectMan->sampleData[pProjectMan->maximumBufferIndex]));
					//cDebug("pProjectMan->curveData[%d] = 0x%02X\r\n", i, pProjectMan->curveData[i]);
					i++;
				}
				//DDC112IF_SendPCEnd();
				
				GraphChannelDataClear(CURVEPAGE_INDEX, CURVE_GRAPHY, 1);
				GraphChannelDataAdd(CURVEPAGE_INDEX, CURVE_GRAPHY, 1, (uint8_t*)(pProjectMan->curveData), 350);
			
				//if(pProjectMan->sampleStatus&SAMPLESTATUS_READY)
					pProjectMan->projectMode = PROJECTMODE_NONE;
				
				//清样品编号
				sprintf((char*)pProjectMan->sampleString, "");
				SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_SAMPLENUM_EDIT, pProjectMan->sampleString);
				
				//检查是否失效
				if(strncmp((const char*)pProjectMan->invalideData, (const char*)pProjectMan->rtc, 10) <= 0)
				{
					SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"警告：卡带已失效！");
					SetScreen(TIPS1PAGE_INDEX);
					
					//报警
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
				
				for(j=0;j<pProjectMan->sampleDataCount;j++) //大端转小端
					EndianConvert(&pProjectMan->sampleData[j]);	

#if 1
				i = 0;
				DDC112IF_SendPCStart();
				for(j=0;j<pProjectMan->sampleDataCount;j++)
				{
					//cDebug("sampleData[%d] = 0x%06x\r\n", j, pProjectMan->sampleData[j]);

					DDC112IF_SendPC(j, pProjectMan->sampleData[j]);
					
					//pProjectMan->curveData[i] = (uint8_t)((float)pProjectMan->sampleData[j]*(float)180/(float)0xFEFFF); //180为液晶屏曲线控件的高度，0xFEFFF为DDC112采集的最大值
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
