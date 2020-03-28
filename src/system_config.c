/*
 * system_init.c
 *
 *  Created on: 07.02.2020
 *      Author: lukas
 */
#include "stm32f3xx.h"
#include "system_init.h"

void system_config(void){
	/* PLL Config */
	RCC->CR &= ~(RCC_CR_PLLON); 									// Deactivate PLL
	while(RCC->CR & RCC_CR_PLLRDY){}								// Wait for PLL to stop
	RCC->CR |= RCC_CR_HSEON;
	while(!(RCC->CR & RCC_CR_HSERDY)){}								// check if HSE is ready
	RCC->CFGR |= RCC_CFGR_PLLSRC_HSE_PREDIV | RCC_CFGR_PLLMUL9;		// Set PLL to input HSI/2, Multiplicator = 16 => HCLK=64MHz
	RCC->CR |= RCC_CR_PLLON; 										// Activate PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)); 								// Wait for PLL to lock

	/*FLASH wait states */
	FLASH->ACR &= ~(FLASH_ACR_LATENCY_Msk);							// Reset Flash Wait states
	FLASH->ACR |= 0b010 << FLASH_ACR_LATENCY_Pos;					// Set Flash wait states to 2

	/*SysClock anpassen */
	RCC->CFGR |= RCC_CFGR_PPRE1_DIV2; 								// PreDiv for ABP1 /2 (ABP1 36MHz max)
	RCC->CFGR |= RCC_CFGR_SW_PLL;									// Set PLL as Sysclock
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){} 		// Wait for switch to PLL as clock source

  	SystemCoreClockUpdate();

  	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; //Clock to SYSCFG
}
