#ifndef __QUEUE__
#define __QUEUE__

typedef unsigned int qdata_t;
typedef unsigned short qsize_t;

typedef struct
{
	qsize_t _head; //队列头
	qsize_t _tail;  //队列尾
	qsize_t _size; //队列长度
	qdata_t *_data;	//队列数据缓存区指针
}Queue_TypeDef;

void Queue_Init(Queue_TypeDef *que, qdata_t *buffer, qsize_t size);
void Queue_Push(Queue_TypeDef *que, qdata_t _data);
void Queue_Pop(Queue_TypeDef *que, qdata_t* _data);
qsize_t Queue_Size(Queue_TypeDef *que);

//=================用户数据======================

#define ORDERFILTER_QUEUE_MAX_SIZE 90 
extern qdata_t orderFilterQueueBuffer[ORDERFILTER_QUEUE_MAX_SIZE];
extern Queue_TypeDef orderFilterQueue; 

#endif
