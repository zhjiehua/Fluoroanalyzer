#include "pageCommon.h"
#include "../Logic/managerment.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"

#ifdef __cplusplus
extern "C" {
#endif

void motorparaPageEditProcess(uint16 control_id, uint8 *str)
{
	switch(control_id)
	{
		case MOTORPARA_DISLOCATERETURN_EDIT:
			pProjectMan->dislocateReturn = StringToInt32(str);
			AT24CXX_Write(DISLOCATERETURN_BASEADDR, (uint8_t*)&pProjectMan->dislocateReturn, 4);
		break;
		case MOTORPARA_DISLOCATESPEED_EDIT:
			pProjectMan->dislocateSpeed = StringToInt32(str);
			AT24CXX_Write(DISLOCATESPEED_BASEADDR, (uint8_t*)&pProjectMan->dislocateSpeed, 1);
		break;
		case MOTORPARA_HEATINGUPSPEED_EDIT:
			pProjectMan->heatingUpSpeed = StringToInt32(str);
			AT24CXX_Write(HEATINGUPSPEED_BASEADDR, (uint8_t*)&pProjectMan->heatingUpSpeed, 1);
		break;
		case MOTORPARA_HEATINGDOWNSPEED_EDIT:
			pProjectMan->heatingDownSpeed = StringToInt32(str);
			AT24CXX_Write(HEATINGDOWNSPEED_BASEADDR, (uint8_t*)&pProjectMan->heatingDownSpeed, 1);
		break;
		case MOTORPARA_CUROFF1HEATVOL_EDIT:
			pProjectMan->cutoff1HeatingVoltage = StringToInt32(str);
			AT24CXX_Write(CUTOFF1HEATINGVOL_BASEADDR, (uint8_t*)&pProjectMan->cutoff1HeatingVoltage, 1);
		break;
		case MOTORPARA_CUROFF2HEATVOL_EDIT:
			pProjectMan->cutoff2HeatingVoltage = StringToInt32(str);
			AT24CXX_Write(CUTOFF2HEATINGVOL_BASEADDR, (uint8_t*)&pProjectMan->cutoff2HeatingVoltage, 1);
		break;
		case MOTORPARA_FUSINGHEATVOL_EDIT:
			pProjectMan->fusingHeatingVoltage = StringToInt32(str);
			AT24CXX_Write(FUSINGHEATINGVOL_BASEADDR, (uint8_t*)&pProjectMan->fusingHeatingVoltage, 1);
		break;
		default:
			cDebug("motorparaPage EDIT error!\n");
		break;
	}
}

	
void motorparaPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case MOTORPARA_BACK_BUTTON:
		break;
		default:
			cDebug("motorparaPage BUTTON error!\n");
		break;
	}
}

#ifdef __cplusplus
}
#endif
