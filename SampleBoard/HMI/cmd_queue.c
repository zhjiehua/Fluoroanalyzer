#include "cmd_queue.h"
#include "uart.h"

#ifdef __cplusplus
extern "C" {
#endif

QUEUE que = {0,0,0};  //指令队列
static qsize cmd_pos = 0;  //当前指令指针位置

void queue_reset(void)
{
	que._head = que._tail = 0;
	cmd_pos = 0;
}

//void queue_push(qdata _data)
//{
//	qsize pos = (que._head+1)%QUEUE_MAX_SIZE;
//	if(pos!=que._tail)//非满状态
//	{
//		que._data[que._head] = _data;
//		que._head = pos;
//	}
//}

//从队列中取一个数据
void queue_pop(qdata* _data)
{
	if(que._tail!=que._head)//非空状态
	{
		*_data = que._data[que._tail];
		que._tail = (que._tail+1)%QUEUE_MAX_SIZE;
	}
}

//获取队列中有效数据个数
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
		//取一个数据
		queue_pop(&_data);

		if(cmd_pos==0&&((_data&0xF0)!=0x80))//指令第一个字节必须是帧头，否则跳过
		    continue;

		if(cmd_pos<buf_len)//防止缓冲区溢出
			buffer[cmd_pos++] = _data;

		//凑够5字节得到完整帧
		if(cmd_pos>=5)
		{
			cmd_size = cmd_pos; //指令字节长度
			cmd_pos = 0; //复位指令指针

#if(DATASAFTY_CHECK_ENABLE)
			//检查数据完整性
			if(!CheckDataSafty(buffer+1, 4))//CRC校验
				return 0;
#endif

			return cmd_size;
		}
	}

	return 0;//没有形成完整的一帧
}

#ifdef __cplusplus
}
#endif
