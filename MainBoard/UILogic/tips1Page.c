#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void tips1PageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
	case TIPS1_OK_BUTTON:
		//pProjectMan->tipsButton = TIPS_OK;
		
		SetScreen(pProjectMan->page);
		break;
		default:
			cDebug("tips1Page BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
