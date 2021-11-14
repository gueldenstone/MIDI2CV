/**
 * @file: pcm1681.hpp
 *
 * @date: 2020-06-26
 *
 * @author: Lukas Güldenstein
 */

#ifndef _PCM1681_HPP
#define _PCM1681_HPP

/* -------------------------------- includes -------------------------------- */
#include "dma.hpp"
#include "i2c.hpp"
#include "sai.hpp"
#include "stmlibxx/interrupt.hpp"

#define PCM1681_STATUS_OK 0x7FFF
/* -------------------------- register definitions -------------------------- */
typedef struct {
  __IO uint8_t RSV1;
  __IO uint8_t AT1;
  __IO uint8_t AT2;
  __IO uint8_t AT3;
  __IO uint8_t AT4;
  __IO uint8_t AT5;
  __IO uint8_t AT6;
  __IO uint8_t MUT;
  __IO uint8_t DAC_OP;
  __IO uint8_t FMT;
  __IO uint8_t CR1;
  __IO uint8_t REV;
  __IO uint8_t CR2;
  __IO uint8_t CR3;
  __IO uint8_t ZRO;
  __IO uint8_t RSV2;
  __IO uint8_t AT7;
  __IO uint8_t AT8;
  __IO uint8_t RSV3[2];
} PCM1681_TypeDef;

// Register addresses
#define AT1_address 0x1
#define AT2_address 0x2
#define AT3_address 0x3
#define AT4_address 0x4
#define AT5_address 0x5
#define AT6_address 0x6
#define MUT_address 0x7
#define DAC_OP_address 0x8
#define FMT_address 0x9
#define CR1_address 0xA
#define REV_address 0xB
#define CR2_address 0xC
#define CR3_address 0xD
#define ZRO_address 0xE
#define AT7_address 0x10
#define AT8_address 0x11

// default state
#define ATx_default 0xFF
#define MUT_default 0x0
#define DAC_OP_default 0x0
#define FMT_default 0x3
#define CR1_default 0x0
#define REV_default 0xFF
#define CR2_default 0xF
#define CR3_default 0x0

// mutes
#define PCM1681_MUT_1 0b00000001
#define PCM1681_MUT_2 0b00000010
#define PCM1681_MUT_3 0b00000100
#define PCM1681_MUT_4 0b00001000
#define PCM1681_MUT_5 0b00010000
#define PCM1681_MUT_6 0b00100000
#define PCM1681_MUT_7 0b01000000
#define PCM1681_MUT_8 0b10000000

// DAC operation
#define PCM1681_DAC1_EN 0b00000001
#define PCM1681_DAC2_EN 0b00000010
#define PCM1681_DAC3_EN 0b00000100
#define PCM1681_DAC4_EN 0b00001000
#define PCM1681_DAC5_EN 0b00010000
#define PCM1681_DAC6_EN 0b00100000
#define PCM1681_DAC7_EN 0b01000000
#define PCM1681_DAC8_EN 0b10000000

// format register
#define PCM1681_FMT_RJ_24 0b00000000
#define PCM1681_FMT_RJ_16 0b00000011
#define PCM1681_FMT_I2S_16_24 0b00000100
#define PCM1681_FMT_LJ_16_24 0b00000101
#define PCM1681_FMT_I2S_TDM_24 0b00000110
#define PCM1681_FMT_LJ_TDM_24 0b00000111
#define PCM1681_FMT_I2S_DSP_24 0b00001000
#define PCM1681_FMT_LJ_DSP_24 0b00001001
#define PCM1681_FMT_FLT 0b00100000

// CR1 Reset, Zero Flag Polarity, Output Phase Sample Frequency, De-Emphasis
#define PCM1681_CR1_SRST 0b10000000
#define PCM1681_CR1_ZREV 0b01000000
#define PCM1681_CR1_DREV 0b001000000
#define PCM1681_CR1_DMF_44k1 0b000000000
#define PCM1681_CR1_DMF_48k 0b000001000
#define PCM1681_CR1_DMF_32k 0b000010000
#define PCM1681_CR1_DMC 0b00000001

// REV Output Phase per Channel
#define PCM1681_REV_DAC1 0b00000001
#define PCM1681_REV_DAC2 0b00000010
#define PCM1681_REV_DAC3 0b00000100
#define PCM1681_REV_DAC4 0b00001000
#define PCM1681_REV_DAC5 0b00010000
#define PCM1681_REV_DAC6 0b00100000
#define PCM1681_REV_DAC7 0b01000000
#define PCM1681_REV_DAC8 0b10000000

// CR2 Oversampling Filter Roll-Off per DATA
#define PCM1681_CR2_OVER 0b10000000
#define PCM1681_CR2_FLT1 0b00000001
#define PCM1681_CR2_FLT2 0b00000010
#define PCM1681_CR2_FLT3 0b00000100
#define PCM1681_CR2_FLT4 0b00001000

// CR3 Digital Attenuation Mode, Zero-Flag Channel-Combination
#define PCM1681_CR3_DAMS 0b10000000
#define PCM1681_CR3_AZRO_Pos 5U
#define PCM1681_CR3_AZRO_Msk 0b01100000
#define PCM1681_CR3_AZRO_A 0b00
#define PCM1681_CR3_AZRO_B 0b01
#define PCM1681_CR3_AZRO_C 0b10
#define PCM1681_CR3_AZRO_D 0b11

