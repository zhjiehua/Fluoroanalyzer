#include "pageCommon.h"
#include "../Logic/managerment.h"
#include "CPrintf.h"
#include "../HARDWARE/DCMotor/DCMotor.h"

#ifdef __cplusplus
extern "C" {
#endif

void statusPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case STATUS_CUTOFF1_BUTTON:
		case STATUS_CUTOFF2_BUTTON:
		case STATUS_AUTO_BUTTON:
		case STATUS_RESET_BUTTON:	
		{
			xSemaphoreTake(pProjectMan->projectStatusSem, portMAX_DELAY);
			if(state)
			{
				if(!(pProjectMan->projectStatus & PROJECT_RUNNING))
				{
					pProjectMan->projectStopFlag = 0;
					pProjectMan->projectStatus = control_id+6;
					pProjectMan->projectStatus |= PROJECT_RUNNING;
					xSemaphoreGive(pProjectMan->projectStatusSem);
				}
				else
				{
					xSemaphoreGive(pProjectMan->projectStatusSem);
					SetButtonValue(STATUSPAGE_INDEX, control_id, 0);
				}
			}
			else
			{
				pProjectMan->projectStopFlag = 1;
				xSemaphoreGive(pProjectMan->projectStatusSem);
			}
		}
		break;
		case STATUS_BACK_BUTTON:
			pProjectMan->inStatusPageFlag = 0;
		break;
		default:
			cDebug("statusPage BUTTON error!\n");
		break;
	}
}

void statusPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		default:
			cDebug("statusPage EDITS error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
