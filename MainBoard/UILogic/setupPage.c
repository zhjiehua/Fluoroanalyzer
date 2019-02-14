#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HardwareCommon.h"
#include "QRCode/QRCode.h"

#include "main.h"

#ifdef __cplusplus
extern "C" {
#endif

void setupPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case SETUP_QRCODE_BUTTON:
			pProjectMan->qrCodeEnable = state;
			AT24CXX_Write(QRCODE_EN_BASEADDR, (uint8_t*)&pProjectMan->qrCodeEnable, 1);
		
			//QRCode_StartRecognize();
		break;
		case SETUP_PRINTER_BUTTON:
			pProjectMan->printerEnable = state;
			AT24CXX_Write(PRINTER_EN_BASEADDR, (uint8_t*)&pProjectMan->printerEnable, 1);
		break;
		case SETUP_ALARM_BUTTON:
			pProjectMan->alarmEnable = state;
			AT24CXX_Write(ALARM_EN_BASEADDR, (uint8_t*)&pProjectMan->alarmEnable, 1);
		break;
		case SETUP_TOUCH_BUTTON:
			UnlockDeviceConfig();
			pProjectMan->beeperEnable = state;
			AT24CXX_Write(BEEPER_EN_BASEADDR, (uint8_t*)&pProjectMan->beeperEnable, 1);
		break;
		case SETUP_QRINIT_BUTTON:
			QRCodeModelInit();
		break;
		case SETUP_RETURN_BUTTON:
			pProjectMan->page = MAINPAGE_INDEX;
		break;
		default:
			cDebug("setupPage BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
