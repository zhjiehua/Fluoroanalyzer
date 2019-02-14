#include "hmi_driver.h"
#include "hmi_user_uart.h"
#include "cmd_queue.h"
#include "cmd_process.h"
#include "stdio.h"
#include "misc.h"

#include "../User/CPrintf.h"
#include "../UILogic/pageCommon.h"
#include "../Logic/management.h"
#include "../Logic/QRCodeData.h"
#include "miscellaneous.h"

#include "hw_config.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif


	
/*! 
 *  \brief  ��Ϣ�������̣��˴�һ�㲻��Ҫ����
 *  \param msg ��������Ϣ
 *  \param size ��Ϣ����
 */
void ProcessMessage( PCTRL_MSG msg, uint16 size )
{
	uint8 cmd_type = msg->cmd_type;//ָ������
	uint8 ctrl_msg = msg->ctrl_msg;   //��Ϣ������
	uint8 control_type = msg->control_type;//�ؼ�����
	uint16 screen_id = PTR2U16(&msg->screen_id_high);//����ID
	uint16 control_id = PTR2U16(&msg->control_id_high);//�ؼ�ID
	uint32 value = PTR2U32(msg->param);//��ֵ

	switch(cmd_type)
	{		
	case NOTIFY_TOUCH_PRESS://����������
	case NOTIFY_TOUCH_RELEASE://�������ɿ�
		//NotifyTouchXY(cmd_buffer[1],PTR2U16(cmd_buffer+2),PTR2U16(cmd_buffer+4));
		break;	
	case NOTIFY_WRITE_FLASH_OK://дFLASH�ɹ�
		NotifyWriteFlash(1);
		break;
	case NOTIFY_WRITE_FLASH_FAILD://дFLASHʧ��
		NotifyWriteFlash(0);
		break;
	case NOTIFY_READ_FLASH_OK://��ȡFLASH�ɹ�
		//NotifyReadFlash(1,cmd_buffer+2,size-6);//ȥ��֡ͷ֡β
		break;
	case NOTIFY_READ_FLASH_FAILD://��ȡFLASHʧ��
		//NotifyReadFlash(0,0,0);
		break;
	case NOTIFY_READ_RTC://��ȡRTCʱ��
		//NotifyReadRTC(cmd_buffer[1],cmd_buffer[2],cmd_buffer[3],cmd_buffer[4],cmd_buffer[5],cmd_buffer[6],cmd_buffer[7]);
		NotifyReadRTC(hmi_cmd_buffer[2],hmi_cmd_buffer[3],hmi_cmd_buffer[4],hmi_cmd_buffer[5],hmi_cmd_buffer[6],hmi_cmd_buffer[7],hmi_cmd_buffer[8]);
		break;
	case NOTIFY_CONTROL:
		{
			if(ctrl_msg==MSG_GET_CURRENT_SCREEN)//����ID�仯֪ͨ
			{
				NotifyScreen(screen_id);
			}
			else
			{
				switch(control_type)
				{
				case kCtrlButton: //��ť�ؼ�
					//cDebug("NotifyButton(), %d, %d, %d\n", screen_id, control_id, msg->param[1]);
					NotifyButton(screen_id,control_id,msg->param[1]);
					break;
				case kCtrlText://�ı��ؼ�
					//cDebug("NotifyText(), %d, %d, %s\n", screen_id, control_id, msg->param);
					NotifyText(screen_id,control_id,msg->param);
					break;
				case kCtrlProgress: //�������ؼ�
					NotifyProgress(screen_id,control_id,value);
					break;
				case kCtrlSlider: //�������ؼ�
					NotifySlider(screen_id,control_id,value);
					break;
				case kCtrlMeter: //�Ǳ�ؼ�
					NotifyMeter(screen_id,control_id,value);
					break;
				case kCtrlMenu://�˵��ؼ�
					//cDebug("NotifyMenu(), %d, %d, %d, %d\n", screen_id, control_id, msg->param[0], msg->param[1]);
					NotifyMenu(screen_id,control_id,msg->param[0],msg->param[1]);
					break;
				case kCtrlSelector://ѡ��ؼ�
					NotifySelector(screen_id,control_id,msg->param[0]);
					break;
				case kCtrlRTC://����ʱ�ؼ�
					NotifyTimer(screen_id,control_id);
					break;
				default:
					break;
				}
			}			
		}
		break;
	case NOTIFY_RESET:
		cDebug("NOTIFY_RESET\n");
		NotifyReset();
		break;
	case NOTIFY_ONLINE:
		cDebug("NOTIFY_ONLINE\n");
		
		pProjectMan->lcdNotifyResetFlag = 1;
		pProjectMan->page = MAINPAGE_INDEX;
		SetScreen(pProjectMan->page);
		
		break;
	case NOTIFY_UNLOCK:
		SetTouchPaneOption(1, pProjectMan->beeperEnable, 1, 1);
		LockDeviceConfig();
		break;

	//USB��������
	case MSG_USBUART_DATA:
	{
		uint8_t i;
		for(i=0;i<size;i++)
			USB_USART_SendData(((uint8_t*)msg)[i]);
	}
	break;
	
	//��ά������
	case MSG_QRCODE_DATA: 
		QRCode_Data_Parse((uint8*)(&msg->ctrl_msg), size-6);
		break;
	
	//�û�����
	case MSG_USERUART_DATA: 
		User_Data_Parse((uint8*)(&msg->ctrl_msg), size-6);
		break;
	
	default:
		break;
	}
}

