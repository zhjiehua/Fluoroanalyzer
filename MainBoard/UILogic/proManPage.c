#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/QRCode/QRCode.h"

#ifdef __cplusplus
extern "C" {
#endif

void proManPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case PROMAN_PROJECT_BUTTON:
			pProjectMan->scanInputFlag = 2;
			if(pProjectMan->qrCodeEnable)
				QRCode_StartRecognize();
		break;
		case PROMAN_BATCH_BUTTON:
			pProjectMan->scanInputFlag = 1;
			if(pProjectMan->qrCodeEnable)
				QRCode_StartRecognize();
		break;
		case PROMAN_RETURN_BUTTON:
			pProjectMan->page = MAINPAGE_INDEX;
		break;
		default:
			cDebug("proManPage BUTTON error!\n");
		break;
	}
}

void proManPageEditProcess(uint16 control_id, uint8 *str)
{
	uint8_t len;
	switch(control_id)
	{
		case PROMAN_PROJECT_EDIT:
			len = strlen((const char*)str)+1;
			memcpy(pProjectMan->projectString, str, len);
			AT24CXX_Write(PROJECT_STRING_BASEADDR, pProjectMan->projectString, len);
		
			cDebug("pProjectMan->projectString = %s\r\n", pProjectMan->projectString);
		break;
		case PROMAN_BATCH_EDIT:
			len = strlen((const char*)str)+1;
			memcpy(pProjectMan->batchString, str, len);
			AT24CXX_Write(BATCH_STRING_BASEADDR, pProjectMan->batchString, len);
		
			cDebug("pProjectMan->batchString = %s\r\n", pProjectMan->batchString);
		break;
		case PROMAN_CX_EDIT:
			pProjectMan->axisCX = StringToInt32(str);
			AT24CXX_Write(AXIS_CX_BASEADDR, (uint8_t*)&pProjectMan->axisCX, 2);
		
			cDebug("pProjectMan->axisCX = %d\r\n", pProjectMan->axisCX);
		break;
		case PROMAN_TX_EDIT:
			pProjectMan->axisTX = StringToInt32(str);
			AT24CXX_Write(AXIS_TX_BASEADDR, (uint8_t*)&pProjectMan->axisTX, 2);
		
			cDebug("pProjectMan->axisTX = %d\r\n", pProjectMan->axisTX);
		break;
		default:
			cDebug("parameterPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
