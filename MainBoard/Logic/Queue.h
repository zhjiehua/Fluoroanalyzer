#ifndef __QUEUE__
#define __QUEUE__

typedef unsigned int qdata_t;
typedef unsigned short qsize_t;

typedef struct
{
	qsize_t _head; //����ͷ
	qsize_t _tail;  //����β
	qsize_t _size; //���г���
	qdata_t *_data;	//�������ݻ�����ָ��
}Queue_TypeDef;

void Queue_Init(Queue_TypeDef *que, qdata_t *buffer, qsize_t size);
void Queue_Push(Queue_TypeDef *que, qdata_t _data);
void Queue_Pop(Queue_TypeDef *que, qdata_t* _data);
qsize_t Queue_Size(Queue_TypeDef *que);

//=================�û�����======================

#define ORDERFILTER_QUEUE_MAX_SIZE 90 
extern qdata_t orderFilterQueueBuffer[ORDERFILTER_QUEUE_MAX_SIZE];
extern Queue_TypeDef orderFilterQueue; 

#endif
