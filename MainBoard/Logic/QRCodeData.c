#include "QRCodeData.h"
#include "management.h"
#include "string.h"
#include "../HARDWARE/24CXX/24cxx.h"

#define SOH	'!' //���⿪ʼ
#define STX '@' //���Ŀ�ʼ
#define US	',' //��Ԫ�ָ���
#define ETX '#' //���Ľ���
#define EOT '$' //�������

#define PROJECT_NAME	"01"
#define BATCH_NUMBER	"02"
#define INVALIDE_DATA	"03"
#define SCANLINE_NUM	"04"
#define CLINE_POSITION	"05"
#define TLINE_POSITION	"06"
#define CHECK_WINDOW	"07"
#define SAMPLE_TYPE		"08"
#define CONCEN_UNIT		"09"
#define DETECT_RANGE_L	"10"
#define DETECT_RANGE_H	"11"
#define REFER_RANGE_L	"12"
#define REFER_RANGE_H	"13"
#define FITTING_TYPE	"14"
#define FITTING_MODEL	"15"
#define FITTING_PARA1	"16"
#define FITTING_PARA2	"17"
#define FITTING_PARA3	"18"
#define FITTING_PARA4	"19"

void QR_Parse_All(uint8_t *buffer, uint8_t len)
{
	uint8_t i = 0, j = 0;
	uint8_t temp[10];
	
	while(buffer[i] != EOT)
	{
		if(buffer[i] == STX) //���Ŀ�ʼ 
		{
			i++;
			
			if(!strncmp((const char*)&buffer[i], (const char*)PROJECT_NAME, 2))//��Ŀ����
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
				{
					pProjectMan->projectString[j++] = buffer[i++];
				}
				pProjectMan->projectString[j] = '\0';
				
				SetTextValue(PROMANPAGE_INDEX, PROMAN_PROJECT_EDIT, pProjectMan->projectString);
				
				AT24CXX_Write(PROJECT_STRING_BASEADDR, (uint8_t*)pProjectMan->projectString, strlen((const char*)pProjectMan->projectString)+1);
				
				cDebug("pProjectMan->projectString = %s\r\n", pProjectMan->projectString);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)BATCH_NUMBER, 2))//���ű��
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
				{
					pProjectMan->batchString[j++] = buffer[i++];
				}
				pProjectMan->batchString[j] = '\0';
				
				SetTextValue(PROMANPAGE_INDEX, PROMAN_BATCH_EDIT, pProjectMan->batchString);
				
				AT24CXX_Write(BATCH_STRING_BASEADDR, (uint8_t*)pProjectMan->batchString, strlen((const char*)pProjectMan->batchString)+1);
				
				cDebug("pProjectMan->batchString = %s\r\n", pProjectMan->batchString);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)INVALIDE_DATA, 2))//ʧЧ����
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
				{
					pProjectMan->invalideData[j++] = buffer[i++];
				}
				pProjectMan->invalideData[j] = '\0';
				
				AT24CXX_Write(INVALIDEDATA_STRING_BASEADDR, (uint8_t*)pProjectMan->invalideData, strlen((const char*)pProjectMan->invalideData)+1);
				
				cDebug("pProjectMan->invalideData = %s\r\n", pProjectMan->invalideData);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)SCANLINE_NUM, 2))//ɨ������
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->lineNumber = StringToInt32(temp)+2;
				
				AT24CXX_Write(SCANLINE_NUM_BASEADDR, (uint8_t*)&pProjectMan->lineNumber, sizeof(pProjectMan->lineNumber));
				
				cDebug("pProjectMan->lineNumber = %d\r\n", pProjectMan->lineNumber);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)CLINE_POSITION, 2))//C��λ��
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->axisCX = StringToInt32(temp);
				
				SetTextValueInt32(PROMANPAGE_INDEX, PROMAN_CX_EDIT, pProjectMan->axisCX);
				
				AT24CXX_Write(AXIS_CX_BASEADDR, (uint8_t*)&pProjectMan->axisCX, sizeof(pProjectMan->axisCX));
				
				cDebug("pProjectMan->axisCX = %d\r\n", pProjectMan->axisCX);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)TLINE_POSITION, 2))//T��λ��
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->axisTX = StringToInt32(temp);
				
				SetTextValueInt32(PROMANPAGE_INDEX, PROMAN_TX_EDIT, pProjectMan->axisTX);
				
				AT24CXX_Write(AXIS_TX_BASEADDR, (uint8_t*)&pProjectMan->axisTX, sizeof(pProjectMan->axisTX));
				
				cDebug("pProjectMan->axisTX = %d\r\n", pProjectMan->axisTX);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)CHECK_WINDOW, 2))//��ⴰ��
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->checkWindowAxisX = StringToInt32(temp);
				
				AT24CXX_Write(CHECKWINDOW_BASEADDR, (uint8_t*)&pProjectMan->checkWindowAxisX, sizeof(pProjectMan->checkWindowAxisX));
				
				cDebug("pProjectMan->checkWindowAxisX = %d\r\n", pProjectMan->checkWindowAxisX);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)SAMPLE_TYPE, 2))//��������
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->sampleType = StringToInt32(temp);
				
				AT24CXX_Write(SAMPLETYPE_BASEADDR, (uint8_t*)&pProjectMan->sampleType, sizeof(pProjectMan->sampleType));
				
				cDebug("pProjectMan->sampleType = %d\r\n", pProjectMan->sampleType);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)CONCEN_UNIT, 2))//Ũ�ȵ�λ
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
				{
					pProjectMan->concentrationUnit[j++] = buffer[i++];
				}
				pProjectMan->concentrationUnit[j] = '\0';
				
				AT24CXX_Write(CONCENUNIT_STRING_BASEADDR, (uint8_t*)pProjectMan->concentrationUnit, strlen((const char*)pProjectMan->concentrationUnit)+1);
				
				cDebug("pProjectMan->concentrationUnit = %s\r\n", pProjectMan->concentrationUnit);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)DETECT_RANGE_L, 2))//��ⷶΧ-L
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->checkRangeLow = StringToFloat(temp);
				
				AT24CXX_Write(CHECKRANGELOW_BASEADDR, (uint8_t*)&pProjectMan->checkRangeLow, sizeof(pProjectMan->checkRangeLow));
				
				cDebug("pProjectMan->checkRangeLow = %f\r\n", pProjectMan->checkRangeLow);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)DETECT_RANGE_H, 2))//��ⷶΧ-H
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->checkRangeHigh = StringToFloat(temp);
				
				AT24CXX_Write(CHECKRANGEHIGH_BASEADDR, (uint8_t*)&pProjectMan->checkRangeHigh, sizeof(pProjectMan->checkRangeHigh));
				
				cDebug("pProjectMan->checkRangeHigh = %f\r\n", pProjectMan->checkRangeHigh);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)REFER_RANGE_L, 2))//�ο���Χ-L
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->referRangeLow = StringToFloat(temp);
				
				AT24CXX_Write(REFERRANGELOW_BASEADDR, (uint8_t*)&pProjectMan->referRangeLow, sizeof(pProjectMan->referRangeLow));
				
				cDebug("pProjectMan->referRangeLow = %f\r\n", pProjectMan->referRangeLow);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)REFER_RANGE_H, 2))//�ο���Χ-H
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->referRangeHigh = StringToFloat(temp);
				
				AT24CXX_Write(REFERRANGEHIGH_BASEADDR, (uint8_t*)&pProjectMan->referRangeHigh, sizeof(pProjectMan->referRangeHigh));
				
				cDebug("pProjectMan->referRangeHigh = %f\r\n", pProjectMan->referRangeHigh);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)FITTING_TYPE, 2))//�궨��ʽ
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->fittingType = StringToInt32(temp);
				
				AT24CXX_Write(FITTINGTYPE_BASEADDR, (uint8_t*)&pProjectMan->fittingType, sizeof(pProjectMan->fittingType));
				
				cDebug("pProjectMan->fittingType = %d\r\n", pProjectMan->fittingType);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)FITTING_MODEL, 2))//���ģ��
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->fittingModel = StringToInt32(temp);
				
				AT24CXX_Write(FITTINGMODEL_BASEADDR, (uint8_t*)&pProjectMan->fittingModel, sizeof(pProjectMan->fittingModel));
				
				cDebug("pProjectMan->fittingModel = %d\r\n", pProjectMan->fittingModel);
			}
			
			else if(!strncmp((const char*)&buffer[i], (const char*)FITTING_PARA1, 2))//��ϲ���1
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->fittingPara1 = StringToFloat(temp);
				
				AT24CXX_Write(FITTINGPARA1_BASEADDR, (uint8_t*)&pProjectMan->fittingPara1, sizeof(pProjectMan->fittingPara1));
				
				cDebug("pProjectMan->fittingPara1 = %f\r\n", pProjectMan->fittingPara1);
			}
			else if(!strncmp((const char*)&buffer[i], (const char*)FITTING_PARA2, 2))//��ϲ���2
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->fittingPara2 = StringToFloat(temp);
				
				AT24CXX_Write(FITTINGPARA2_BASEADDR, (uint8_t*)&pProjectMan->fittingPara2, sizeof(pProjectMan->fittingPara2));
				
				cDebug("pProjectMan->fittingPara2 = %f\r\n", pProjectMan->fittingPara2);
			}
			else if(!strncmp((const char*)&buffer[i], (const char*)FITTING_PARA3, 2))//��ϲ���3
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->fittingPara3 = StringToFloat(temp);
				
				AT24CXX_Write(FITTINGPARA3_BASEADDR, (uint8_t*)&pProjectMan->fittingPara3, sizeof(pProjectMan->fittingPara3));
				
				cDebug("pProjectMan->fittingPara3 = %f\r\n", pProjectMan->fittingPara3);
			}
			else if(!strncmp((const char*)&buffer[i], (const char*)FITTING_PARA4, 2))//��ϲ���4
			{
				i += 3;
				j = 0;
				while(buffer[i] != ETX)
					temp[j++] = buffer[i++];
				temp[j] = '\0';
				pProjectMan->fittingPara4 = StringToFloat(temp);
				
				AT24CXX_Write(FITTINGPARA4_BASEADDR, (uint8_t*)&pProjectMan->fittingPara4, sizeof(pProjectMan->fittingPara4));
				
				cDebug("pProjectMan->fittingPara4 = %f\r\n", pProjectMan->fittingPara4);
			}
		}
		else
			i++;
	}//while
	
