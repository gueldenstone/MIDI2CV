/**
 * @file: main.cpp
 *
 * @date: 2020-06-22
 *
 * @author: Lukas Güldenstein
 *
 * @brief: MIDI2CV Interface for Modular Synth and Digitakt
 */

/* -------------------------------- includes -------------------------------- */

#include "board.hpp"
#include "config.hpp"
#include "midi2cv.hpp"
#include "stmlibxx/gpio.hpp"
#define A_NORM 8388607U
int main() {
  Board::initialize();

/** Enable Logging for Debugging */
#ifdef LOGGING
  UART log;
  log.start();
#endif
  __disable_irq();
  MIDI2CV converter;
  __enable_irq();
  while (1) {

    converter.convert();
    __WFI();
  }
}
