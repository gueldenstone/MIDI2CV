/**
 * @file: midi.hpp
 *
 * @date: 2020-06-09
 *
 * @author: Lukas Güldenstein
 *
 * @info: Include this header in your project.
 *        derive the class, which will use the MIDI interface from the MIDI
 * class using: User_Class : public MIDI<User_Class> Implement the Handlers
 *
 *        void NoteOn_MIDIHandler(const uint8_t&, const uint8_t&, const
 * uint8_t&); void NoteOff_MIDIHandler(const uint8_t&, const uint8_t&, const
 * uint8_t&); void PolyPressure_MIDIHandler(const uint8_t&, const uint8_t&,
 * const uint8_t&); void ControlChange_MIDIHandler(const uint8_t&, const
 * uint8_t&, const uint8_t&); void PitchBend_MIDIHandler(const uint8_t&, const
 * uint8_t&, const uint8_t&); void ProgramChange_MIDIHandler(const uint8_t&,
 * const uint8_t&); void ChannelPressure_MIDIHandler(const uint8_t&, const
 * uint8_t&); void Clock_MIDIHandler(void); void Start_MIDIHandler(void); void
 * Stop_MIDIHandler(void); void Continue_MIDIHandler(void); void
 * SystemReset_MIDIHandler(void);
 *
 *        in your using class.
 *
 */

#ifndef _MIDI_HPP
#define _MIDI_HPP

/* -------------------------------- includes -------------------------------- */
#include "stm32g474xx.h"
#include "stmlibxx/gpio.hpp"
#include "stmlibxx/interrupt.hpp"

/* ------------------------------- local types ------------------------------ */
enum MIDIMessageType : uint8_t {
  NoteOn = 0x90,
  NoteOff = 0x80,
  PolyPressure = 0xA0,
  ControlChange = 0xB0,
  ProgramChange = 0xC0,
  ChannelPressure = 0xD0,
  PitchBend = 0xE0,
  TimingClock = 0xF8,
  SysExStart = 0xF0,
  SysExStop = 0xF7,
  Start = 0xFA,
  Continue = 0xFB,
  Stop = 0xFC,
  SystemReset = 0xFF
};
const uint8_t msb = 0x80;
const uint8_t System = 0xF0;
typedef struct {
  MIDIMessageType type;
  uint8_t status_data;
  uint8_t databyte0;
  uint8_t databyte1;
} TypeDefMIDIMessage;