//	for(i=0;i<len;i++)
//		cDebug("%c", buffer[i]);
}

void QRCode_Data_Parse(uint8_t *buffer, uint8_t len)
{
	uint8_t i;
	uint8_t cmd = buffer[0];
	switch(cmd)
	{
		case QRCODE_CMD_PROJECT: //��Ŀ���
			
			if(pProjectMan->page != PROMANPAGE_INDEX || pProjectMan->scanInputFlag != 2)
				break;
			
			if(len > PROJECT_LEN) len = PROJECT_LEN-1;
			for(i=0;i<len;i++)
			{
				pProjectMan->projectString[i] = buffer[i+1]; 
			}
			pProjectMan->projectString[i] = '\0';

			SetTextValue(PROMANPAGE_INDEX, PROMAN_PROJECT_EDIT, pProjectMan->projectString);
			
			AT24CXX_Write(PROJECT_STRING_BASEADDR, pProjectMan->projectString, strlen((const char*)pProjectMan->projectString)+1);
		break;
		case QRCODE_CMD_BATCH: //���α��
			
			if(pProjectMan->page != PROMANPAGE_INDEX || pProjectMan->scanInputFlag != 1)
				break;
			
			if(len > BATCH_LEN) len = BATCH_LEN-1;
			for(i=0;i<len;i++)
			{
				pProjectMan->batchString[i] = buffer[i+1]; 
			}
			pProjectMan->batchString[i] = '\0';
			
			SetTextValue(PROMANPAGE_INDEX, PROMAN_BATCH_EDIT, pProjectMan->batchString);
			
			AT24CXX_Write(BATCH_STRING_BASEADDR, pProjectMan->batchString, strlen((const char*)pProjectMan->batchString)+1);
		break;
		case QRCODE_CMD_SAMPLE: //��Ʒ���
			
			if(pProjectMan->page != FASTTESTPAGE_INDEX || pProjectMan->scanInputFlag != 0)
				break;
			
			if(len > SAMPLE_LEN) len = SAMPLE_LEN-1;
			for(i=0;i<len;i++)
			{
				pProjectMan->sampleString[i] = buffer[i+1]; 
			}
			pProjectMan->sampleString[i] = '\0';
			
			SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_SAMPLENUM_EDIT, pProjectMan->sampleString);
			
			AT24CXX_Write(SAMPLE_STRING_BASEADDR, pProjectMan->sampleString, strlen((const char*)pProjectMan->sampleString)+1);
		break;
			
		case QRCODE_CMD_ALL:
//			cDebug("QRCODE_CMD_ALL = %s\r\n", buffer);
//			cDebug("QRCODE_CMD_ALL size = %d\r\n", len);
			
			if(pProjectMan->page != PROMANPAGE_INDEX)
				break;
			
			QR_Parse_All(buffer, len);
		
		break;
		default:
		break;
	}
}
