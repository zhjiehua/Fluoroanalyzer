#include "Queue.h"

#ifdef __cplusplus
extern "C" {
#endif

//队列实际缓冲区
qdata_t orderFilterQueueBuffer[ORDERFILTER_QUEUE_MAX_SIZE];
	
//指令队列
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
	if(pos!=que->_tail)//非满状态
	{
		que->_data[que->_head] = _data;
		que->_head = pos;
	}
}

//从队列中取一个数据
void Queue_Pop(Queue_TypeDef *que, qdata_t* _data)
{
	if(que->_tail!=que->_head)//非空状态
	{
		*_data = que->_data[que->_tail];
		que->_tail = (que->_tail+1)%que->_size;
	}
}

//获取队列中有效数据个数
qsize_t Queue_Size(Queue_TypeDef *que)
{
	return ((que->_head+que->_size-que->_tail)%que->_size);
}

#ifdef __cplusplus
}
#endif
