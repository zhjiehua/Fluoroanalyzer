#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

#ifdef __cplusplus
extern "C" {
#endif

void curvePageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case CURVE_RETURN_BUTTON:
		break;
		case CURVE_XZOOMIN_BUTTON:
			pProjectMan->curveXMul += 1;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveXMul = %d\r\n", pProjectMan->curveXMul);
		break;
		case CURVE_XZOOMOUT_BUTTON:
			pProjectMan->curveXMul -= 1;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveXMul = %d\r\n", pProjectMan->curveXMul);
		break;
		case CURVE_YZOOMIN_BUTTON:
			pProjectMan->curveYMul += 1;
			//if(pProjectMan->curveYMul == 201) pProjectMan->curveYMul = 1;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveYMul = %d\r\n", pProjectMan->curveYMul);
		break;
		case CURVE_YZOOMOUT_BUTTON:
			pProjectMan->curveYMul -= 1;
			//if(pProjectMan->curveYMul == 0) pProjectMan->curveYMul = 200;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveYMul = %d\r\n", pProjectMan->curveYMul);
		break;
		case CURVE_UP_BUTTON:
			pProjectMan->curveYOffset -= 2;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);

			//cDebug("pProjectMan->curveYOffset = %d\r\n", pProjectMan->curveYOffset);
		break;
		case CURVE_DOWN_BUTTON:
			pProjectMan->curveYOffset += 2;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveYOffset = %d\r\n", pProjectMan->curveYOffset);
		break;
		case CURVE_LEFT_BUTTON:
			pProjectMan->curveXOffset += 2;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveXOffset = %d\r\n", pProjectMan->curveXOffset);
		break;
		case CURVE_RIGHT_BUTTON:
			pProjectMan->curveXOffset -= 2;
			GraphSetViewport(CURVEPAGE_INDEX, CURVE_GRAPHY, pProjectMan->curveXOffset,
				pProjectMan->curveXMul, pProjectMan->curveYOffset, pProjectMan->curveYMul);
		
			//cDebug("pProjectMan->curveXOffset = %d\r\n", pProjectMan->curveXOffset);
		break;
		default:
			cDebug("curvePage BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