/* ---------------------------- class definitions --------------------------- */
template <typename Handler>
class MIDI : public stmlibxx::Interrupt<MIDI<Handler>, UART4_IRQn> {
  friend class stmlibxx::Interrupt<MIDI<Handler>, UART4_IRQn>;

public:
  MIDI() { configure(); }

protected:
  uint8_t F_midipollreq = 0;
  uint8_t midibyte = 0;
  TypeDefMIDIMessage MIDIMessage;
  volatile uint8_t start = 0, count = 0, first = 0, state = 0;

private:
  // casting to allow use of handler functions from child class
  Handler &handler = static_cast<Handler &>(*this);

private:
  void configure(void) {
    // UART Init */
    GPIO_C.set_pin_config(P11, Alternate, AF5);

    RCC->CCIPR &= ~RCC_CCIPR_UART4SEL_Msk;
    RCC->CCIPR |= 0b10 << RCC_CCIPR_UART4SEL_Pos; // select HSI
    RCC->APB1ENR1 |= RCC_APB1ENR1_UART4EN;        // clock for USART1
    UART4->CR2 |= 0b00 << USART_CR2_STOP_Pos;     // 1 stop-bit
    UART4->BRR |= 16000000 / 31250;               // baud rate is 3125000baud
    UART4->CR3 |= USART_CR3_OVRDIS; // disable overrun, not fully received data
                                    // will be discarded
    UART4->CR1 |=
        USART_CR1_RXNEIE;       // enable interrupt for not empty RX register
    UART4->CR1 |= USART_CR1_RE; // receive on
    UART4->CR1 |= USART_CR1_UE; // enable uart interface
    NVIC_EnableIRQ(UART4_IRQn);
  }

private:
  void IRQHandler(void) {
    static uint8_t expDataByteNum, dataByteNum;
    midibyte = UART4->RDR;               // get byte from MIDI-Message
    if ((midibyte & System) == System) { /* check for system message */
      switch (midibyte) { // System Messages are handled inside the interrupt
                          // for better timing
      case TimingClock:
        handler.Clock_MIDIHandler();
        break;
      case Start:
        handler.Start_MIDIHandler();
        break;
      case Continue:
        handler.Continue_MIDIHandler();
        break;
      case Stop:
        handler.Stop_MIDIHandler();
        break;
      case SystemReset:
        handler.SystemReset_MIDIHandler();
        break;
      }
    } else if (midibyte & msb) { // determine wether status or data, if status
      MIDIMessage.type =
          (MIDIMessageType)(midibyte & 0xF0); // extract the message type
      MIDIMessage.status_data = (midibyte & 0x0F);
      if (MIDIMessage.type == (ProgramChange | ChannelPressure)) {
        expDataByteNum = 1;
      } else {
        expDataByteNum = 2;
      }
    } else if (!(midibyte & msb)) { // if data, write data
      switch (dataByteNum) {
      case 0:
        MIDIMessage.databyte0 = midibyte;
        break;
      case 1:
        MIDIMessage.databyte1 = midibyte;
      }
      dataByteNum++;
      if (dataByteNum >= expDataByteNum) {
        dataByteNum = 0;
        F_midipollreq = 1;
      }
    }
  }

protected:
  void receive(void) {
    if (F_midipollreq) {
      switch (MIDIMessage.type) {
      case TimingClock:
        handler.Clock_MIDIHandler();
        F_midipollreq = 0;
        break;
      case Start:
        handler.Start_MIDIHandler();
        F_midipollreq = 0;
        break;
      case Continue:
        handler.Continue_MIDIHandler();
        F_midipollreq = 0;
        break;
      case Stop:
        handler.Stop_MIDIHandler();
        F_midipollreq = 0;
        break;
      case SystemReset:
        handler.SystemReset_MIDIHandler();
        F_midipollreq = 0;
        break;
      case SysExStart:
        handler.SysExStart_MIDIHandler();
        F_midipollreq = 0;
        break;
      case SysExStop:
        handler.SysExStop_MIDIHandler();
        F_midipollreq = 0;
        break;
      case NoteOn:
        handler.NoteOn_MIDIHandler(MIDIMessage.status_data,
                                   MIDIMessage.databyte0,
                                   MIDIMessage.databyte1);
        F_midipollreq = 0;
        break;
      case NoteOff:
        handler.NoteOff_MIDIHandler(MIDIMessage.status_data,
                                    MIDIMessage.databyte0,
                                    MIDIMessage.databyte1);
        F_midipollreq = 0;
        break;
      case PolyPressure:
        handler.PolyPressure_MIDIHandler(MIDIMessage.status_data,
                                         MIDIMessage.databyte0,
                                         MIDIMessage.databyte1);
        F_midipollreq = 0;
        break;
      case ControlChange:
        handler.ControlChange_MIDIHandler(MIDIMessage.status_data,
                                          MIDIMessage.databyte0,
                                          MIDIMessage.databyte1);
        F_midipollreq = 0;
        break;
      case ProgramChange:
        handler.ProgramChange_MIDIHandler(MIDIMessage.status_data,
                                          MIDIMessage.databyte0);
        F_midipollreq = 0;
        break;
      case ChannelPressure:
        handler.ChannelPressure_MIDIHandler(MIDIMessage.status_data,
                                            MIDIMessage.databyte0);
        F_midipollreq = 0;
        break;
      case PitchBend:
        handler.PitchBend_MIDIHandler(MIDIMessage.status_data,
                                      MIDIMessage.databyte0,
                                      MIDIMessage.databyte1);
        F_midipollreq = 0;
        break;
      default:
        break;
      }
    }
  }
};

#endif /* _MIDI_HPP */
