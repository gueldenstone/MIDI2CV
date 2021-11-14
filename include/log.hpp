/**
 * @file: log.hpp
 *
 * @date: 2020-06-28
 *
 * @author: Lukas Güldenstein
 */

#ifndef _LOG_HPP
#define _LOG_HPP

/* -------------------------------- includes -------------------------------- */
#include "stm32g4xx.h"
#include "stmlibxx/gpio.hpp"

// #define LOGGING

// #ifdef LOGGING
// #define LOG(TEXT) printf_(TEXT);
// #else
// #define LOG(x)
// #endif

// extern "C" {
// int __io_putchar(int ch) {
//   if (ch == '\n') __io_putchar('\r');
//   while ((LPUART1->ISR & USART_ISR_TXE) == 0)
//     ;
//   LPUART1->TDR = ch;
//   return ch;
// }
// }
// extern "C" {
// void _putchar(char character) {
//   if (character == '\n') __io_putchar('\r');
//   while ((LPUART1->ISR & USART_ISR_TXE) == 0)
//     ;
//   LPUART1->TDR = character;
// }
// }
/* ----------------------------- class definiton ---------------------------- */
class UART {
public:
  UART() {
    GPIO_A.set_pin_config(P2, Alternate, AF12);
    GPIO_A.set_pin_config(P3, Alternate, AF12);

    // configure LPUART
    uint32_t lpuartdiv, baudrate = 9600, presc, lpuart_ker_ck;

    RCC->CCIPR |= 0b10 << RCC_CCIPR_LPUART1SEL_Pos; // HSI16 is UART input
    lpuart_ker_ck = 16000000;
    RCC->APB1ENR2 |= RCC_APB1ENR2_LPUART1EN; // enable clocking
    LPUART1->PRESC = 0b0011;                 // prescaler = 6
    presc = 6;
    lpuartdiv =
        (256 * lpuart_ker_ck) / (baudrate * presc); // calculate BRR register
    LPUART1->BRR = lpuartdiv;                       // set BRR Register
    LPUART1->CR1 |= USART_CR1_UE;                   // enable LPUART
  }
  void start(void) {
    LPUART1->CR1 |= USART_CR1_TE; // enable UART transmitter
  }
};
#endif /* _LOG_HPP */
