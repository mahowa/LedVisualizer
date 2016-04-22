

#include <stdio.h>
#include <stdlib.h>
#include "diag/Trace.h"
#include "stm32f0xx.h"

#include "Lights.h"

// Sample pragmas to cope with warnings. Please note the related line at
// the end of this function, used to pop the compiler diagnostics status.
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"
#pragma GCC diagnostic ignored "-Wmissing-declarations"
#pragma GCC diagnostic ignored "-Wreturn-type"


/*=====================================================================================
	Private Functions
=======================================================================================*/

/*
 * Initializes PC8 in Output Mode
 */







/*=====================================================================================
	Main Entry Point to Program
=======================================================================================*/
int main(void){
	INITI();

	TIM3_PWM_P8(17000);
	TIM3_PWM_P7(17000);
	while(1){}
}

#pragma GCC diagnostic pop

// ----------------------------------------------------------------------------
