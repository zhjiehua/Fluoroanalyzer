#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../StepMotor/StepMotor.h"
#include "../HARDWARE/DDC112/ddc112_interface.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif
	
void fastTestPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case FASTTEST_RETURN_BUTTON:
			pProjectMan->page = MAINPAGE_INDEX;
		break;
		case FASTTEST_START_BUTTON:
			ReadRTC();
			
			pProjectMan->projectMode = PROJECTMODE_NORMAL;
			//DDC112IF_PurpleLedEnable(1);
		
			pProjectMan->page = RESULTPAGE_INDEX;//TIPSPAGE_INDEX
			//pProjectMan->page = TIPSPAGE_INDEX;
		break;
		default:
			cDebug("fastTestPage BUTTON error!\n");
		break;
	}
}

void fastTestPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case FASTTEST_PROJECT_EDIT:  	
		break;
		case FASTTEST_BATCHNUM_EDIT:
		break;
		case FASTTEST_SAMPLENUM_EDIT:
		break;
		default:
			cDebug("fastTestPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
