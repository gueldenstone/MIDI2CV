/*
 * cv_convert.c
 *
 *  Created on: 09.02.2020
 *      Author: lukas
 */
#include "cv_convert.h"

volatile uint8_t dacBuf1, dacBuf2, dacbuffer[2];


void midi_to_cv_init(void){
	/* Clock Pin (PB0) */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;								// clock for GPIOB
	GPIOB->MODER &= ~0x3;											// reset mode for PB0
	GPIOB->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER0_Pos;	// clock output PB0

	/* start/stop (PB1) */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;								// clock for GPIOB
	GPIOB->MODER &= ~0xC;											// reset mode for PB1
	GPIOB->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER1_Pos;	// start/stop output PB1

	/* Gates / Triggers */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOBEN;			// clock for GPIOA & B
	GPIOA->MODER &= ~0xFFFF;										// Reset mode for pins
	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER0_Pos;	// Trig 1 (PA0)
	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER1_Pos;	// Trig 2 (PA1)
	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER2_Pos;	// Trig 3 (PA2)
	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER3_Pos;	// Trig 4 (PA3)
//	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER4_Pos;	// Trig 5 (PA4)
//	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER5_Pos;	// Trig 6 (PA5)
	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER6_Pos;	// Trig 7 (PA6)
	GPIOA->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER7_Pos;	// Trig 8 (PA7)

	GPIOB->MODER &= ~0xFF00;										// Reset mode for pins
	GPIOB->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER4_Pos;	// Gate 1 (PB4)
	GPIOB->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER5_Pos;	// Gate 2 (PB5)
	GPIOB->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER6_Pos;	// Gate 3 (PB6)
	GPIOB->MODER |= GPIOx_MODER_OUTPUT << GPIO_MODER_MODER7_Pos;	// Gate 4 (PB7)



	/* DAC config PA4/PA5*/
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;								// clock for GPIOA
	RCC->APB1ENR |=  RCC_APB1ENR_TIM6EN | RCC_APB1ENR_DAC1EN;		// clock for DAC1 & TIM6 interface
	GPIOA->MODER |= GPIOx_MODER_ANALOG << GPIO_MODER_MODER4_Pos;	// alternate function on PA4
	GPIOA->MODER |= GPIOx_MODER_ANALOG << GPIO_MODER_MODER5_Pos;	// alternate function on PA5
	DAC1->CR |= DAC_CR_TEN1 | DAC_CR_TEN2 | DAC_CR_EN1 | DAC_CR_EN2;
	TIM6->PSC = 14; 						/* Trigger @48kHz */
	TIM6->ARR = 99;
	TIM6->CR2 |= 0b010 << TIM_CR2_MMS_Pos;	/* Update Event as TRGO */
	TIM6->DIER |= TIM_DIER_UIE;				/* enable interrupt for data transfer */
	TIM6->CR1 |= TIM_CR1_CEN; 				/* enable TIM6 */
	NVIC_EnableIRQ(TIM6_DAC_IRQn);
}

void TIM6_DACUNDER_IRQHandler(void){
	DAC1->DHR8RD = ( dacbuffer[0] | (dacbuffer[1] << 8));
	/* ISR finished */
	TIM6->SR &= ~TIM_SR_UIF;
}


/* ----------MIDI-Event Handlers------------------------- */
/* Channel Events */
void NoteOn_MIDIHandler(uint8_t channel, uint8_t note, uint8_t velocity){
	switch(channel){
	/* Channel 1 */
	case 0:
		if(!velocity)
			GATE1_OFF;
		else
			GATE1_ON;
		break;

	/* Channel 2 */
	case 1:
		if(!velocity)
			GATE2_OFF;
		else
			GATE2_ON;
		break;

	/* Channel 3 */
	case 2:
		if(!velocity)
			GATE3_OFF;
		else
			GATE3_ON;
		break;

	/* Channel 4 */
	case 3:
		if(!velocity)
			GATE4_OFF;
		else
			GATE4_ON;
		break;

	/* Channel 5 */
	case 4:
		if(!velocity)
			TRIG1_OFF;
		else
			TRIG1_ON;
		break;

	/* Channel 6 */
	case 5:
		if(!velocity)
			TRIG2_OFF;
		else
			TRIG2_ON;
		break;

	/* Channel 7 */
	case 6:
		if(!velocity)
			TRIG3_OFF;
		else
			TRIG3_ON;
		break;

	/* Channel 8 */
	case 7:
		if(!velocity)
			TRIG4_OFF;
		else
			TRIG4_ON;
		break;

	/* Channel 9 */
	case 8:
		if(!velocity)
			TRIG5_OFF;
		else
			TRIG5_ON;
		break;

	/* Channel 10 */
	case 9:
		if(!velocity)
			TRIG6_OFF;
		else
			TRIG6_ON;
		break;

	/* Channel 11 */
	case 10:
		if(!velocity)
			TRIG7_OFF;
		else
			TRIG7_ON;
		break;

	/* Channel 12 */
	case 11:
		if(!velocity)
			TRIG8_OFF;
		else
			TRIG8_ON;
		break;

//	/* Channel 13 */
//	case 12:
//		if(!velocity)
//			TRIG1_OFF;
//		else
//			TRIG1_ON;
//		break;
//
//	/* Channel 14 */
//	case 13:
//		if(!velocity)
//			TRIG1_OFF;
//		else
//			TRIG1_ON;
//		break;
//
//	/* Channel 15 */
//	case 14:
//		if(!velocity)
//			TRIG1_OFF;
//		else
//			TRIG1_ON;
//		break;
//
//	/* Channel 16 */
//	case 15:
//		if(!velocity)
//			TRIG1_OFF;
//		else
//			TRIG1_ON;
//		break;
	}
}

