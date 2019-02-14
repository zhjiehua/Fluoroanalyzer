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

//���ܺ���
void Printer_SendStrings(uint8_t *str);
void Printer_PrintLF(void); //��ӡ������
void Printer_PrintReturn(void); //��ӡ���س�
void Printer_Init(void); //��ӡ����ʼ��
void Printer_PrintPaperFeed(uint8_t n); //��ӡ����ֽ ��ֽ����n*0.125mm
void Printer_PrintPaperSkipNLine(uint8_t n); //��ӡ����ֽn��
void Printer_ModeSetting(uint8_t mode); //���ô�ӡģʽ
void Printer_FontSelect(uint8_t font); //ѡ���ַ�����
void Printer_ChineseModeSetting(void); //���ú���ģʽ
void Printer_ChineseModeCancel(void); //ȡ������ģʽ
void Printer_AlignTypeSelect(uint8_t type); //ѡ����뷽ʽ
void Printer_DefaultLineSpaceSetting(void); //����Ĭ���м�� 0.375mm
void Printer_LineSpaceSetting(uint8_t n); //�����м�� n*0.125mm
void Printer_CharRightSpaceSetting(uint8_t n); //�����ַ��Ҽ��
void Printer_LeftSpaceSetting(uint16_t n); //�����ַ�����
void Printer_PrintRangeWidthSetting(uint16_t width); //���ô�ӡ������
void Printer_CharSizeSetting(uint8_t size); //�����ַ���С
void Printer_PrintGrayLevelSetting(uint8_t grayLevel); //���ô�ӡ�Ҷ�
void Printer_PrintInverseSetting(uint8_t flag); //ȡ��/���÷��״�ӡ
void Printer_PrintBottonUpSetting(uint8_t flag); //ȡ��/���õߵ���ӡ
void Printer_PrintUnderlineSetting(uint8_t flag); //���/�����»���
void Printer_StatusSend(uint8_t status); //����״̬
void Printer_BarCodeWidthSetting(uint8_t width); //������������
void Printer_BarCodeHeightSetting(uint8_t height); //����������߶�
void Printer_HRICharPositionSetting(uint8_t position); //����HRI�ַ���ӡλ��
void Printer_BarCodePrint(uint8_t barCodeType, uint8_t dataLen, uint8_t *data); //��ӡ������
void Printer_BitmapModeSelect(uint8_t bitmapMode, uint16_t dataLen, uint8_t *data); //ѡ��λͼģʽ
void Printer_NVBitmapDownload(uint8_t bitmapCount, uint8_t *data); //����NVλͼ
void Printer_NVBitmapPrint(uint8_t bitmapMode, uint8_t index); //NVλͼ��ӡ
void Printer_PattenBitmapPrint(uint8_t bitmapMode, uint16_t xCount, uint16_t yCount, uint8_t *data); //��ӡ��դλͼ
void Printer_CharSet1Select(void); //ѡ���ַ���1
void Printer_CharSet2Select(void); //ѡ���ַ���2
void Printer_CursorSelect(uint8_t flag); //ѡ�����±�

#endif
