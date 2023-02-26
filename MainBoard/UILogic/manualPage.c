#include "pageCommon.h"
#include "../Logic/management.h"
#include "CPrintf.h"
#include "../HARDWARE/24CXX/24cxx.h"
#include "../HARDWARE/StepMotor/stepmotor.h"
#include "../HARDWARE/DDC112/ddc112_interface.h"

#ifdef __cplusplus
extern "C" {
#endif

void manualPageButtonProcess(uint16 control_id, uint8 state)
{
	switch(control_id)
	{
		case MANUAL_FRONT_BUTTON:
			if(state)
			{
				if(pProjectMan->projectMode == PROJECTMODE_NONE)
				{
					pProjectMan->projectMode = PROJECTMODE_MANUAL;
					
					StepMotor_SetSpeed(0, pProjectMan->sampleHomeSpeed);
					StepMotor_SetDir(0, CW);
					StepMotor_SetCMD(0, ENABLE);
                    cDebug("front\r\n");
				}
			}
			else
			{
				pProjectMan->projectMode = PROJECTMODE_NONE;
				StepMotor_Stop(0);
			}
		break;
		case MANUAL_BACK_BUTTON:
			if(state)
			{
				if(pProjectMan->projectMode == PROJECTMODE_NONE)
				{
					pProjectMan->projectMode = PROJECTMODE_MANUAL;
					
					StepMotor_SetSpeed(0, pProjectMan->sampleHomeSpeed);
					StepMotor_SetDir(0, CCW);
					StepMotor_SetCMD(0, ENABLE);
                    cDebug("back\r\n");
				}
			}
			else
			{
				pProjectMan->projectMode = PROJECTMODE_NONE;
				StepMotor_Stop(0);
			}
		break;
		case MANUAL_LAMP_BUTTON:
			if(state)
			{
				if(pProjectMan->projectMode == PROJECTMODE_NONE)
				{
					pProjectMan->projectMode = PROJECTMODE_MANUAL;				
					DDC112IF_PurpleLedEnable(1);
				}
			}
			else
			{
				pProjectMan->projectMode = PROJECTMODE_NONE;
				DDC112IF_PurpleLedEnable(0);
			}
		break;
		case MANUAL_SAMPLE_BUTTON: //在所在的点采集350个点然后传给PC
			pProjectMan->projectMode = PROJECTMODE_TEST;
		break;
			
		case MANUAL_RETURN_BUTTON:
			pProjectMan->page = PARAMETERPAGE_INDEX;
		break;
		default:
			cDebug("manualPage BUTTON error!\n");
		break;
	}
}

void manualPageEditProcess(uint16 control_id, uint8 *str)
{
//	switch(control_id)
//	{
//		case TEMPPARA_CUTOFF1KP_EDIT:
//			pProjectMan->cutoff1PID.Proportion = StringToInt32(str)/10.0;
//			AT24CXX_Write(CUTOFF1KP_BASEADDR, (uint8_t*)&pProjectMan->cutoff1PID.Proportion, 4);
//		break;
//		case TEMPPARA_CUTOFF1KI_EDIT:
//			pProjectMan->cutoff1PID.Integral = StringToInt32(str)/10.0;
//			AT24CXX_Write(CUTOFF1KI_BASEADDR, (uint8_t*)&pProjectMan->cutoff1PID.Integral, 4);
//		break;
//		case TEMPPARA_CUTOFF1KD_EDIT:
//			pProjectMan->cutoff2PID.Derivative = StringToInt32(str)/10.0;
//			AT24CXX_Write(CUTOFF2KD_BASEADDR, (uint8_t*)&pProjectMan->cutoff1PID.Derivative, 4);
//		break;
//		case TEMPPARA_CUTOFF2KP_EDIT:
//			pProjectMan->cutoff2PID.Proportion = StringToInt32(str)/10.0;
//			AT24CXX_Write(CUTOFF2KP_BASEADDR, (uint8_t*)&pProjectMan->cutoff2PID.Proportion, 4);
//		break;
//		case TEMPPARA_CUTOFF2KI_EDIT:
//			pProjectMan->cutoff1PID.Integral = StringToInt32(str)/10.0;
//			AT24CXX_Write(CUTOFF2KI_BASEADDR, (uint8_t*)&pProjectMan->cutoff2PID.Integral, 4);
//		break;
//		case TEMPPARA_CUTOFF2KD_EDIT:
//			pProjectMan->cutoff2PID.Derivative = StringToInt32(str)/10.0;
//			AT24CXX_Write(CUTOFF2KD_BASEADDR, (uint8_t*)&pProjectMan->cutoff2PID.Derivative, 4);
//		break;
//		case TEMPPARA_CUTOFF1TEMP_EDIT:
//			pProjectMan->cutoff1Temperature = StringToInt32(str);
//			PID_UpdateSetPoint(&(pProjectMan->cutoff1PID), pProjectMan->cutoff1Temperature);
//			AT24CXX_Write(CUTOFF1TEMP_BASEADDR, (uint8_t*)&pProjectMan->cutoff1Temperature, 4);
//		break;
//		case TEMPPARA_CUTOFF2TEMP_EDIT:
//			pProjectMan->cutoff2Temperature = StringToInt32(str);
//			PID_UpdateSetPoint(&(pProjectMan->cutoff2PID), pProjectMan->cutoff2Temperature);
//			AT24CXX_Write(CUTOFF2TEMP_BASEADDR, (uint8_t*)&pProjectMan->cutoff2Temperature, 4);
//		break;
//		case TEMPPARA_FUSINGTEMP_EDIT:
//			pProjectMan->fusingTemperature = StringToInt32(str);
//			AT24CXX_Write(FUSINGTEMP_BASEADDR, (uint8_t*)&pProjectMan->fusingTemperature, 4);
//		break;
//		case TEMPPARA_FUSINGHOLDINGTEMP_EDIT:
//			pProjectMan->fusingHoldingTemperature = StringToInt32(str);
//			AT24CXX_Write(FUSINGHOLDINGTEMP_BASEADDR, (uint8_t*)&pProjectMan->fusingHoldingTemperature, 4);
//		break;
//		default:
//			cDebug("tempparaPage EDIT error!\n");
//		break;
//	}
}

#ifdef __cplusplus
}
#endif
