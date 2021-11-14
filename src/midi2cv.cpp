/**
 * @file: midi2cv.cpp
 *
 * @date: 2020-07-03
 *
 * @author: Lukas Güldenstein
 *
 * @brief: Implementation of Conversion from MIDI2CV
 */

/* -------------------------------- includes -------------------------------- */
#include "midi2cv.hpp"

void MIDI2CV::configure(void) {
  /* Clock Pin (PB0) */
  CLOCK_BANK.set_pin_config(CLOCK_PIN, Output, PushPull);

  /* start/stop (PB1) */
  // START_BANK.set_pin_config(START_PIN, Output, PushPull);

  /* Gates */
  stmlibxx::GPIO_Pin_Config Gates;
  Gates.Mode = Output;
  Gates.OutputType = PushPull;
  Gates.configure_bank(GPIO_A, {P4, P5, P6, P7});
  Gates.configure_bank(GPIO_B, {P0, P1, P2, P10, P11, P12, P13, P14, P15});
  Gates.configure_bank(GPIO_C, {P4, P5, P6});

  // initialize and startDAC
  if (dac1.configure() == PCM1681_STATUS_OK) {
    dac1.start();
  } else {
    while (1)
      ;
  }
}

void MIDI2CV::convert(void) { receive(); }

/* ----------MIDI-Event Handlers------------------------- */
/* Channel Events */
void MIDI2CV::NoteOn_MIDIHandler(const uint8_t &channel, const uint8_t &note,
                                 const uint8_t &velocity) {
  if (!velocity) {
    if (note == last_Note[channel]) {
      m_gate_banks[channel].pin_off(m_gate_pins[channel]);
    }
  } else {
    m_gate_banks[channel].pin_on(m_gate_pins[channel]);
    dac1.set_channel(channel,
                     (int)((note - 60) * dac1.resolution / 12 - dac1.offset));
    last_Note[channel] = note;
  }
}

void MIDI2CV::NoteOff_MIDIHandler(const uint8_t &channel, const uint8_t &note,
                                  const uint8_t &velocity) {
  if (note == last_Note[channel]) {
    m_gate_banks[channel].pin_off(m_gate_pins[channel]);
  }
}

void MIDI2CV::PolyPressure_MIDIHandler(const uint8_t &channel,
                                       const uint8_t &note,
                                       const uint8_t &pressure) {}

void MIDI2CV::ControlChange_MIDIHandler(const uint8_t &channel,
                                        const uint8_t &control,
                                        const uint8_t &value) {
  // dac1.set_channel(channel, (int)((value - 60) * dac1.resolution) -
  // dac1.offset);
}
void MIDI2CV::PitchBend_MIDIHandler(const uint8_t &channel, const uint8_t &note,
                                    const uint8_t &velocity) {}
void MIDI2CV::ProgramChange_MIDIHandler(const uint8_t &channel,
                                        const uint8_t &programm) {}
void MIDI2CV::ChannelPressure_MIDIHandler(const uint8_t &channel,
                                          const uint8_t &pressure) {}
/* System Events */
void MIDI2CV::Clock_MIDIHandler(void) {
  if (start == 1) {
    if (!first) {
      CLOCK_BANK.pin_on(CLOCK_PIN);
      first = 1;
    }
    count++;
    if (count == 12) {
      if (!state) {
        CLOCK_BANK.pin_off(CLOCK_PIN);
      } else if (state) {
        CLOCK_BANK.pin_on(CLOCK_PIN);
      }
      count = 0;
      state ^= 1;
    }
  } else if (start == 0) {
    count = 0;
    CLOCK_BANK.pin_off(CLOCK_PIN);
  }
}

void MIDI2CV::Start_MIDIHandler(void) {
  // START_BANK.pin_on(START_PIN);
  start = 1;
  first = 0;
  count = 0;
  state = 0;
}

void MIDI2CV::Stop_MIDIHandler(void) {
  // START_BANK.pin_off(START_PIN);
  start = 0;
  first = 0;
  count = 0;
  state = 0;
}

void MIDI2CV::Continue_MIDIHandler(void) {}

void MIDI2CV::SystemReset_MIDIHandler(void) {}

void MIDI2CV::SysExStart_MIDIHandler(void) { __NOP(); }
void MIDI2CV::SysExStop_MIDIHandler(void) {}
