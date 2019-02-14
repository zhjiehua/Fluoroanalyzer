#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../StepMotor/StepMotor.h"
#include "Printer/Printer.h"
#include "QRCode/QRCode.h"
#include "../DDC112/ddc112_interface.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void U16EndianConvert(uint16_t *pdata)
{
    uint8_t *p = (uint8_t*)pdata;
	uint8_t temp = *(p+1);
	*(p+1) = *p;
	*p = temp;
}
	
void resultPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case RESULT_RETURN_BUTTON:
			pProjectMan->projectMode = PROJECTMODE_NONE;
		
			DDC112IF_PurpleLedEnable(0);
		
			pProjectMan->page = FASTTESTPAGE_INDEX;
		break;
		case RESULT_CURVE_BUTTON:
		{
			uint8_t i;
			uint16_t x, y;
			char str[15];
			
//			if(pProjectMan->projectMode == PROJECTMODE_NONE)
//				pProjectMan->projectMode = PROJECTMODE_HOME;
			
//			pProjectMan->projectMode = PROJECTMODE_MANUAL;
//			StepMotor_SetSpeed(0, pProjectMan->sampleSpeed);
//			StepMotor_SetDir(0, CCW);
//			StepMotor_SetCMD(0, ENABLE);
			
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
			
//			for(i=0;i<pProjectMan->maximalBufferIndex;i++)
//			{
//				uint16_t x = pProjectMan->maximalBuffer[i] - pProjectMan->valideDataStartIndex;
//				uint16_t y = 180 - pProjectMan->curveData[x] + 40; //180为液晶屏曲线控件的高度
//				sprintf(str, "%d,%d\r\n", x, pProjectMan->sampleData[pProjectMan->maximalBuffer[i]]);
//				DisText(x, y-30, 1, 2, (uint8_t*)str);
//				GUI_CircleFill(x+2, y, 3);
//			}
			
				x = pProjectMan->axisTXCur;
				y = 180 - pProjectMan->curveData[x] + 40; //180为液晶屏曲线控件的高度
				sprintf(str, "%d,%d\r\n", x, pProjectMan->sampleData[pProjectMan->axisTXCur]);
				DisText(x, y-30, 1, 2, (uint8_t*)str);
				GUI_CircleFill(x+2, y, 3);

				x = pProjectMan->axisCXCur;
				y = 180 - pProjectMan->curveData[x] + 40; //180为液晶屏曲线控件的高度
				sprintf(str, "%d,%d\r\n", x, pProjectMan->sampleData[pProjectMan->axisCXCur]);
				DisText(x, y-30, 1, 2, (uint8_t*)str);
				GUI_CircleFill(x+2, y, 3);
		}	
		break;
		case RESULT_TRANS_BUTTON:
		{
//			pProjectMan->projectMode = PROJECTMODE_MANUAL;
//			StepMotor_SetSpeed(0, pProjectMan->sampleSpeed);
//			StepMotor_SetDir(0, CW);
//			StepMotor_SetCMD(0, ENABLE);
			
			uint16_t i, j;
			
			//SetTextValue(TIPSPAGE_INDEX, TIPS0_TIPS_EDIT, (uint8_t*)"传输中……");
			//SetScreen(TIPSPAGE_INDEX);
			
			DDC112IF_SendPCStart();
			i = 0;
			for(j=0;j<350;j++)
				DDC112IF_SendPC(j, pProjectMan->sampleData[j]); //sampleData  filterData			
			DDC112IF_SendPCEnd();
	
			SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"传输完成");
			SetScreen(TIPS1PAGE_INDEX);
		}			
		break;
		case RESULT_PRINT_BUTTON:
		{
			//SetTextValue(TIPSPAGE_INDEX, TIPS0_TIPS_EDIT, (uint8_t*)"打印中……");
			//SetScreen(TIPSPAGE_INDEX);
			
			if(pProjectMan->printerEnable && pProjectMan->gender)
			{
				ResultPrint();

				SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"打印完成");
				SetScreen(TIPS1PAGE_INDEX);
			}
			
//			DDC112IF_SendPCEnd();
			
//			StepMotor_Stop(0);
//			pProjectMan->projectMode = PROJECTMODE_NONE;
		}
		break;
		default:
			cDebug("resultPage BUTTON error!\n");
		break;
	}
}

void resultPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case RESULT_DATA_EDIT:  	
		case RESULT_TIME_EDIT:
		case RESULT_C_VALUE_EDIT:
		case RESULT_UNIT_EDIT:
		case RESULT_RESULT_EDIT:
		case RESULT_T_VALUE_EDIT:
		case RESULT_T_C_EDIT:
		case RESULT_CONCENT_EDIT:
		break;
		default:
			cDebug("resultPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