//�ı��ؼ���ʾ����ֵ
void SetTextValueInt32(uint16 screen_id, uint16 control_id,int32 value)
{
	char buffer[12] = {0};
	sprintf(buffer,"%d",value);
	SetTextValue(screen_id,control_id,(uchar *)buffer);
}

//�ַ���ת����
int32 StringToInt32(uint8 *str)
{
	int32 v = 0;
	sscanf((const char *)str,"%d",&v);
	return v;
}

//�ı��ؼ���ʾ������
void SetTextValueFloat(uint16 screen_id, uint16 control_id, float value)
{
	char buffer[12] = {0};
	sprintf(buffer,"%.1f",value);
	SetTextValue(screen_id,control_id,(uchar *)buffer);
}

//�ַ���ת����
float StringToFloat(uint8 *str)
{
	double v = 0;
	sscanf((const char *)str,"%lf",&v);
	return v;
}

/*! 
 *  \brief  ��������λ֪ͨ
 *  \details  �������ϵ縴λ���߱�����λ��ִ�д˺���
 */
void NotifyReset(void)
{
	//TODO: ����û�����
	SetHandShake();//������������
}

/*! 
 *  \brief  �����л�֪ͨ
 *  \details  ��ǰ����ı�ʱ(�����GetScreen)��ִ�д˺���
 *  \param screen_id ��ǰ����ID
 */
void NotifyScreen(uint16 screen_id)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ���������¼���Ӧ
 *  \param press 1���´�������3�ɿ�������
 *  \param x x����
 *  \param y y����
 */
void NotifyTouchXY(uint8 press,uint16 x,uint16 y)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ť�ؼ�֪ͨ
 *  \details  ����ť״̬�ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param state ��ť״̬��0����1����
 */
void NotifyButton(uint16 screen_id, uint16 control_id, uint8 state)
{
	//TODO: ����û�����
	switch(screen_id)
	{
		case LOGOPAGE_INDEX:
			logoPageButtonProcess(control_id, state);
		break;
		case MAINPAGE_INDEX:
			mainPageButtonProcess(control_id, state);
		break;
		case FASTTESTPAGE_INDEX:
			fastTestPageButtonProcess(control_id, state);
		break;
		case CALIBPAGE_INDEX:
			calibPageButtonProcess(control_id, state);
		break;
		case RESULTPAGE_INDEX:
			resultPageButtonProcess(control_id, state);
		break;
		case SETUPPAGE_INDEX:
			setupPageButtonProcess(control_id, state);
		break;
		case PARAMETERPAGE_INDEX:
			parameterPageButtonProcess(control_id, state);
		break;
		case CURVEPAGE_INDEX:
			curvePageButtonProcess(control_id, state);
		break;
		case TIPS1PAGE_INDEX:
			tips1PageButtonProcess(control_id, state);
		break;
		case MANUALPAGE_INDEX:
			manualPageButtonProcess(control_id, state);
		break;
		case PROMANPAGE_INDEX:
			proManPageButtonProcess(control_id, state);
		break;
		default:
			cDebug("cmd_process NotifyButton error!\n");
		break;
	}
}

