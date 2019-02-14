#ifndef _PRINTER_H_
#define _PRINTER_H_

#include "../HMI/hmi_user_uart.h"
#include "stdint.h"

//void SEND_DATA(uint8 c);
//void BEGIN_CMD();
//void END_CMD();
//void SendNU8(uint8 *pData, uint16 nDataLen);
//void SendNU16(uint16 *pData, uint16 nDataLen);

//void SendStrings(uint8_t *str);

//功能函数
void Printer_SendStrings(uint8_t *str);
void Printer_PrintLF(void); //打印并换行
void Printer_PrintReturn(void); //打印并回车
void Printer_Init(void); //打印机初始化
void Printer_PrintPaperFeed(uint8_t n); //打印并进纸 进纸长度n*0.125mm
void Printer_PrintPaperSkipNLine(uint8_t n); //打印并走纸n行
void Printer_ModeSetting(uint8_t mode); //设置打印模式
void Printer_FontSelect(uint8_t font); //选择字符字体
void Printer_ChineseModeSetting(void); //设置汉字模式
void Printer_ChineseModeCancel(void); //取消汉字模式
void Printer_AlignTypeSelect(uint8_t type); //选择对齐方式
void Printer_DefaultLineSpaceSetting(void); //设置默认行间距 0.375mm
void Printer_LineSpaceSetting(uint8_t n); //设置行间距 n*0.125mm
void Printer_CharRightSpaceSetting(uint8_t n); //设置字符右间距
void Printer_LeftSpaceSetting(uint16_t n); //设置字符左间距
void Printer_PrintRangeWidthSetting(uint16_t width); //设置打印区域宽度
void Printer_CharSizeSetting(uint8_t size); //设置字符大小
void Printer_PrintGrayLevelSetting(uint8_t grayLevel); //设置打印灰度
void Printer_PrintInverseSetting(uint8_t flag); //取消/设置反白打印
void Printer_PrintBottonUpSetting(uint8_t flag); //取消/设置颠倒打印
void Printer_PrintUnderlineSetting(uint8_t flag); //添加/消除下划线
void Printer_StatusSend(uint8_t status); //传送状态
void Printer_BarCodeWidthSetting(uint8_t width); //设置条形码宽度
void Printer_BarCodeHeightSetting(uint8_t height); //设置条形码高度
void Printer_HRICharPositionSetting(uint8_t position); //设置HRI字符打印位置
void Printer_BarCodePrint(uint8_t barCodeType, uint8_t dataLen, uint8_t *data); //打印条形码
void Printer_BitmapModeSelect(uint8_t bitmapMode, uint16_t dataLen, uint8_t *data); //选择位图模式
void Printer_NVBitmapDownload(uint8_t bitmapCount, uint8_t *data); //下载NV位图
void Printer_NVBitmapPrint(uint8_t bitmapMode, uint8_t index); //NV位图打印
void Printer_PattenBitmapPrint(uint8_t bitmapMode, uint16_t xCount, uint16_t yCount, uint8_t *data); //打印光栅位图
void Printer_CharSet1Select(void); //选择字符集1
void Printer_CharSet2Select(void); //选择字符集2
void Printer_CursorSelect(uint8_t flag); //选择上下标

#endif
