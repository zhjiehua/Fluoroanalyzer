#ifdef __cplusplus	  
extern "C" {
#endif

#include "project.h"
#include "managerment.h"
#include "../User/CPrintf.h"
#include "stdio.h"

#include "../HARDWARE/DCMotor/DCMotor.h"
#include "../HARDWARE/StepMotor/StepMotor.h"
#include "../HARDWARE/RelayMOS/RelayMOS.h"
#include "../HARDWARE/Beep/beep.h"

#include "FreeRTOS.h"
#include "task.h"
    
void CalibrationTask(void)
{
	uint8_t i;
	uint8_t pumpSel = pProjectMan->caliPumpSel;
	float time = pProjectMan->pCaliPumpPara[pumpSel];
	//cDebug("========caliPage start to run the PUMP program!\n");

//	cDebug("pProjectMan->caliPumpSel = %d\n", (uint16_t)pProjectMan->caliPumpSel);
//	pDCMotor->SetCMD(pProjectMan->caliPumpSel, ENABLE);
//	os_wait(K_TMO, 1000, 0);
//	pDCMotor->SetCMD(pProjectMan->caliPumpSel, DISABLE);


	//��ʾ�Ƿ��ע�ܵ�
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(TIPS2PAGE_INDEX);//��ת����ʾ2ҳ��
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, (uchar*)"Whether fill the tube?");
	else
		SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, (uchar*)"�Ƿ��ע�ܵ���");

	beepAlarm(1);
    
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	if(pProjectMan->tipsButton == TIPS_CANCEL)
	{
		pProjectMan->tipsButton = TIPS_NONE;
		SetScreen(CALIBRATIONPAGE_INDEX);//��ת��У׼ҳ��
	}
	else
	{
        DCMotor_Run((DCMotorEnum_TypeDef)pumpSel, CW, 10);
		vTaskDelay(380);				
		DCMotor_Stop((DCMotorEnum_TypeDef)pumpSel);
	
		while(1)
		{
			//��ʾ�Ƿ������ע�ܵ�
			SetScreen(TIPS2PAGE_INDEX);//��ת����ʾ2ҳ��
			if(pProjectMan->lang == 0)
				SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, (uchar*)"Whether continue fill the tube?");
			else
				SetTextValue(TIPS2PAGE_INDEX, TIPS2_TIPS_EDIT, (uchar*)"�Ƿ������ע�ܵ���");
			
			beepAlarm(1);
			while(pProjectMan->tipsButton == TIPS_NONE);
				
			SetScreen(CALIBRATIONPAGE_INDEX);//��ת������ҳ��
			if(pProjectMan->tipsButton == TIPS_CANCEL)
			{
				pProjectMan->tipsButton = TIPS_NONE;
				break;	
			}
			else
			{
				pProjectMan->tipsButton = TIPS_NONE;
				//������ע�ܵ�
                DCMotor_Run((DCMotorEnum_TypeDef)pumpSel, CW, 10);
                vTaskDelay(125);				
                DCMotor_Stop((DCMotorEnum_TypeDef)pumpSel);
			}
		}		
	}

    vTaskDelay(1000);

	//��ʼУ׼
    DCMotor_Run((DCMotorEnum_TypeDef)pProjectMan->caliPumpSel, CW, 10);
	//ֱ���������40�ε�1mL�����Թ�40mL
	for(i=0;i<40;i++)
		vTaskDelay(time);
	DCMotor_Stop((DCMotorEnum_TypeDef)pProjectMan->caliPumpSel);
    
	SetControlEnable(CALIBRATIONPAGE_INDEX, CALI_PUMPSELECT_BUTTON, 1);
	SetControlEnable(CALIBRATIONPAGE_INDEX, CALI_START_BUTTON, 1);
}

#ifdef __cplusplus
}
#endif
