/**
 * @file: i2c.hpp
 *
 * @date: 2020-06-28
 *
 * @author: Lukas Güldenstein
 */

#ifndef _I2C_HPP
#define _I2C_HPP

/* -------------------------------- includes -------------------------------- */
#include "config.hpp"
#include "stm32g4xx.h"
#include "stmlibxx/gpio.hpp"
#include "stmlibxx/interrupt.hpp"

/* --------------------------- custom definitions --------------------------- */

#define PRESC I2C_TIMINGR_PRESC_Pos
#define SCLDEL I2C_TIMINGR_SCLDEL_Pos
#define SDADEL I2C_TIMINGR_SDADEL_Pos
#define SCLH I2C_TIMINGR_SCLH_Pos
#define SCLL I2C_TIMINGR_SCLL_Pos

/* ---------------------------- class declaration --------------------------- */
class I2C : public stmlibxx::Interrupt<I2C, I2C3_EV_IRQn> {
  friend class stmlibxx::Interrupt<I2C, I2C3_EV_IRQn>;

public:
  I2C() = delete;
  I2C(const uint8_t &slaveAddress) : m_i2c_address{slaveAddress} {
    configure();
  }
  uint8_t read(const uint8_t &reg) {
    volatile uint8_t read_data;
    send(reg);                        // send register address
    I2C3->CR2 &= ~I2C_CR2_NBYTES_Msk; // reset N_Bytes
    I2C3->CR2 |= 1 << I2C_CR2_NBYTES_Pos;
    I2C3->CR2 |= I2C_CR2_RD_WRN; // read
    I2C3->CR2 |= I2C_CR2_START;
    // I2C3->TXDR = i2c_address;
    while (!(I2C3->ISR & I2C_ISR_RXNE)) {
      __NOP();
    }
    read_data = (0xFF & I2C3->RXDR);
    return read_data;
  }

protected:
  void send(const uint8_t &data1) {
    I2C3->CR2 &= ~I2C_CR2_RD_WRN;
    I2C3->CR2 &= ~I2C_CR2_NBYTES_Msk;     // reset N_Bytes
    I2C3->CR2 |= 1 << I2C_CR2_NBYTES_Pos; // set 2 bytes for transmission
    I2C3->CR2 &= ~I2C_CR2_RD_WRN;         // write
    I2C3->CR2 |= I2C_CR2_START;
    I2C3->TXDR = data1;
    while (!(I2C3->ISR & I2C_ISR_TXE)) {
      __NOP();
    }
  }
  void send(const uint8_t &data1, const uint8_t &data2) {
    I2C3->CR2 &= ~I2C_CR2_RD_WRN;         // write
    I2C3->CR2 &= ~I2C_CR2_NBYTES_Msk;     // reset N_Bytes
    I2C3->CR2 |= 2 << I2C_CR2_NBYTES_Pos; // set 2 bytes for transmission
    I2C3->CR2 |= I2C_CR2_START;
    I2C3->TXDR = data1;
    while (!(I2C3->ISR & I2C_ISR_TXE)) {
      __NOP();
    }
    I2C3->TXDR = data2;
    while (!(I2C3->ISR & I2C_ISR_TXE)) {
      __NOP();
    }
  }
  uint8_t m_i2c_address;

private:
  void configure() {
    /* --------------------- configure Pins PC8=SCL PC9=SDA
     * --------------------- */
    stmlibxx::GPIO_Pin_Config I2C_Pins(GPIO_C, {P8, P9});
    I2C_Pins.Mode = Alternate;
    I2C_Pins.OutputType = OpenDrain;
    I2C_Pins.AlternateFunction = AF8;
    I2C_Pins.configure();
    /* --------------------------- I2C3 configuration
     * --------------------------- */
    //! select clock signal 100kHz
    RCC->CCIPR |= 0b10 << RCC_CCIPR_I2C3SEL_Pos; // select HSI16 clock
    RCC->APB1ENR1 |= RCC_APB1ENR1_I2C3EN;        // enable clock

    I2C3->CR2 |= m_i2c_address << I2C_CR2_SADD_Pos; // set address
    const uint32_t timing = (3 << PRESC | 0x13 << SCLL | 0xF << SCLH |
                             0x2 << SDADEL | 0x4 << SCLDEL);
    I2C3->TIMINGR = timing;
    I2C3->CR1 |= I2C_CR1_TCIE;
    I2C3->CR1 |= I2C_CR1_NACKIE;
    I2C3->CR2 |= I2C_CR2_AUTOEND;
    I2C3->CR1 |= I2C_CR1_PE; // enable I2C
    NVIC_EnableIRQ(I2C3_EV_IRQn);
    NVIC_EnableIRQ(I2C3_ER_IRQn);
  }
  void IRQHandler(void) {
    while (1) {
      ;
    }
  }
};

#endif /* _I2C_HPP */
