/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f3xx.h"
#include "main.h"
#include "midi_receive.h"
#include "system_init.h"
#include "cv_convert.h"

extern uint8_t dacbuffer[2];

int main(void)
{
	system_config();
	midi_rx_init();
	midi_to_cv_init();
	dacbuffer[0] = 0;
	dacbuffer[1] = 0;
	while(1){
		getMIDIMessages();
	}
}