void NoteOff_MIDIHandler(uint8_t channel, uint8_t note, uint8_t velocity){
	switch(channel){
	/* Channel 1 */
	case 0:
		GATE1_OFF;
		break;

	/* Channel 2 */
	case 1:
		GATE2_OFF;
		break;

	/* Channel 3 */
	case 2:
		GATE3_OFF;
		break;

	/* Channel 4 */
	case 3:
		GATE4_OFF;
		break;

	/* Channel 5 */
	case 4:
		TRIG1_OFF;
		break;

	/* Channel 6 */
	case 5:
		TRIG2_OFF;
		break;

	/* Channel 7 */
	case 6:
		TRIG3_OFF;
		break;

	/* Channel 8 */
	case 7:
		TRIG4_OFF;
		break;

	/* Channel 9 */
	case 8:
		TRIG5_OFF;
		break;

	/* Channel 10 */
	case 9:
		TRIG6_OFF;
		break;

	/* Channel 11 */
	case 10:
		TRIG7_OFF;
		break;

	/* Channel 12 */
	case 11:
		TRIG8_OFF;
		break;

//	/* Channel 13 */
//	case 12:
//		TRIG1_OFF;
//		break;
//
//	/* Channel 14 */
//	case 13:
//		TRIG1_OFF;
//		break;
//
//	/* Channel 15 */
//	case 14:
//		TRIG1_OFF;
//		break;
//
//	/* Channel 16 */
//	case 15:
//		TRIG1_OFF;
//		break;
	}

}

void PolyPressure_MIDIHandler(uint8_t channel, uint8_t note, uint8_t pressure){

}

void ControlChange_MIDIHandler(uint8_t channel, uint8_t control, uint8_t value){
	switch(channel){
	/* Channel 1 */
	case 0:
//		i2cData[0] = ;
		break;

	/* Channel 2 */
	case 1:
		GATE2_OFF;
		break;

	/* Channel 3 */
	case 2:
		GATE3_OFF;
		break;

	/* Channel 4 */
	case 3:
		GATE4_OFF;
		break;

	/* Channel 5 */
	case 4:
		TRIG1_OFF;
		break;

	/* Channel 6 */
	case 5:
		TRIG2_OFF;
		break;

	/* Channel 7 */
	case 6:
		TRIG3_OFF;
		break;

	/* Channel 8 */
	case 7:
		TRIG4_OFF;
		break;

	/* Channel 9 */
	case 8:
		TRIG5_OFF;
		break;

	/* Channel 10 */
	case 9:
		TRIG6_OFF;
		break;

	/* Channel 11 */
	case 10:
		TRIG7_OFF;
		break;

	/* Channel 12 */
	case 11:
		TRIG8_OFF;
		break;

//	/* Channel 13 */
//	case 12:
//		TRIG1_OFF;
//		break;
//
//	/* Channel 14 */
//	case 13:
//		TRIG1_OFF;
//		break;
//
//	/* Channel 15 */
//	case 14:
//		TRIG1_OFF;
//		break;
//
//	/* Channel 16 */
//	case 15:
//		TRIG1_OFF;
//		break;
	}
}

void PitchBend_MIDIHandler(uint8_t channel, uint8_t note, uint8_t velocity){

}

void ProgramChange_MIDIHandler(uint8_t channel, uint8_t programm){

}

void ChannelPressure_MIDIHandler(uint8_t channel, uint8_t pressure){

}

volatile _Bool start;
/* System Events */
void Clock_MIDIHandler(void){
	volatile static _Bool state=0;
	static int8_t count=0;
	if(start==1){
		GPIOB->BSRR |= GPIO_BSRR_BS_0;
		count++;
		if(count==12){
				if(!state){
					GPIOB->BSRR |= GPIO_BSRR_BR_0;
				}
				else if(state){
					GPIOB->BSRR |= GPIO_BSRR_BS_0;
				}
				count=0;
				state ^= 1;
			}
	}
	else if(start==0){
			count=-1;
			GPIOB->BSRR |= GPIO_BSRR_BR_0;
	}

}

void Start_MIDIHandler(void){
	GPIOB->BSRR |= GPIO_BSRR_BS_1;		// set PB1 to high (start)
	start=1;
}

void Stop_MIDIHandler(void){
	GPIOB->BSRR |= GPIO_BSRR_BR_1;		// reset PB1 to low (stop)
	start=0;
}

void Continue_MIDIHandler(void){

}

void SystemReset_MIDIHandler(void){

}
