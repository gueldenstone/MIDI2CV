/**
 * @file: dma.hpp
 *
 * @date: 2020-06-28
 *
 * @author: Lukas Güldenstein
 */

#ifndef _DMA_HPP
#define _DMA_HPP

/* -------------------------------- includes -------------------------------- */
#include "stm32g4xx.h"
#include "stmlibxx/interrupt.hpp"
/* --------------------------- Channel definitions -------------------------- */
#define CHANNEL1 0U
#define CHANNEL2 1U
#define CHANNEL3 2U
#define CHANNEL4 3U
#define CHANNEL5 4U
#define CHANNEL6 5U
#define CHANNEL7 6U
#define CHANNEL8 7U

/* ---------------------------- class definitions --------------------------- */
class DMA1_CH1 : public stmlibxx::Interrupt<DMA1_CH1, DMA1_Channel1_IRQn> {
  friend class stmlibxx::Interrupt<DMA1_CH1, DMA1_Channel1_IRQn>;

public:
  DMA1_CH1() { configure(); }

protected:
  int DMATx_Buf[8] = {};

private:
  void configure(void) {
    // DMA
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMAMUX1EN;
    DMA1_Channel1->CPAR =
        (uint32_t)(&(SAI1_Block_A->DR));             // peripheral is SAI1A FIFO
    DMA1_Channel1->CMAR = (uint32_t)(DMATx_Buf);     // memory is SAI1A FIFO
    DMA1_Channel1->CNDTR = 8;                        // 8 word FIFO
    DMA1_Channel1->CCR |= 0b10 << DMA_CCR_MSIZE_Pos; // 32-bit Memory
    DMA1_Channel1->CCR |= 0b10 << DMA_CCR_PSIZE_Pos; // 32-bit Peripheral
    DMA1_Channel1->CCR |= DMA_CCR_MINC;              // increment memory
    DMA1_Channel1->CCR |= DMA_CCR_CIRC;              // increment circular
    DMA1_Channel1->CCR |= DMA_CCR_DIR;               // read from memory
    DMA1_Channel1->CCR |= DMA_CCR_TCIE; // transfer complete interrupt
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    // DMA MUX
    DMAMUX1_Channel0->CCR = 108
                            << DMAMUX_CxCR_DMAREQ_ID_Pos; // Channel 108 is SAI1
    DMA1_Channel1->CCR |= DMA_CCR_EN;    // Enable DMA Channel 1
    SAI1_Block_A->CR1 |= SAI_xCR1_DMAEN; // enable DMA on SAI
  }
  void IRQHandler(void) { DMA1->IFCR |= DMA_ISR_GIF1; }
};

class DMA1_CH2 : stmlibxx::Interrupt<DMA1_CH2, DMA1_Channel2_IRQn> {
  friend class stmlibxx::Interrupt<DMA1_CH2, DMA1_Channel2_IRQn>;

public:
  DMA1_CH2() { configure(&(ADC1->DR), &ADC_Buf); }

protected:
  uint16_t ADC_Buf = 0;

private:
  void configure(const __IO uint32_t *peripheral, const uint16_t *memory) {
    // DMA
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->AHB1ENR |= RCC_AHB1ENR_DMAMUX1EN;
    DMA1_Channel2->CPAR = (uint32_t)(peripheral);    // peripheral is SAI1A FIFO
    DMA1_Channel2->CMAR = (uint32_t)(memory);        // memory is SAI1A FIFO
    DMA1_Channel2->CNDTR = 1;                        // 1 word FIFO
    DMA1_Channel2->CCR |= 0b01 << DMA_CCR_MSIZE_Pos; // 16-bit Memory
    DMA1_Channel2->CCR |= 0b01 << DMA_CCR_PSIZE_Pos; // 16-bit Peripheral
    // DMA1_Channel2->CCR |= DMA_CCR_MINC;               // increment memory
    DMA1_Channel2->CCR |= DMA_CCR_CIRC; // increment circular
    DMA1_Channel2->CCR &= ~DMA_CCR_DIR; // read from peripheral
    DMA1_Channel2->CCR |= DMA_CCR_TCIE; // transfer complete interrupt
    // NVIC_EnableIRQ(DMA1_Channel2_IRQn);

    // DMA MUX
    DMAMUX1_Channel1->CCR =
        5
        << DMAMUX_CxCR_DMAREQ_ID_Pos; // Channel 5 is ADC1 //! naming is one off
    DMA1_Channel2->CCR |= DMA_CCR_EN; // Enable DMA Channel 2
  }
  void IRQHandler(void) {
    while (1) {
      __NOP();
    }
  }
};

#endif /* _DMA_HPP */
