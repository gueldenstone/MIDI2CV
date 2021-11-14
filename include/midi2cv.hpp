/**
 * @file: midi2cv.hpp
 *
 * @date: 2020-07-03
 *
 * @author: Lukas Güldenstein
 */

#ifndef _MIDI2CV_HPP
#define _MIDI2CV_HPP

#define SAMPLE_RATE 48000U
#define OVRSAMPLE 256U
#define I2C_ADDRESS 0b10011000
/* -------------------------------- includes -------------------------------- */
#include "midi.hpp"
#include "pcm1681.hpp"
#include "stm32g474xx.h"

/* --------------------------------- defines -------------------------------- */
/* Clock/Transport */
#define CLOCK_PIN P0
#define START_PIN P11
#define CLOCK_BANK GPIO_B
#define START_BANK GPIO_A

/* Gates */
#define GATE1_PIN P4
#define GATE2_PIN P5
#define GATE3_PIN P6
#define GATE4_PIN P7
#define GATE5_PIN P4
#define GATE6_PIN P5
#define GATE7_PIN P0
#define GATE8_PIN P1
#define GATE9_PIN P2
#define GATE10_PIN P10
#define GATE11_PIN P11
#define GATE12_PIN P12
#define GATE13_PIN P13
#define GATE14_PIN P14
#define GATE15_PIN P15
#define GATE16_PIN P6

#define GATE1_BANK GPIO_A
#define GATE2_BANK GPIO_A
#define GATE3_BANK GPIO_A
#define GATE4_BANK GPIO_A
#define GATE5_BANK GPIO_C
#define GATE6_BANK GPIO_C
#define GATE7_BANK GPIO_B
#define GATE8_BANK GPIO_B
#define GATE9_BANK GPIO_B
#define GATE10_BANK GPIO_B
#define GATE11_BANK GPIO_B
#define GATE12_BANK GPIO_B
#define GATE13_BANK GPIO_B
#define GATE14_BANK GPIO_B
#define GATE15_BANK GPIO_B
#define GATE16_BANK GPIO_C

/* -------------------------- SysExMessage defines -------------------------- */
enum Analog_Out_Mode { none = 0, note = 1, control = 2 };

class MIDI2CV : public MIDI<MIDI2CV> {
  friend class MIDI<MIDI2CV>;

public:
  MIDI2CV() { configure(); }
  void convert(void);

private:
  void configure(void);

private:
  void set_all_gates(bool *);

private:
  // Handler functions from MIDI
  void NoteOn_MIDIHandler(const uint8_t &, const uint8_t &, const uint8_t &);
  void NoteOff_MIDIHandler(const uint8_t &, const uint8_t &, const uint8_t &);
  void PolyPressure_MIDIHandler(const uint8_t &, const uint8_t &,
                                const uint8_t &);
  void ControlChange_MIDIHandler(const uint8_t &, const uint8_t &,
                                 const uint8_t &);
  void PitchBend_MIDIHandler(const uint8_t &, const uint8_t &, const uint8_t &);
  void ProgramChange_MIDIHandler(const uint8_t &, const uint8_t &);
  void ChannelPressure_MIDIHandler(const uint8_t &, const uint8_t &);
  void Clock_MIDIHandler(void);
  void Start_MIDIHandler(void);
  void Stop_MIDIHandler(void);
  void Continue_MIDIHandler(void);
  void SystemReset_MIDIHandler(void);
  void SysExStart_MIDIHandler(void);
  void SysExStop_MIDIHandler(void);

private:
  PCM1681 dac1 = PCM1681(SAMPLE_RATE, OVRSAMPLE, I2C_ADDRESS);

private:
  stmlibxx::GPIO m_gate_banks[16] = {
      GATE1_BANK,  GATE2_BANK,  GATE3_BANK,  GATE4_BANK,
      GATE5_BANK,  GATE6_BANK,  GATE7_BANK,  GATE8_BANK,
      GATE9_BANK,  GATE10_BANK, GATE11_BANK, GATE12_BANK,
      GATE13_BANK, GATE14_BANK, GATE15_BANK, GATE16_BANK};

  const GPIO_Pin m_gate_pins[16] = {
      GATE1_PIN,  GATE2_PIN,  GATE3_PIN,  GATE4_PIN,  GATE5_PIN,  GATE6_PIN,
      GATE7_PIN,  GATE8_PIN,  GATE9_PIN,  GATE10_PIN, GATE11_PIN, GATE12_PIN,
      GATE13_PIN, GATE14_PIN, GATE15_PIN, GATE16_PIN};
  uint8_t last_Note[16] = {};
};

#endif /* _MIDI2CV_HPP */
