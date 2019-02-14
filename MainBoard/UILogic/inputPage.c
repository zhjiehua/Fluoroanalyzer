#include "pageCommon.h"
#include "../Logic/managerment.h"
#include "CPrintf.h"

#ifdef __cplusplus
extern "C" {
#endif

void inputPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case INPUT_BACK_BUTTON:
			pProjectMan->inInputPageFlag = 0;
		break;
		default:
			cDebug("inputPage BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
