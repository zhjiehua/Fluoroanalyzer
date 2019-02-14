#ifndef _PAGE_COMMON_H_
#define _PAGE_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "string.h"
#include "../HMI/hmi_user_uart.h"
#include "../HMI/hmi_driver.h"
#include "../HMI/cmd_process.h"

/************************************************************************/
/* 页面ID定义                                                           */
/************************************************************************/
#define LOGOPAGE_INDEX					0
#define MAINPAGE_INDEX					1
#define FASTTESTPAGE_INDEX			2
#define CALIBPAGE_INDEX					3
#define RESULTPAGE_INDEX				4
#define SETUPPAGE_INDEX					5
#define PARAMETERPAGE_INDEX			6
#define CURVEPAGE_INDEX					7
#define TIPSPAGE_INDEX					8
#define TIPS1PAGE_INDEX					9
#define MANUALPAGE_INDEX				10
#define PROMANPAGE_INDEX				11

/************************************************************************/
/* LOGO页面控件ID                                                       */
/************************************************************************/
#define LOGO_START_BUTTON					1

/************************************************************************/
/* 主页面控件ID                                                         */
/************************************************************************/
#define MAIN_FASTTEST_BUTTON			1
#define MAIN_PARACALIB_BUTTON			2
#define MAIN_SETUP_BUTTON					3
#define MAIN_PARAMETER_BUTTON			4
#define MAIN_HISTORY_BUTTON				5
#define MAIN_PROMAN_BUTTON				6

/************************************************************************/
/* 快速测试页面控件ID                                                   */
/************************************************************************/
#define FASTTEST_PROJECT_EDIT			1
#define FASTTEST_BATCHNUM_EDIT		2
#define FASTTEST_SAMPLENUM_EDIT		3
#define FASTTEST_RETURN_BUTTON		4
#define FASTTEST_START_BUTTON			5

/************************************************************************/
/* 校准页面控件ID                                                   */
/************************************************************************/
#define CALIB_FACTOR_EDIT					1
#define CALIB_SETUP_BUTTON				2
#define CALIB_RETURN_BUTTON				3
#define CALIB_RTC									6

/************************************************************************/
/* 测试结果页面控件ID                                                   */
/************************************************************************/
#define RESULT_DATA_EDIT					1
#define RESULT_TIME_EDIT					2
#define RESULT_C_VALUE_EDIT				3
#define RESULT_UNIT_EDIT					4
#define RESULT_RESULT_EDIT				5
#define RESULT_T_VALUE_EDIT				6
#define RESULT_T_C_EDIT						7
#define RESULT_CONCENT_EDIT				8

#define RESULT_RETURN_BUTTON			9
#define RESULT_CURVE_BUTTON				10
#define RESULT_TRANS_BUTTON				11
#define RESULT_PRINT_BUTTON				12

/************************************************************************/
/* 设置页面控件ID                                                  	 	*/
/************************************************************************/
#define SETUP_QRCODE_BUTTON				1
#define SETUP_PRINTER_BUTTON			2
#define SETUP_ALARM_BUTTON				3
#define SETUP_TOUCH_BUTTON				4
#define SETUP_QRINIT_BUTTON				5

#define SETUP_RETURN_BUTTON				9

/************************************************************************/
/* 参数页面控件ID                                                  	 	*/
/************************************************************************/
#define PARAMETER_SAMPLESTART_EDIT	1
#define PARAMETER_SAMPLESTOP_EDIT		2
#define PARAMETER_HOME_BUTTON				3
#define PARAMETER_GO_SP_BUTTON			4
#define PARAMETER_GO_EP_BUTTON			5

#define PARAMETER_RETURN_BUTTON			9
#define PARAMETER_MANUAL_BUTTON			10

/************************************************************************/
/* 曲线页面控件ID                                                  	 	*/
/************************************************************************/
#define CURVE_GRAPHY							1
#define CURVE_XZOOMIN_BUTTON			2
#define CURVE_XZOOMOUT_BUTTON			3
#define CURVE_UP_BUTTON						4
#define CURVE_DOWN_BUTTON					5
#define CURVE_LEFT_BUTTON					6
#define CURVE_RIGHT_BUTTON				7
#define CURVE_YZOOMIN_BUTTON			9
#define CURVE_YZOOMOUT_BUTTON			10

#define CURVE_RETURN_BUTTON				8

/************************************************************************/
/* 提示页面控件ID                                                  	 	*/
/************************************************************************/
#define TIPS0_TIPS_EDIT						1
#define TIPS1_TIPS_EDIT						1
#define TIPS1_OK_BUTTON						2

/************************************************************************/
/* 手动页面控件ID                                                  	 	*/
/************************************************************************/
#define MANUAL_FRONT_BUTTON				1
#define MANUAL_BACK_BUTTON				2
#define MANUAL_LAMP_BUTTON				3
#define MANUAL_SAMPLE_BUTTON			4

#define MANUAL_RETURN_BUTTON			9

/************************************************************************/
/* 项目管理页面控件ID                                                  	 	*/
/************************************************************************/
#define PROMAN_PROJECT_BUTTON			1
#define PROMAN_PROJECT_EDIT				2
#define PROMAN_BATCH_BUTTON				3
#define PROMAN_BATCH_EDIT					4
#define PROMAN_CX_EDIT						5
#define PROMAN_TX_EDIT						6

#define PROMAN_RETURN_BUTTON			9


/************************************************************************/
/* 软件版本                                                             */
/************************************************************************/
#define PANEL_VERSION					"9.1.7"
#define VERSION							"1.0.0"

/*
 * 函数声明
 */
void logoPageButtonProcess(uint16 control_id, uint8 state);
 
void mainPageButtonProcess(uint16 control_id, uint8 state);

void fastTestPageButtonProcess(uint16 control_id, uint8 state);
void fastTestPageEditProcess(uint16 control_id, uint8 *str);

void calibPageButtonProcess(uint16 control_id, uint8 state);
void calibPageEditProcess(uint16 control_id, uint8 *str);

void resultPageButtonProcess(uint16 control_id, uint8 state);
void resultPageEditProcess(uint16 control_id, uint8 *str);

void setupPageButtonProcess(uint16 control_id, uint8 state);

void parameterPageButtonProcess(uint16 control_id, uint8 state);
void parameterPageEditProcess(uint16 control_id, uint8 *str);

void curvePageButtonProcess(uint16 control_id, uint8  state);
void tips1PageButtonProcess(uint16 control_id, uint8 state);

void manualPageButtonProcess(uint16 control_id, uint8 state);

void proManPageButtonProcess(uint16 control_id, uint8 state);
void proManPageEditProcess(uint16 control_id, uint8 *str);

#ifdef __cplusplus
}
#endif

#endif
