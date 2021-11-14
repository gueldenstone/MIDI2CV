/**
 * @file: adc.hpp
 *
 * @date: 2020-07-01
 *
 * @author: Lukas Güldenstein
 */

#ifndef _ADC_HPP
#define _ADC_HPP

/* -------------------------------- includes -------------------------------- */
#include "dma.hpp"
#include "stm32g4xx.h"
#include "stmlibxx/interrupt.hpp"

class ADC1_2 : public stmlibxx::Interrupt<ADC1_2, ADC1_2_IRQn>,
               public DMA1_CH2 {
  friend class stmlibxx::Interrupt<ADC1_2, ADC1_2_IRQn>;

public:
  ADC1_2() { configure(); }
  void start(void) {
    ADC1->CR |= ADC_CR_ADSTART; // start conversion
  }
  uint16_t get(void) {
    value = ADC_Buf;
    return ADC_Buf;
  }

  uint16_t value;

private:
  void configure(void) {
    // PLL P Clock @ approx 60Mhz
    RCC->CCIPR |= 0b01 << RCC_CCIPR_ADC12SEL_Pos; // select PLL P Clock
    RCC->AHB2ENR |= RCC_AHB2ENR_ADC12EN;          // enable clock
    ADC12_COMMON->CCR |= ADC_CCR_DMACFG;          // DMA config in circular

    // using GPIO PA0
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;          // enable Clock for GPIOA
    GPIOA->MODER |= 0b11 << GPIO_MODER_MODE0_Pos; // set analog mode

    /* ADC1 einschalten */
    ADC1->CR &= ~(0b11 << 28); // Voltage regulator: Intermediate state
    ADC1->CR |= 0b01 << 28;    // Voltage regulator: Enabled
    for (volatile int x = 0; x < 65535; x++) {
      __NOP();
    }                         // Warte 10 us
    ADC1->CR |= ADC_CR_ADCAL; // Kalibriere den ADC
    while ((ADC1->CR & ADC_CR_ADCAL) != 0) {
      __NOP();
    } // Warte bis Kalibrierung abgeschlossen

    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    __NOP();
    ADC1->ISR |= ADC_ISR_ADRDY;
    ADC1->CR |= ADC_CR_ADEN; // Enable ADC
    while ((ADC1->ISR & ADC_ISR_ADRDY) == 0) {
      __NOP();
    } // Warte bis ADC bereit

    /* ADC1 Config */
    // ADC1->CFGR |= ADC_CFGR_AUTDLY;               // auto delay
    ADC1->CFGR |= ADC_CFGR_CONT;   // continous mode
    ADC1->CFGR |= ADC_CFGR_DMACFG; // DMA config in circular
    ADC1->CFGR |= ADC_CFGR_DMAEN;  // enable DMA
    ADC1->SMPR1 |=
        0b111 << ADC_SMPR1_SMP0_Pos; // Channel 1: Sample time 247.5 cycles
    ADC1->SQR1 |=
        0x1
        << ADC_SQR1_SQ1_Pos; // 1st conv. in regular sequence: Channel 1 (PA0)

    ADC1->IER |= ADC_IER_EOCIE; // Interrupt enable for EOS (Stufe 2)
    // NVIC_EnableIRQ(ADC1_2_IRQn);
  }
  void IRQHandler() { ADC1->ISR |= ADC_ISR_EOC; }
};

#endif /* _ADC_HPP */
