#include "Queue.h"

#ifdef __cplusplus
extern "C" {
#endif

//����ʵ�ʻ�����
qdata_t orderFilterQueueBuffer[ORDERFILTER_QUEUE_MAX_SIZE];
	
//ָ�����
Queue_TypeDef orderFilterQueue; 
	
void Queue_Init(Queue_TypeDef *que, qdata_t *buffer, qsize_t size)
{
	que->_head = que->_tail = 0;
	
	que->_data = buffer;
	que->_size = size;
}

void Queue_Push(Queue_TypeDef *que, qdata_t _data)
{
	qsize_t pos = (que->_head+1)%que->_size;
	if(pos!=que->_tail)//����״̬
	{
		que->_data[que->_head] = _data;
		que->_head = pos;
	}
}

//�Ӷ�����ȡһ������
void Queue_Pop(Queue_TypeDef *que, qdata_t* _data)
{
	if(que->_tail!=que->_head)//�ǿ�״̬
	{
		*_data = que->_data[que->_tail];
		que->_tail = (que->_tail+1)%que->_size;
	}
}

//��ȡ��������Ч���ݸ���
qsize_t Queue_Size(Queue_TypeDef *que)
{
	return ((que->_head+que->_size-que->_tail)%que->_size);
}

#ifdef __cplusplus
}
#endif
