/*! 
 *  \file cmd_queue.h
 *  \brief ����ָ�����
 *  \version 1.0
 *  \date 2012-2015
 *  \copyright ���ݴ�ʹ��Ƽ����޹�˾
 */

#ifndef _CMD_QUEUE
#define _CMD_QUEUE
#include "hmi_driver.h"

typedef unsigned char qdata;
typedef unsigned short qsize;

typedef struct _QUEUE
{
	qsize _head; //����ͷ
	qsize _tail;  //����β
	
	qsize _cmd_pos; //��ǰָ��ָ��λ��
	
	qsize _size; //���г���
	qdata *_data;	//�������ݻ�����ָ��
}QUEUE;

/*! 
 *  \brief  ���ָ������
 */
void queue_init(QUEUE *que, qdata *buffer, qsize size);

/*! 
 * \brief  ���ָ������
 * \detial ���ڽ��յ����ݣ�ͨ���˺�������ָ����� 
 *  \param  _data ָ������
 */
void queue_push(QUEUE *que, qdata _data);

/*! 
 *  \brief  ��ָ�������ȡ��һ��������ָ��
 *  \param  cmd ָ����ջ�����
 *  \param  buf_len ָ����ջ�������С
 *  \return  ָ��ȣ�0��ʾ������������ָ��
 */
qsize queue_find_cmd(QUEUE *que, qdata *cmd,qsize buf_len);

qsize queue_size(QUEUE *que);
void queue_pop(QUEUE *que, qdata* _data);

//=================�û�����======================

#define HMI_QUEUE_MAX_SIZE 300 //128  /*!< ָ����ջ�������С��������Ҫ�������������ô�һЩ*/
#define DDC112_QUEUE_MAX_SIZE 40 //128  /*!< ָ����ջ�������С��������Ҫ�������������ô�һЩ*/
#define USER_QUEUE_MAX_SIZE 20 //128  /*!< ָ����ջ�������С��������Ҫ�������������ô�һЩ*/
#define USB_QUEUE_MAX_SIZE 40

extern qdata hmi_que_buffer[];
extern qdata ddc112_que_buffer[];
extern qdata user_que_buffer[];
extern qdata usb_que_buffer[];

extern QUEUE hmi_que;
extern QUEUE ddc112_que;
extern QUEUE user_que;
extern QUEUE usb_que;

#endif
