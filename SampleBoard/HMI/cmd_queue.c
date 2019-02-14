#include "cmd_queue.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

QUEUE que = {0,0,0};  //ָ�����
static qsize cmd_pos = 0;  //��ǰָ��ָ��λ��

void queue_reset(void)
{
	que._head = que._tail = 0;
	cmd_pos = 0;
}

//void queue_push(qdata _data)
//{
//	qsize pos = (que._head+1)%QUEUE_MAX_SIZE;
//	if(pos!=que._tail)//����״̬
//	{
//		que._data[que._head] = _data;
//		que._head = pos;
//	}
//}

//�Ӷ�����ȡһ������
void queue_pop(qdata* _data)
{
	if(que._tail!=que._head)//�ǿ�״̬
	{
		*_data = que._data[que._tail];
		que._tail = (que._tail+1)%QUEUE_MAX_SIZE;
	}
}

//��ȡ��������Ч���ݸ���
static qsize queue_size()
{
	return ((que._head+QUEUE_MAX_SIZE-que._tail)%QUEUE_MAX_SIZE);
}

qsize queue_find_cmd(qdata *buffer,qsize buf_len)
{
	qsize cmd_size = 0;
	qdata _data = 0;

	while(queue_size()>0)
	{
		//ȡһ������
		queue_pop(&_data);

		if(cmd_pos==0&&((_data&0xF0)!=0x80))//ָ���һ���ֽڱ�����֡ͷ����������
		    continue;

		if(cmd_pos<buf_len)//��ֹ���������
			buffer[cmd_pos++] = _data;

		//�չ�5�ֽڵõ�����֡
		if(cmd_pos>=5)
		{
			cmd_size = cmd_pos; //ָ���ֽڳ���
			cmd_pos = 0; //��λָ��ָ��

#if(DATASAFTY_CHECK_ENABLE)
			//�������������
			if(!CheckDataSafty(buffer+1, 4))//CRCУ��
				return 0;
#endif

			return cmd_size;
		}
	}

	return 0;//û���γ�������һ֡
}

#ifdef __cplusplus
}
#endif
