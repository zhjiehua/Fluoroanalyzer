#ifndef _CMD_QUEUE
#define _CMD_QUEUE
#include "hmi_driver.h"

typedef unsigned char qdata;
typedef unsigned short qsize;

#define QUEUE_MAX_SIZE 20 //128  /*!< ָ����ջ�������С��������Ҫ�������������ô�һЩ*/

typedef struct _QUEUE
{
	qsize _head; //����ͷ
	qsize _tail;  //����β
	qdata _data[QUEUE_MAX_SIZE];	//�������ݻ�����
}QUEUE;

/*! 
 *  \brief  ���ָ������
 */
void queue_reset(void);

/*! 
 * \brief  ���ָ������
 * \detial ���ڽ��յ����ݣ�ͨ���˺�������ָ����� 
 *  \param  _data ָ������
 */
void queue_push(qdata _data);

/*! 
 *  \brief  ��ָ�������ȡ��һ��������ָ��
 *  \param  cmd ָ����ջ�����
 *  \param  buf_len ָ����ջ�������С
 *  \return  ָ��ȣ�0��ʾ������������ָ��
 */
qsize queue_find_cmd(qdata *cmd,qsize buf_len);

void queue_pop(qdata* _data);

extern QUEUE que;

#endif
