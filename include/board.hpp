/**
 * @file: board.hpp
 *
 * @date: 2020-06-26
 *
 * @author: Lukas Güldenstein
 */

#ifndef _BOARD_HPP
#define _BOARD_HPP

/* -------------------------------- includes -------------------------------- */
#include "stm32g4xx.h"

class Board {
public:
  Board();
  static void initialize(void) {
    /* PLL Config */
    RCC->CR &= ~(RCC_CR_PLLON); // Deactivate PLL
    while (RCC->CR & RCC_CR_PLLRDY) {
      __NOP();
    }                        // Wait for PLL to stop
    RCC->CR |= RCC_CR_HSEON; // activate HSE
    while (!(RCC->CR & RCC_CR_HSERDY)) {
      __NOP();
    } // check if HSE is ready

    // configure PLL
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_HSE;          // set HSE as PLL Input
    RCC->PLLCFGR |= (7 - 1) << RCC_PLLCFGR_PLLM_Pos; // M=7
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLN_Msk;           // reset N
    RCC->PLLCFGR |= 86 << RCC_PLLCFGR_PLLN_Pos;      // N=86
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLR_Msk;           // R=2
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLP_Msk;           // reset P
    RCC->PLLCFGR |= 5 << RCC_PLLCFGR_PLLP_Pos;       // P=5

    RCC->CR |= RCC_CR_PLLON; // Activate PLL
    while (!(RCC->CR & RCC_CR_PLLRDY)) {
      __NOP(); // Wait for PLL to lock
    }

    // enable PLL Outputs
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLREN; // enable R-Output
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // enable Q-Output
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLPEN; // enable P-Output

    /* FLASH wait states */
    FLASH->ACR &= ~(FLASH_ACR_LATENCY_Msk);        // Reset Flash Wait states
    FLASH->ACR |= 0b1000 << FLASH_ACR_LATENCY_Pos; // Set Flash wait states to 4

    /* SysClock anpassen */
    RCC->CFGR &= ~RCC_CFGR_HPRE_Msk;  // reset AHB prescaler to 1
    RCC->CFGR &= ~RCC_CFGR_PPRE1_Msk; // reset APB1 prescaler to 1
    RCC->CFGR &= ~RCC_CFGR_PPRE2_Msk; // reset APB2 prescaler to 1

    RCC->CFGR |= RCC_CFGR_SW_PLL; // Set PLL as Sysclock
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) {
      __NOP(); // Wait for switch to PLL as clock source
    }

    SystemCoreClockUpdate();
  };
};

#endif /* _BOARD_HPP */
