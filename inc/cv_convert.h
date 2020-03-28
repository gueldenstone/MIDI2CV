#ifndef _CV_CONVERT_H
#define _CV_CONVERT_H

#include "main.h"

/* local types */


/* function prototypes */
void sendPulse(void);
void midi_to_cv_init(void);
void TIM6_DAC_IRQHandler(void);

/* MIDI-Event Handlers */

void NoteOn_MIDIHandler(uint8_t, uint8_t, uint8_t);
void NoteOff_MIDIHandler(uint8_t, uint8_t, uint8_t);
void PolyPressure_MIDIHandler(uint8_t, uint8_t, uint8_t);
void ControlChange_MIDIHandler(uint8_t, uint8_t, uint8_t);
void PitchBend_MIDIHandler(uint8_t, uint8_t, uint8_t);
void ProgramChange_MIDIHandler(uint8_t, uint8_t);
void ChannelPressure_MIDIHandler(uint8_t, uint8_t);
void Clock_MIDIHandler(void);
void Start_MIDIHandler(void);
void Stop_MIDIHandler(void);
void Continue_MIDIHandler(void);
void SystemReset_MIDIHandler(void);

/* local defines */

/* Gates */
#define GATE1_ON GPIOB->BSRR |= GPIO_BSRR_BS_4
#define GATE2_ON GPIOB->BSRR |= GPIO_BSRR_BS_5
#define GATE3_ON GPIOB->BSRR |= GPIO_BSRR_BS_6
#define GATE4_ON GPIOB->BSRR |= GPIO_BSRR_BS_7

#define GATE1_OFF GPIOB->BSRR |= GPIO_BSRR_BR_4
#define GATE2_OFF GPIOB->BSRR |= GPIO_BSRR_BR_5
#define GATE3_OFF GPIOB->BSRR |= GPIO_BSRR_BR_6
#define GATE4_OFF GPIOB->BSRR |= GPIO_BSRR_BR_7

#define TRIG1_ON GPIOA->BSRR |= GPIO_BSRR_BS_0
#define TRIG2_ON GPIOA->BSRR |= GPIO_BSRR_BS_1
#define TRIG3_ON GPIOA->BSRR |= GPIO_BSRR_BS_2
#define TRIG4_ON GPIOA->BSRR |= GPIO_BSRR_BS_3
#define TRIG5_ON GPIOA->BSRR |= GPIO_BSRR_BS_4
#define TRIG6_ON GPIOA->BSRR |= GPIO_BSRR_BS_5
#define TRIG7_ON GPIOA->BSRR |= GPIO_BSRR_BS_6
#define TRIG8_ON GPIOA->BSRR |= GPIO_BSRR_BS_7

#define TRIG1_OFF GPIOA->BSRR |= GPIO_BSRR_BR_0
#define TRIG2_OFF GPIOA->BSRR |= GPIO_BSRR_BR_1
#define TRIG3_OFF GPIOA->BSRR |= GPIO_BSRR_BR_2
#define TRIG4_OFF GPIOA->BSRR |= GPIO_BSRR_BR_3
#define TRIG5_OFF GPIOA->BSRR |= GPIO_BSRR_BR_4
#define TRIG6_OFF GPIOA->BSRR |= GPIO_BSRR_BR_5
#define TRIG7_OFF GPIOA->BSRR |= GPIO_BSRR_BR_6
#define TRIG8_OFF GPIOA->BSRR |= GPIO_BSRR_BR_7



#endif /* _CV_CONVERT_H */
