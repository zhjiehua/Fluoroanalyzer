#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/QRCode/QRCode.h"

#ifdef __cplusplus
extern "C" {
#endif

void parameterPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case PARAMETER_HOME_BUTTON:
			if(state)
			{
				if(pProjectMan->projectMode == PROJECTMODE_NONE)
					pProjectMan->projectMode = PROJECTMODE_HOME;
			}
			else
			{
				pProjectMan->projectMode = PROJECTMODE_NONE;
				pProjectMan->smStopFlag = 1;
			}
		break;
		case PARAMETER_GO_SP_BUTTON:
			if(state)
			{
				if(pProjectMan->projectMode == PROJECTMODE_NONE)
					pProjectMan->projectMode = PROJECTMODE_GOREADY;
			}
			else
			{
				pProjectMan->projectMode = PROJECTMODE_NONE;
				pProjectMan->smStopFlag = 1;
			}
		break;
		case PARAMETER_GO_EP_BUTTON:
			if(state)
			{
				if(pProjectMan->projectMode == PROJECTMODE_NONE)
					pProjectMan->projectMode = PROJECTMODE_SAMPLE;
			}
			else
			{
				pProjectMan->projectMode = PROJECTMODE_NONE;
				pProjectMan->smStopFlag = 1;
			}
		break;
		case PARAMETER_MANUAL_BUTTON:
			pProjectMan->page = MANUALPAGE_INDEX;
		break;
		case PARAMETER_RETURN_BUTTON:
			pProjectMan->page = MAINPAGE_INDEX;
		break;
		default:
			cDebug("parameterPage BUTTON error!\n");
		break;
	}
}

void parameterPageEditProcess(uint16 control_id, uint8 *str)
{
	uint8_t len;
	switch(control_id)
	{
		case PARAMETER_SAMPLESTART_EDIT:
			pProjectMan->sampleStartPoint = StringToInt32(str);
			AT24CXX_Write(SAMPLESTART_BASEADDR, (uint8_t*)&pProjectMan->sampleStartPoint, 4);
		
			cDebug("pProjectMan->sampleStartPoint = %d\r\n", pProjectMan->sampleStartPoint);
		break;
		case PARAMETER_SAMPLESTOP_EDIT:
			pProjectMan->sampleStopPoint = StringToInt32(str);
			AT24CXX_Write(SAMPLESTOP_BASEADDR, (uint8_t*)&pProjectMan->sampleStopPoint, 4);
		
			cDebug("pProjectMan->sampleStopPoint = %d\r\n", pProjectMan->sampleStopPoint);
		break;
		default:
			cDebug("parameterPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
