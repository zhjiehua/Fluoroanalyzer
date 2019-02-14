#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HardwareCommon.h"

#ifdef __cplusplus
extern "C" {
#endif

void calibPageButtonProcess(uint16 control_id, uint8 state)
{	
	switch(control_id)
	{
		case CALIB_SETUP_BUTTON:
			
		break;
		case CALIB_RETURN_BUTTON:
			pProjectMan->page = MAINPAGE_INDEX;
		break;
		default:
			cDebug("calibPage BUTTON error!\n");
		break;
	}
}

void calibPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{	 
		case CALIB_FACTOR_EDIT:
			pProjectMan->calibFactor = StringToFloat(str);
			AT24CXX_Write(CALIBFACTOR_BASEADDR, (uint8_t*)&pProjectMan->calibFactor, 4);
		
			cDebug("pProjectMan->calibFactor = %f\n", pProjectMan->calibFactor);
		break;
		default:
			cDebug("calibPage EDIT error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