// ZRO Zero detection Status
#define PCM1681_ZRO_ZERO1 0b00000001
#define PCM1681_ZRO_ZERO2 0b00000010
#define PCM1681_ZRO_ZERO3 0b00000100
#define PCM1681_ZRO_ZERO4 0b00001000
#define PCM1681_ZRO_ZERO5 0b00010000
#define PCM1681_ZRO_ZERO6 0b00100000
#define PCM1681_ZRO_ZERO7 0b01000000
#define PCM1681_ZRO_ZERO8 0b10000000

/* ---------------------------- class definitions --------------------------- */

class PCM1681 : public SAI, public I2C, public DMA1_CH1 {
public:
  PCM1681() = delete;
  PCM1681(uint32_t smprate, uint32_t ovrsmp, uint8_t address)
      : SAI(smprate, ovrsmp), I2C(address) {
    register_config.AT1 = ATx_default;
    register_config.AT2 = ATx_default;
    register_config.AT3 = ATx_default;
    register_config.AT4 = ATx_default;
    register_config.AT5 = ATx_default;
    register_config.AT6 = ATx_default;
    register_config.AT7 = ATx_default;
    register_config.AT8 = ATx_default;
    register_config.MUT = MUT_default;
    register_config.DAC_OP = DAC_OP_default;
    register_config.FMT = FMT_default;
    register_config.CR1 = CR1_default;
    register_config.CR2 = CR2_default;
    register_config.CR3 = CR3_default;
    register_config.REV = REV_default;
  }

  void set_channel(uint8_t channel, int value) {
    // comment
    this->DMATx_Buf[channel] = value;
  }

  void set_all_channels(int *channel_data) {
    for (int i = 0; i > 7; i++) {
      DMA1_CH1::DMATx_Buf[i] = channel_data[i];
    }
  }

  uint16_t configure(void) {
    int status;
    register_config.FMT = PCM1681_FMT_LJ_TDM_24;
    send_config();
    status = check_config();
    for (int i = 0; i <= 7; i++) {
      DMATx_Buf[i] = -offset;
    }
    return status;
  }

public:
  const uint32_t max_amp = 8388607U;
  const float max_amp_V = 6.27f;
  const float min_amp_V = -5.95f;
  const float resolution = 2 * max_amp / (max_amp_V - min_amp_V);
  const float offset_V = 0.1490f;
  const float offset = offset_V * resolution;

private:
  void send_config(void) {
    set_register(AT1_address, register_config.AT1);
    set_register(AT2_address, register_config.AT2);
    set_register(AT3_address, register_config.AT3);
    set_register(AT4_address, register_config.AT4);
    set_register(AT5_address, register_config.AT5);
    set_register(AT6_address, register_config.AT6);
    set_register(MUT_address, register_config.MUT);
    set_register(DAC_OP_address, register_config.DAC_OP);
    set_register(FMT_address, register_config.FMT);
    set_register(CR1_address, register_config.CR1);
    set_register(REV_address, register_config.REV);
    set_register(CR2_address, register_config.CR2);
    set_register(CR3_address, register_config.CR3);
    set_register(AT7_address, register_config.AT7);
    set_register(AT8_address, register_config.AT8);
  }
  uint16_t check_config(void) {
    uint16_t status = 0;
    if (read(AT1_address) == register_config.AT1)
      status |= 0x1;
    else
      status &= ~0x1;

    if (read(AT2_address) == register_config.AT2)
      status |= 0x2;
    else
      status &= ~0x2;

    if (read(AT3_address) == register_config.AT3)
      status |= 0x4;
    else
      status &= ~0x4;

    if (read(AT4_address) == register_config.AT4)
      status |= 0x8;
    else
      status &= ~0x8;

    if (read(AT5_address) == register_config.AT5)
      status |= 0x10;
    else
      status &= ~0x10;

    if (read(AT6_address) == register_config.AT6)
      status |= 0x20;
    else
      status &= ~0x20;

    if (read(MUT_address) == register_config.MUT)
      status |= 0x40;
    else
      status &= ~0x40;

    if (read(DAC_OP_address) == register_config.DAC_OP)
      status |= 0x80;
    else
      status &= ~0x80;

    if (read(FMT_address) == register_config.FMT)
      status |= 0x100;
    else
      status &= ~0x100;

    if (read(CR1_address) == register_config.CR1)
      status |= 0x200;
    else
      status &= ~0x200;

    if (read(REV_address) == register_config.REV)
      status |= 0x400;
    else
      status &= ~0x400;

    if (read(CR2_address) == register_config.CR2)
      status |= 0x800;
    else
      status &= ~0x800;

    if (read(CR3_address) == register_config.CR3)
      status |= 0x1000;
    else
      status &= ~0x1000;

    if (read(AT7_address) == register_config.AT7)
      status |= 0x2000;
    else
      status &= ~0x2000;

    if (read(AT8_address) == register_config.AT8)
      status |= 0x4000;
    else
      status &= ~0x4000;

    return status;
  }
  void set_register(uint8_t reg, uint8_t data) { I2C::send(reg, data); }
  PCM1681_TypeDef register_config;
};

#endif /* _PCM1681_HPP */
