#include "management.h"
#include "stepmotor.h"
#include "timer.h"
#include "io.h"
#include "uart.h"
#include "cmd_process.h"

#include <stdio.h>

Man_TypeDef man;

void InitMan(void)
{
	man.ddc112Data = 0;

    man.index = 0;
    man.bufferSize = 4;

    man.integralTime = 1;

    man.control = 0;
    man.dataSendFlag = 0;
    man.dataValidFlag = 0;


}

