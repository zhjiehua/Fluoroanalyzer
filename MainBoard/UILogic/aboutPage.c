#include "pageCommon.h"
#include "../Logic/managerment.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/DCMotor/DCMotor.h"
#include "../HardwareCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

void aboutPageButtonProcess(uint16 control_id, uint8  state)
{
	uint8_t tempData[10];
	uint16_t i;
	switch(control_id)
	{
		case ABOUT_CLEAROUTPUT_BUTTON:
			//清零总产量
			pProjectMan->totalOutput = 0;
			SetTextInt32(MAINPAGE_INDEX, MAIN_OUTPUT_EDIT, 0, 0, 0);
			AT24CXX_Write(TOTALOUTPUT_BASEADDR, (uint8_t*)&pProjectMan->totalOutput, 4);
		break;
		case ABOUT_SAVEASDEFAULT_BUTTON:
			for(i=0;i<EMERGENCYFLAG_BASEADDR+20;i+=10)
			{
				AT24CXX_Read(i, (uint8_t*)tempData, 10);
				AT24CXX_Write(RESTOREDEFAULT_BASEADDR+i, (uint8_t*)tempData, 10);
			}
		break;
		case ABOUT_RESTOREDEFAULT_BUTTON:
			for(i=0;i<EMERGENCYFLAG_BASEADDR+20;i+=10)
			{
				AT24CXX_Read(RESTOREDEFAULT_BASEADDR+i, (uint8_t*)tempData, 10);
				AT24CXX_Write(i, (uint8_t*)tempData, 10);
			}
		break;
		case ABOUT_CLEARUSABLETIMES_BUTTON:
			pProjectMan->usableTimes = 0;
			AT24CXX_Write(USABLETIMES_BASEADDR, (uint8_t*)&pProjectMan->usableTimes, 2);
		break;
		default:
			cDebug("aboutPage BUTTON error!\n");
		break;
	}
}

//void aboutPageEditProcess(uint16 control_id, uint8 *str)
//{
//	switch(control_id)
//	{	   
//		default:
//			cDebug("aboutPage EDIT error!\n");
//		break;
//	}
//}

#ifdef __cplusplus
}
#endif
