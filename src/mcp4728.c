/*
 * mcp4728.c
 *
 *  Created on: 23.02.2020
 *      Author: lukas
 */
#include "stm32f3xx.h"


volatile uint8_t newSlaveAddress, currentDeviceNum, i2cCylce, i2cByte;
volatile uint8_t i2cData[3];

void MCP4728_Init(void){

	/* DAC config (I2C, PB8, PB9) */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN;
	GPIOB->MODER &= ~0xF0000;									// reset PB8, PB9
	GPIOB->MODER |= 0b10 << GPIO_MODER_MODER8_Pos;				// alternate function for I2C, SCL (PB8)
	GPIOB->MODER |= 0b10 << GPIO_MODER_MODER9_Pos;				// alternate function for I2C, SDA (PB9)
	GPIOB->AFR[1] |= 0b0100<< GPIO_AFRH_AFRH0_Pos;				// AF4 I2C SCL (PB8)
	GPIOB->AFR[1] |= 0b0100<< GPIO_AFRH_AFRH1_Pos;				// AF4 I2C SDA (PB9)
	GPIOB->OTYPER |= GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9;		// PB8, PB9 open-drain config
//	GPIOB->PUPDR |= GPIOx_PUPDR_PU << GPIO_PUPDR_PUPDR8_Pos;	// pull-up on PB8
//	GPIOB->PUPDR |= GPIOx_PUPDR_PU << GPIO_PUPDR_PUPDR9_Pos;	// pull-up on PB9

	RCC->APB1ENR |= RCC_APB1ENR_I2C1EN;		// clock for I2C1
	I2C1->TIMINGR = 0x10808DD3;				// timing by CubeMX with 72Mhz
	I2C1->CR1 |= I2C_CR1_TXDMAEN; 			// DMA request for TX
	I2C1->CR2 |= 3 << I2C_CR2_NBYTES_Pos; 	// sending 3 bytes to DAC
	I2C1->CR2 |= I2C_CR2_AUTOEND;			// send automatic stop condition after 3 bytes

	// DMA
	DMA1_Channel6->CCR |= DMA_CCR_MINC | DMA_CCR_CIRC ;		// memory increment and circular mode
	DMA1_Channel6->CCR |= DMA_CCR_DIR;						// direction to peripherial
	DMA1_Channel6->CPAR = (uint32_t)(&(I2C1->TXDR));		// peripherial adress is TXDR
	DMA1_Channel6->CMAR = (uint32_t)&i2cData;				// data comes from uint8_t array with 3 elements
	DMA1_Channel6->CCR |= DMA_CCR_TCIE;						// enable interrupt on transfer
	NVIC_EnableIRQ(DMA1_Channel6_IRQn);

	DMA1_Channel1->CCR |= DMA_CCR_EN;		// enable DMA Channel 6
	I2C1->CR1 |= I2C_CR1_PE;				// enable I2C
}


void setSlaveAddress(uint8_t address, uint8_t deviceNum){

	/* configure I2C pins for bit-banging */
	RCC->AHBENR |= RCC_AHBENR_GPIOBEN | RCC_AHBENR_GPIOAEN;
	GPIOB->MODER &= ~0xF0000;									// reset PB8, PB9
	GPIOB->MODER |= 0b01 << GPIO_MODER_MODER8_Pos;				// output SCL (PB8)
	GPIOB->MODER |= 0b01 << GPIO_MODER_MODER9_Pos;				// output SDA (PB9)
	GPIOB->OTYPER |= GPIO_OTYPER_OT_8 | GPIO_OTYPER_OT_9;		// PB8, PB9 open-drain config
	GPIOA->MODER &= ~0xF0000;									// reset PA8, PA9
	GPIOB->MODER &= ~0xFF000000;								// reset PB12-PB15
	GPIOA->MODER |= 0x50000;									// set pins PA8, PA9 to output for RDY/BSY
	GPIOB->MODER |= 0x55000000;									// set pins PB12 - PB15 to output for LDAC and RDY/BSY

	RCC->APB2ENR |= RCC_APB2ENR_TIM16EN;		// use TIM16 for bit-banging I2C
	TIM16->PSC = SystemCoreClock/1000000;		// Timer in µs
	TIM16->ARR = 10;							// update every 10µs
	TIM16->DIER |= TIM_DIER_UIE;				// enable interrupt on update
	NVIC_EnableIRQ(TIM16_IRQn);
	newSlaveAddress = address;
	currentDeviceNum = deviceNum;
	i2cCylce = 0;
	TIM16->CR1 |= TIM_CR1_CEN;					// enable TIM16
}

void TIM1_UP_TIM16_IRQHandler(void){
	switch(currentDeviceNum){
	case 1:
		if((i2cByte >> (7 - i2cCylce)) & 0b1)
			GPIOB->BSRR |= GPIO_BSRR_BS_9;
		break;
	case 2:

		break;
	case 4:

		break;
	}

}
