#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/QRCode/QRCode.h"

#ifdef __cplusplus
extern "C" {
#endif
	
void logoPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case LOGO_START_BUTTON:
			pProjectMan->page = MAINPAGE_INDEX;
			break;
		default:
			cDebug("logoPage BUTTON error!\n");
		break;
	}
}
	
void mainPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case MAIN_FASTTEST_BUTTON:
			SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_PROJECT_EDIT, pProjectMan->projectString);
			SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_BATCHNUM_EDIT, pProjectMan->batchString);
			SetTextValue(FASTTESTPAGE_INDEX, FASTTEST_SAMPLENUM_EDIT, pProjectMan->sampleString);
			
			pProjectMan->scanInputFlag = 0;
			if(pProjectMan->qrCodeEnable)
				QRCode_StartRecognize();
			
			pProjectMan->page = FASTTESTPAGE_INDEX;
			
			break;
		case MAIN_PARACALIB_BUTTON:
			{
				uint8_t temp[6];
				sprintf((char*)temp, "%.02f", pProjectMan->calibFactor);
				SetTextValue(CALIBPAGE_INDEX, CALIB_FACTOR_EDIT, temp);
				cDebug("temp = %s\r\n", temp);
			}
			//SetTextFloat(CALIBPAGE_INDEX, CALIB_FACTOR_EDIT, pProjectMan->calibFactor, 2, 0);
		
			//cDebug("pProjectMan->calibFactor f = %f\r\n", pProjectMan->calibFactor);
			//cDebug("pProjectMan->calibFactor x = %08X\r\n", pProjectMan->calibFactor);
			
			pProjectMan->page = CALIBPAGE_INDEX;
			break;
		case MAIN_SETUP_BUTTON:
			SetButtonValue(SETUPPAGE_INDEX, SETUP_QRCODE_BUTTON, pProjectMan->qrCodeEnable);
			SetButtonValue(SETUPPAGE_INDEX, SETUP_PRINTER_BUTTON, pProjectMan->printerEnable);
			SetButtonValue(SETUPPAGE_INDEX, SETUP_ALARM_BUTTON, pProjectMan->alarmEnable);
			SetButtonValue(SETUPPAGE_INDEX, SETUP_TOUCH_BUTTON, pProjectMan->beeperEnable);
		
			pProjectMan->page = SETUPPAGE_INDEX;
			break;
		case MAIN_PARAMETER_BUTTON:
			SetTextInt32(PARAMETERPAGE_INDEX, PARAMETER_SAMPLESTART_EDIT, pProjectMan->sampleStartPoint, 0, 0);
			SetTextInt32(PARAMETERPAGE_INDEX, PARAMETER_SAMPLESTOP_EDIT, pProjectMan->sampleStopPoint, 0, 0);

			pProjectMan->page = PARAMETERPAGE_INDEX;
			break;
		case MAIN_HISTORY_BUTTON:
			pProjectMan->page = RESULTPAGE_INDEX;
			break;
		case MAIN_PROMAN_BUTTON:
			pProjectMan->page = PROMANPAGE_INDEX;
			SetTextValue(PROMANPAGE_INDEX, PROMAN_PROJECT_EDIT, pProjectMan->projectString);
			SetTextValue(PROMANPAGE_INDEX, PROMAN_BATCH_EDIT, pProjectMan->batchString);
			SetTextInt32(PROMANPAGE_INDEX, PROMAN_CX_EDIT, pProjectMan->axisCX, 0, 0);
			SetTextInt32(PROMANPAGE_INDEX, PROMAN_TX_EDIT, pProjectMan->axisTX, 0, 0);
		break;
		default:
			cDebug("mainPage BUTTON error!\n");
			break;
	}
}

#ifdef __cplusplus
}
#endif
