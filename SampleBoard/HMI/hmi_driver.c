#include "hmi_driver.h"

#ifdef __cplusplus
extern "C" {
#endif

#if (DATASAFTY_CHECK_ENABLE==1)
uint8 CheckDataSafty(uint8 *buffer, uint16 n)
{
	uint16 i;
	for(i=0;i<n;i++)
	{
		if(buffer[i]&0x80)
			return 0;
	}
	return 1;
}
#endif

#ifdef __cplusplus
}
#endif