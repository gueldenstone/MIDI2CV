/**
 * @file: sai.hpp
 *
 * @date: 2020-06-28
 *
 * @author: Lukas Güldenstein
 */

#ifndef _SAI_HPP
#define _SAI_HPP

/* -------------------------------- includes -------------------------------- */
#include <math.h>

#include "stm32g4xx.h"
#include "stmlibxx/gpio.hpp"
#include "stmlibxx/interrupt.hpp"

/* ---------------------------- class defintions ---------------------------- */

/** PCM1681 with SAI Interface using SAI1_A
 * --------------------------------------------------------------
 * Signal name  |   STM32 Signal |  STM32 Pin | PCM1681 Pin     |
 * --------------------------------------------------------------
 * Bit-CLock    |   SAI_SCK_A    |  PA8       | BCK             |
 * Frame-Sync   |   SAI_FS_A     |  PA9       | LRCK            |
 * Data         |   SAI_SD_A     |  PA10      | Data1           |
 * Master-Clock |   SAI_MCLK_A   |  PB8       | SCK             |
 * --------------------------------------------------------------
 */
class SAI : public stmlibxx::Interrupt<SAI, SAI1_IRQn> {
  friend class stmlibxx::Interrupt<SAI, SAI1_IRQn>;

protected:
  uint32_t m_samplerate, m_ovrsample;

private:
  void configure(void) {
    /* ------------------------- configure pins for SAI
     * ------------------------- */

    // alternate function mode
    GPIO_A.set_pin_config(P8, Alternate, AF14);
    GPIO_A.set_pin_config(P9, Alternate, AF14);
    GPIO_A.set_pin_config(P10, Alternate, AF14);
    GPIO_B.set_pin_config(P8, Alternate, AF14);

    /** the SAI needs a MCLK of samplerate * oversample to work
     * MCKDIV divides the SystemClock feeding into SAI
     * System Clock has to be configured with an (even) multiple of 12.288Mhz
     */
    SystemCoreClockUpdate();
    volatile int mckdiv;
    mckdiv = (int)round((float)SystemCoreClock / (m_samplerate * m_ovrsample));

    // configure SAI Interface
    RCC->CCIPR |= 0b01 << RCC_CCIPR_SAI1SEL_Pos; // set PPLQ as SAI1 Clock
    RCC->APB2ENR |= RCC_APB2ENR_SAI1EN;          // enable clock

    SAI1_Block_A->CR1 &= ~SAI_xCR1_MCKDIV_Msk; // reset MCKDIV
    SAI1_Block_A->CR1 |=
        mckdiv << SAI_xCR1_MCKDIV_Pos; // set MCKDIV for MCLK=12.288 MHz

    SAI1_Block_A->CR1 |= 0b110 << SAI_xCR1_DS_Pos; // 24 Bit Data
    SAI1_Block_A->FRCR |= 0xFF
                          << SAI_xFRCR_FRL_Pos; // Frame Lenght is 8*32bit-1=255
    SAI1_Block_A->FRCR |= SAI_xFRCR_FSPOL;
    SAI1_Block_A->FRCR |= (2) << SAI_xFRCR_FSALL_Pos; // set Sync to 24-bit on

    SAI1_Block_A->SLOTR = 0x00FF0780; // 8 Slots enabled with 32bit width.

    SAI1_Block_A->CR1 |= SAI_xCR1_MCKEN; // Enable Master Clock
    // wait for reset
    for (volatile uint32_t i = 0; i < 65535; i++) {
      ;
    }
  }

public:
  SAI() = delete;
  SAI(const uint32_t &smprate, const uint32_t &ovrsmpl)
      : m_samplerate{smprate}, m_ovrsample{ovrsmpl} {
    configure();
  }
  void start(void) {
    SAI1_Block_A->CR1 |= SAI_xCR1_SAIEN; // Enable SAI Block A
  }
  void enable_interrupt(void) {
    // enable interrupt for SAI1
    NVIC_EnableIRQ(SAI1_IRQn);
  }
  void disable_interrupt(void) { NVIC_DisableIRQ(SAI1_IRQn); }

private:
  void IRQHandler(void) {
    while (1) {
    }
    // clear over/underrun
    SAI1_Block_A->CLRFR |= SAI_xCLRFR_COVRUDR;
  }
};

#endif /* _SAI_HPP */
