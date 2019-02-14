#include "cmd_queue.h"
#include "hmi_user_uart.h"
#include "HardwareCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//����ʵ�ʻ�����
qdata hmi_que_buffer[HMI_QUEUE_MAX_SIZE];
qdata ddc112_que_buffer[DDC112_QUEUE_MAX_SIZE];
qdata user_que_buffer[USER_QUEUE_MAX_SIZE];
qdata usb_que_buffer[USB_QUEUE_MAX_SIZE];
	
//ָ�����	
QUEUE hmi_que;  
QUEUE ddc112_que;
QUEUE user_que;
QUEUE usb_que;
	
void queue_init(QUEUE *que, qdata *buffer, qsize size)
{
	que->_head = que->_tail = 0;
	
	que->_data = buffer;
	que->_size = size;
	
	que->_cmd_pos = 0;
}

void queue_push(QUEUE *que, qdata _data)
{
	qsize pos = (que->_head+1)%que->_size;
	if(pos!=que->_tail)//����״̬
	{
		que->_data[que->_head] = _data;
		que->_head = pos;
	}
}

//�Ӷ�����ȡһ������
void queue_pop(QUEUE *que, qdata* _data)
{
	if(que->_tail!=que->_head)//�ǿ�״̬
	{
		*_data = que->_data[que->_tail];
		que->_tail = (que->_tail+1)%que->_size;
	}
}

//��ȡ��������Ч���ݸ���
qsize queue_size(QUEUE *que)
{
	return ((que->_head+que->_size-que->_tail)%que->_size);
}

static qsize hmi_queue_find_cmd(QUEUE *que, qdata *buffer, qsize buf_len)
{
	static uint32_t cmd_state = 0;
	qsize cmd_size = 0;
	qdata _data = 0;
	while(queue_size(que)>0)
	{
		//ȡһ������
		queue_pop(que, &_data);

		if(que->_cmd_pos==0&&_data!=HMI_CMD_HEAD)//ָ���һ���ֽڱ�����֡ͷ����������
			continue;

		if(que->_cmd_pos<buf_len)//��ֹ���������
			buffer[que->_cmd_pos++] = _data;

		cmd_state = ((cmd_state<<8)|_data);//ƴ�����4���ֽڣ����һ��32λ����

		//���4���ֽ���֡βƥ�䣬�õ�����֡
		if(cmd_state==HMI_CMD_TAIL)
		{
			cmd_size = que->_cmd_pos; //ָ���ֽڳ���
			cmd_state = 0;  //���¼��֡β��
			que->_cmd_pos = 0; //��λָ��ָ��

#if(HMI_CRC16_ENABLE)
			//ȥ��ָ��ͷβEE��βFFFCFFFF����5���ֽڣ�ֻ�������ݲ���CRC
			if(!CheckCRC16(buffer+1,cmd_size-5))//CRCУ��
				return 0;

			cmd_size -= 2;//ȥ��CRC16��2�ֽڣ�
#endif

			return cmd_size;
		}
	}
	return 0; //û���γ�������һ֡
}

static qsize ddc112_queue_find_cmd(QUEUE *que, qdata *buffer, qsize buf_len)
{
	qsize cmd_size = 0;
	qdata _data = 0;
	while(queue_size(que)>0)
	{
		//ȡһ������
		queue_pop(que, &_data);

		if(que->_cmd_pos==0&&((_data&0xF0)!=0x80))//ָ���һ���ֽڱ�����֡ͷ����������
			continue;

		if(que->_cmd_pos<buf_len)//��ֹ���������
			buffer[que->_cmd_pos++] = _data;

		//���4���ֽ���֡βƥ�䣬�õ�����֡
		if(que->_cmd_pos>=5)
		{
			cmd_size = que->_cmd_pos; //ָ���ֽڳ���
			que->_cmd_pos = 0; //��λָ��ָ��

			return cmd_size;
		}
	}
	return 0; //û���γ�������һ֡
}

qsize queue_find_cmd(QUEUE *que, qdata *buffer, qsize buf_len)
{
	if(que == &hmi_que)
    {
		return hmi_queue_find_cmd(que, buffer, buf_len);
    }
	else if(que == &ddc112_que)
	{
		return ddc112_queue_find_cmd(que, buffer, buf_len);
	}
    else if(que == &user_que)//
    {
		return hmi_queue_find_cmd(que, buffer, buf_len);
    }
	else if(que == &usb_que)
    {
		return hmi_queue_find_cmd(que, buffer, buf_len);
    }
    else 
	{
		return 0;
	}
}

#ifdef __cplusplus
}
#endif
