/*! 
 *  \file cmd_queue.h
 *  \brief 串口指令队列
 *  \version 1.0
 *  \date 2012-2015
 *  \copyright 广州大彩光电科技有限公司
 */

#ifndef _CMD_QUEUE
#define _CMD_QUEUE
#include "hmi_driver.h"

typedef unsigned char qdata;
typedef unsigned short qsize;

typedef struct _QUEUE
{
	qsize _head; //队列头
	qsize _tail;  //队列尾
	
	qsize _cmd_pos; //当前指令指针位置
	
	qsize _size; //队列长度
	qdata *_data;	//队列数据缓存区指针
}QUEUE;

/*! 
 *  \brief  清空指令数据
 */
void queue_init(QUEUE *que, qdata *buffer, qsize size);

/*! 
 * \brief  添加指令数据
 * \detial 串口接收的数据，通过此函数放入指令队列 
 *  \param  _data 指令数据
 */
void queue_push(QUEUE *que, qdata _data);

/*! 
 *  \brief  从指令队列中取出一条完整的指令
 *  \param  cmd 指令接收缓存区
 *  \param  buf_len 指令接收缓存区大小
 *  \return  指令长度，0表示队列中无完整指令
 */
qsize queue_find_cmd(QUEUE *que, qdata *cmd,qsize buf_len);

qsize queue_size(QUEUE *que);
void queue_pop(QUEUE *que, qdata* _data);

//=================用户数据======================

#define HMI_QUEUE_MAX_SIZE 300 //128  /*!< 指令接收缓冲区大小，根据需要调整，尽量设置大一些*/
#define DDC112_QUEUE_MAX_SIZE 40 //128  /*!< 指令接收缓冲区大小，根据需要调整，尽量设置大一些*/
#define USER_QUEUE_MAX_SIZE 20 //128  /*!< 指令接收缓冲区大小，根据需要调整，尽量设置大一些*/
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
