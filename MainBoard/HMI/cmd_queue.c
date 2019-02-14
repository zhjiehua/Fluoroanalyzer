#include "cmd_queue.h"
#include "hmi_user_uart.h"
#include "HardwareCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

//队列实际缓冲区
qdata hmi_que_buffer[HMI_QUEUE_MAX_SIZE];
qdata ddc112_que_buffer[DDC112_QUEUE_MAX_SIZE];
qdata user_que_buffer[USER_QUEUE_MAX_SIZE];
qdata usb_que_buffer[USB_QUEUE_MAX_SIZE];
	
//指令队列	
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
	if(pos!=que->_tail)//非满状态
	{
		que->_data[que->_head] = _data;
		que->_head = pos;
	}
}

//从队列中取一个数据
void queue_pop(QUEUE *que, qdata* _data)
{
	if(que->_tail!=que->_head)//非空状态
	{
		*_data = que->_data[que->_tail];
		que->_tail = (que->_tail+1)%que->_size;
	}
}

//获取队列中有效数据个数
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
		//取一个数据
		queue_pop(que, &_data);

		if(que->_cmd_pos==0&&_data!=HMI_CMD_HEAD)//指令第一个字节必须是帧头，否则跳过
			continue;

		if(que->_cmd_pos<buf_len)//防止缓冲区溢出
			buffer[que->_cmd_pos++] = _data;

		cmd_state = ((cmd_state<<8)|_data);//拼接最后4个字节，组成一个32位整数

		//最后4个字节与帧尾匹配，得到完整帧
		if(cmd_state==HMI_CMD_TAIL)
		{
			cmd_size = que->_cmd_pos; //指令字节长度
			cmd_state = 0;  //重新检测帧尾巴
			que->_cmd_pos = 0; //复位指令指针

#if(HMI_CRC16_ENABLE)
			//去掉指令头尾EE，尾FFFCFFFF共计5个字节，只计算数据部分CRC
			if(!CheckCRC16(buffer+1,cmd_size-5))//CRC校验
				return 0;

			cmd_size -= 2;//去掉CRC16（2字节）
#endif

			return cmd_size;
		}
	}
	return 0; //没有形成完整的一帧
}

static qsize ddc112_queue_find_cmd(QUEUE *que, qdata *buffer, qsize buf_len)
{
	qsize cmd_size = 0;
	qdata _data = 0;
	while(queue_size(que)>0)
	{
		//取一个数据
		queue_pop(que, &_data);

		if(que->_cmd_pos==0&&((_data&0xF0)!=0x80))//指令第一个字节必须是帧头，否则跳过
			continue;

		if(que->_cmd_pos<buf_len)//防止缓冲区溢出
			buffer[que->_cmd_pos++] = _data;

		//最后4个字节与帧尾匹配，得到完整帧
		if(que->_cmd_pos>=5)
		{
			cmd_size = que->_cmd_pos; //指令字节长度
			que->_cmd_pos = 0; //复位指令指针

			return cmd_size;
		}
	}
	return 0; //没有形成完整的一帧
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