/*! 
 *  \brief  �ı��ؼ�֪ͨ
 *  \details  ���ı�ͨ�����̸���(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param str �ı��ؼ�����
 */
void NotifyText(uint16 screen_id, uint16 control_id, uint8 *str)
{
	//TODO: ����û�����
	switch(screen_id)
	{
		case FASTTESTPAGE_INDEX:
			fastTestPageEditProcess(control_id, str);
		break;
		case CALIBPAGE_INDEX:
			calibPageEditProcess(control_id, str);
		break;
		case RESULTPAGE_INDEX:
			resultPageEditProcess(control_id, str);
		break;
		case PARAMETERPAGE_INDEX:
			parameterPageEditProcess(control_id, str);
		break;
		case PROMANPAGE_INDEX:
			proManPageEditProcess(control_id, str);
		break;
		default:
			cDebug("cmd_process NotifyText error!\n");
		break;
	}
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyProgress(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �������ؼ�֪ͨ
 *  \details  ���������ı�(�����GetControlValue)ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifySlider(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �Ǳ�ؼ�֪ͨ
 *  \details  ����GetControlValueʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param value ֵ
 */
void NotifyMeter(uint16 screen_id, uint16 control_id, uint32 value)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  �˵��ؼ�֪ͨ
 *  \details  ���˵���»��ɿ�ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item �˵�������
 *  \param state ��ť״̬��0�ɿ���1����
 */
void NotifyMenu(uint16 screen_id, uint16 control_id, uint8  item, uint8  state)
{
	//TODO: ����û�����
	if(state == 1)
		return;

	switch(screen_id)
	{
		default:
			cDebug("cmd_process NotifyMenu error!\n");
		break;
	}
}

/*! 
 *  \brief  ѡ��ؼ�֪ͨ
 *  \details  ��ѡ��ؼ��仯ʱ��ִ�д˺���
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 *  \param item ��ǰѡ��
 */
void NotifySelector(uint16 screen_id, uint16 control_id, uint8  item)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ʱ����ʱ֪ͨ����
 *  \param screen_id ����ID
 *  \param control_id �ؼ�ID
 */
void NotifyTimer(uint16 screen_id, uint16 control_id)
{
	//TODO: ����û�����
	switch(screen_id)
	{
		default:
		break;
	}
}

/*! 
 *  \brief  ��ȡ�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 *  \param _data ��������
 *  \param length ���ݳ���
 */
void NotifyReadFlash(uint8 status,uint8 *_data,uint16 length)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  д�û�FLASH״̬����
 *  \param status 0ʧ�ܣ�1�ɹ�
 */
void NotifyWriteFlash(uint8 status)
{
	//TODO: ����û�����
}

/*! 
 *  \brief  ��ȡRTCʱ�䣬ע�ⷵ�ص���BCD��
 *  \param year �꣨BCD��
 *  \param month �£�BCD��
 *  \param week ���ڣ�BCD��
 *  \param day �գ�BCD��
 *  \param hour ʱ��BCD��
 *  \param minute �֣�BCD��
 *  \param second �루BCD��
 */
void NotifyReadRTC(uint8 year,uint8 month,uint8 week,uint8 day,uint8 hour,uint8 minute,uint8 second)
{
	//TODO: ����û�����
	sprintf((char*)pProjectMan->rtc, "20%02d/%02d/%02d %02d:%02d:%02d",
		BCDToInt(year), BCDToInt(month), BCDToInt(day),
		BCDToInt(hour), BCDToInt(minute), BCDToInt(second));
	
	SetTextValue(RESULTPAGE_INDEX, RESULT_DATA_EDIT, pProjectMan->rtc);
	
	//cDebug("rtc is %s\r\n", pProjectMan->rtc);
}

#ifdef __cplusplus
}
#endif
