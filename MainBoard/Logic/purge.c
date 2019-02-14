#ifdef __cplusplus	  
extern "C" {
#endif

#include "project.h"
#include "managerment.h"
#include "CPrintf.h"
#include "stdio.h"

#include "../HARDWARE/DCMotor/DCMotor.h"
#include "../HARDWARE/StepMotor/StepMotor.h"
#include "../HARDWARE/Beep/beep.h"

#include "FreeRTOS.h"
#include "task.h"
    
//��ȡ��Һ
void wasteFluidAbsorb(void)
{
	uint8_t relCoord;

	//״̬��ʾ������
	if(pProjectMan->lang == 0)
	{
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, (uint8_t*)"In absorbing waste fluid����");
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"In absorbing waste fluid����");
	}
	else
	{
		SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, (uint8_t*)"��ȡ��Һ�С���");
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"��ȡ��Һ�С���");
	}

	//����Һ��ת����Һ��λ��
	relCoord = StepMotor_Abs2Rel(STEPMOTOR_MAIN, AbsCoordinate[8]); //��Һ��λ��
	StepMotor_RelativePosition(STEPMOTOR_MAIN, relCoord, AbsCoordinate[9]); //����Һ��ת����Һ��

	//������ձ�
	//pDCMotor->SetCMD(PUMP_VACUUM, ENABLE);
    DCMotor_Run(PUMP_VACUUM, CW, 100);
	//�ɿ��йܷ�
	//pDCMotor->SetSpeed(PUMP_PINCH, 0);
    DCMotor_Run(PUMP_PINCH, CW, 0);

	//��ʱ����շ�Һƿ�Ŀ����γ���ѹ��
	vTaskDelay(255*6);

	//���·�Һ��
	DCMotor_WastePump_SetPos(DOWN);

	//��ʱһС��ʱ��
	vTaskDelay(255*13);

	//�����Һ��
	DCMotor_WastePump_SetPos(UP);

	//��ʱ����ȡʣ�µ�Һ��
	vTaskDelay(255*6);

	//�رռйܷ�
	//pDCMotor->SetSpeed(PUMP_PINCH, 100);
    DCMotor_Run(PUMP_PINCH, CW, 100);
	//ֹͣ��ձ�
	//pDCMotor->SetCMD(PUMP_VACUUM, DISABLE);
    DCMotor_Run(PUMP_VACUUM, CW, 0);

	//״̬��ʾ������
	SetTextValue(RUNNINGPAGE_INDEX, RUNNING_STATUS_EDIT, (uint8_t*)"");
	SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"");
}


 //��ϴ����
