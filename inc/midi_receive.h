#ifndef _MIDI_RECEIVE_H
#define _MIDI_RECEIVE_H

#include "main.h"
#include "cv_convert.h"

/* local types */
typedef enum {noteOn, noteOff, polyPressure, controlChange, programChange, channelPressure, pitchBend} MIDIVoiceMessageType;
typedef struct{
	MIDIVoiceMessageType type;
	uint8_t channelNum;
	uint8_t databyte0;
	uint8_t databyte1;
}TypeDefMIDIVoiceMessage;


/* function prototypes */
void midi_rx_init(void);
uint8_t get_midibyte(void);
void getMIDIMessages(void);


/* local defines */
#define PULSE 20
/* status types */
#define NOTE_ON 0x90
#define NOTE_OFF 0x80
#define POLY_PRESSURE 0xA0
#define CONTROL_CHANGE 0xB0
#define PROGRAM_CHANGE 0xC0
#define CHANNEL_PRESSURE 0xD0
#define PITCH_BEND 0xE0
#define SYSTEM 0xF0

/* realtime messages */
#define TIMING_CLOCK 0xF8
#define START 0xFA
#define CONTINUE 0xFB
#define STOP 0xFC
#define SYSTEM_RESET 0xFF


#endif /* _MIDI_RECEIVE_H */


