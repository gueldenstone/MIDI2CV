/*
 * midi_receive.c
 *
 *  Created on: 07.02.2020
 *      Author: lukas
 */
#include "midi_receive.h"

volatile _Bool F_midipollreq;
volatile uint8_t midibyte;
volatile TypeDefMIDIVoiceMessage MIDIMessage;
uint8_t get_midibyte(void){
	return midibyte;
}

void midi_rx_init(void){
	/* UART Init */
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;		/* RX MIDI on PA10 */
	RCC->APB2ENR |= RCC_APB2ENR_USART1EN;	/* clock for USART1 */
	GPIOA->MODER |= GPIOx_MODER_ALT << GPIO_MODER_MODER10_Pos;	/* initialize PA10 for USART1 RX */
	GPIOA->AFR[1] |= 0b111 << GPIO_AFRH_AFRH2_Pos;

	USART1->CR2 |= 0b00 << USART_CR2_STOP_Pos; /* 1 stop-bit */
	USART1->GTPR |= 0b00000001 << USART_GTPR_PSC_Pos; /* PSC = 1 */
	USART1->BRR |= SystemCoreClock/31250; /* baud rate is 3125000baud */
	USART1->CR1 |= USART_CR1_RXNEIE; /* enable interrupt for not empty RX register */
	USART1->CR1 |= USART_CR1_RE; /* receive on */
	USART1->CR1 |= USART_CR1_UE; /* enable uart interface */
	NVIC_EnableIRQ(USART1_IRQn);

}

void USART1_EXTI25_IRQHandler(void){
	static uint8_t expDataByteNum, dataByteNum;
 	midibyte = USART1->RDR;					/* get byte from MIDI-Message */
	if(midibyte & 0x80){					/* determine wether status or data, if status */
		if((midibyte & 0xF0) == 0xF0){		/* check for system message */
			switch(midibyte){
			case TIMING_CLOCK:
				Clock_MIDIHandler();
				break;
			case START:
				Start_MIDIHandler();
				break;
			case CONTINUE:
				Continue_MIDIHandler();
				break;
			case STOP:
				Stop_MIDIHandler();
				break;
			case SYSTEM_RESET:
				SystemReset_MIDIHandler();
				break;
			}
		}
		else{
		switch(midibyte & 0xF0){		/* determine status and channel num for voice Messages */
		case NOTE_ON:
			MIDIMessage.type=noteOn;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 2;
			break;
		case NOTE_OFF:
			MIDIMessage.type=noteOff;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 2;
			break;
		case POLY_PRESSURE:
			MIDIMessage.type=polyPressure;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 2;
			break;
		case CONTROL_CHANGE:
			MIDIMessage.type=controlChange;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 2;
			break;
		case PROGRAM_CHANGE:
			MIDIMessage.type=programChange;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 1;
			break;
		case CHANNEL_PRESSURE:
			MIDIMessage.type=channelPressure;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 1;
			break;
		case PITCH_BEND:
			MIDIMessage.type=pitchBend;
			MIDIMessage.channelNum=(midibyte & 0x0F);
			expDataByteNum = 2;
			break;
		}
		}
	}
	else if(!(midibyte & 0x80)){						/* if data, write data */
			switch(dataByteNum){
			case 0:
				MIDIMessage.databyte0 = midibyte;
				break;
			case 1:
				MIDIMessage.databyte1 = midibyte;
			}
			dataByteNum++;
			if(dataByteNum >= expDataByteNum){
				dataByteNum = 0;
				F_midipollreq=1;
			}
	}
	USART1->ICR = 0xFFFFFFFF;

}


void getMIDIMessages(void){
	if(F_midipollreq){
		switch(MIDIMessage.type){
		case noteOn:
			NoteOn_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0, MIDIMessage.databyte1);
			F_midipollreq=0;
			break;
		case noteOff:
			NoteOff_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0, MIDIMessage.databyte1);
			F_midipollreq=0;
			break;
		case polyPressure:
			PolyPressure_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0, MIDIMessage.databyte1);
			F_midipollreq=0;
			break;
		case controlChange:
			ControlChange_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0, MIDIMessage.databyte1);
			F_midipollreq=0;
			break;
		case programChange:
			ProgramChange_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0);
			F_midipollreq=0;
			break;
		case channelPressure:
			ChannelPressure_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0);
			F_midipollreq=0;
			break;
		case pitchBend:
			PitchBend_MIDIHandler(MIDIMessage.channelNum, MIDIMessage.databyte0, MIDIMessage.databyte1);
			F_midipollreq=0;
			break;
		}
	}
}