void PurgeTask(void)
{
	uint16_t i;
	uint8_t relCoord;
	uint8_t str[50];
				
	if(pProjectMan->purgePumpSel == 0x00)
	{
		SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //���ð���Ϊ����״̬
		return;
	}

	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0);
	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP1_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP2_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP3_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP4_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP5_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP6_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP7_BUTTON, 0);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP8_BUTTON, 0);

	if(pProjectMan->lang == 0)
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"In preparing����");
	else
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"׼���С���");

	//��ʾ��Ҫ��ϴ�Ĺܵ���������ˮ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"Please place the tube that to be washed in the distilled water");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"�뽫Ҫ��ϴ�Ĺܵ���������ˮ��");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//��ԭ��
	StepMotor_Home(STEPMOTOR_MAIN);

	//���ֶ����ԭλ
	relCoord = StepMotor_Abs2Rel(STEPMOTOR_MAIN, AbsCoordinate[9]);
	StepMotor_RelativePosition(STEPMOTOR_MAIN, relCoord, AbsCoordinate[9]); //AbsCoordinate[9]Ϊ�ֶ���

	//��ʾ����Һ�����ڲ۰��1��λ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ1ҳ��
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"Please place the Waste Tank to the plate1");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"�뽫��Һ�����ڡ����1��λ��");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	if(pProjectMan->lang == 0)	
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"Preparation finish!");
	else
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"׼����ɣ�");

	//��ϴҪ��ϴ�Ĺܵ�
	//pStepMotor->offset = STEPMOTOR_PUMP_OFFSET;
	pStepMotor->offset = pProjectMan->posCali2;
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			if(pProjectMan->lang == 0)
				sprintf((char*)str, "Washing the tube %d ����", i+1);
			else
				sprintf((char*)str, "��ϴ���ܵ�%d���С���", i+1);
			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)str);

			//����Һ��ת����Ӧ�ı�λ��
			relCoord = StepMotor_Abs2Rel(STEPMOTOR_MAIN, AbsCoordinate[i]);
			StepMotor_RelativePosition(STEPMOTOR_MAIN, relCoord, AbsCoordinate[9]); //AbsCoordinate[8]Ϊ��Һ��

			//pDCMotor->SetSpeed((DCMotorEnum_TypeDef)i, 10);
			//pDCMotor->SetCMD((DCMotorEnum_TypeDef)i, ENABLE);
            DCMotor_Run((DCMotorEnum_TypeDef)i, CW, 10);            
			vTaskDelay(255*3);
			//pDCMotor->SetCMD((DCMotorEnum_TypeDef)i, DISABLE);
            DCMotor_Stop((DCMotorEnum_TypeDef)i);

			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"");
		}
	}
	//pStepMotor->offset = STEPMOTOR_OFFSET;
	pStepMotor->offset = pProjectMan->posCali1;
	
	//��ȡ��Һ
	wasteFluidAbsorb();

	//��ʾ��Ҫ��ϴ�Ĺܵ����������ﴦ
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"Please place the tubes that to be washed in the clean and dry place");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"�뽫Ҫ��ϴ�Ĺܵ����������ﴦ");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
		
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//���Ҫ��ϴ�Ĺܵ�
	//pStepMotor->offset = STEPMOTOR_PUMP_OFFSET;
	pStepMotor->offset = pProjectMan->posCali2;
	for(i=0;i<8;i++)
	{
		if(pProjectMan->purgePumpSel & (0x01<<i))
		{
			if(pProjectMan->lang == 0)
				sprintf((char*)str, "Pumping water from the tube %d", i+1);
			else
				sprintf((char*)str, "��ա��ܵ�%d���С���", i+1);
			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)str);

			//����Һ��ת����Ӧ�ı�λ��
			relCoord = StepMotor_Abs2Rel(STEPMOTOR_MAIN, AbsCoordinate[i]);
			StepMotor_RelativePosition(STEPMOTOR_MAIN, relCoord, AbsCoordinate[9]); //AbsCoordinate[8]Ϊ3�Ųۣ�����Һ�ۣ�

			DCMotor_Run((DCMotorEnum_TypeDef)i, CW, 10);			
			vTaskDelay(255*3);
			DCMotor_Stop((DCMotorEnum_TypeDef)i);

			SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"");
		}
	}
	//pStepMotor->offset = STEPMOTOR_OFFSET;
	pStepMotor->offset = pProjectMan->posCali1;

	//��ȡ��Һ
	wasteFluidAbsorb();

	//����Һ��ת���ֶ���
	relCoord = StepMotor_Abs2Rel(STEPMOTOR_MAIN, AbsCoordinate[9]); //�ֶ���λ��
	StepMotor_RelativePosition(STEPMOTOR_MAIN, relCoord, AbsCoordinate[9]); //����Һ��ת���ֶ���

	//��ʾȡ�߷�Һ��
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"Please take away the Waste Tank");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"�����߷�Һ��");
	
	beepAlarm(1);	
	while(pProjectMan->tipsButton == TIPS_NONE);
	
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	//��ʾ��ϴ���
	SetScreen(TIPS1PAGE_INDEX);//��ת����ʾ2ҳ��
	if(pProjectMan->lang == 0)
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"Wash finish!");
	else
		SetTextValue(TIPS1PAGE_INDEX, TIPS1_TIPS_EDIT, (uint8_t*)"��ϴ���!");

	beepAlarm(1);
	while(pProjectMan->tipsButton == TIPS_NONE);
	
	pProjectMan->tipsButton = TIPS_NONE;
	SetScreen(PURGEPAGE_INDEX);//��ת����ϴҳ��

	if(pProjectMan->lang == 0)
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"Wash finish!");
	else
		SetTextValue(PURGEPAGE_INDEX, PURGE_STATUS_EDIT, (uint8_t*)"��ϴ���!");

	SetButtonValue(PURGEPAGE_INDEX, PURGE_START_BUTTON, 0x00); //���ð���Ϊ����״̬
	SetControlVisiable(PURGEPAGE_INDEX, PURGE_START_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP1_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP2_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP3_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP4_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP5_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP6_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP7_BUTTON, 1);
	SetControlEnable(PURGEPAGE_INDEX, PURGE_PUMP8_BUTTON, 1);
    
    //ɾ���Լ�
    vTaskDelete( NULL );
}

#ifdef __cplusplus
}
#endif
