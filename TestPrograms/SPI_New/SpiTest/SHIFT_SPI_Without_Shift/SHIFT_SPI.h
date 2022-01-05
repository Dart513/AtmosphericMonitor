/**
 * @file SHIFT_SPI.h
 * @author Darton Li
 * @version 0.1
 * @date 2021-12-19
 *
 * This header file makes it so you can use SPI on a shift register
 * 
 */

#ifndef SHIFT_SPI_INCLUDED
#define SHIFT_SPI_INCLUDED

#include <Arduino.h>
#include "esp8266_peri.h"

//Virtual (output only) pins
//(The neighbourhood of above 364 set of addresses are yet undefined).
#define V0 ESP8266_REG(0x368);
#define V1 ESP8266_REG(0x372);
#define V2 ESP8266_REG(0x376);
#define V3 ESP8266_REG(0x37A);
#define V4 ESP8266_REG(0x37E);
#define V5 ESP8266_REG(0x382);
#define V6 ESP8266_REG(0x386);
#define V7 ESP8266_REG(0x38A);
#define V8 ESP8266_REG(0x38E);

#define P16 16
#define P17 17
#define P18 18
#define P19 19
#define P20 20
#define P21 21
#define P22 22
#define P23 23


// This defines are not representing the real Divider of the ESP8266
// the Defines match to an AVR Arduino on 16MHz for better compatibility
#define SPI_CLOCK_DIV2 		0.125//0x00101001 //8 MHz
#define SPI_CLOCK_DIV4 		0.250 // 0x00241001 //4 MHz
#define SPI_CLOCK_DIV8 		0.50 // 0x004c1001 //2 MHz
#define SPI_CLOCK_DIV16 	1 //0x009c1001 //1 MHz
#define SPI_CLOCK_DIV32 	2  //0x013c1001 //500 KHz
#define SPI_CLOCK_DIV64 	4   //0x027c1001 //250 KHz
#define SPI_CLOCK_DIV128 	8    //0x04fc1001 //125 KHz

const uint8_t SPI_MODE0 = 0x00; ///<  CPOL: 0  CPHA: 0
const uint8_t SPI_MODE1 = 0x01; ///<  CPOL: 0  CPHA: 1
const uint8_t SPI_MODE2 = 0x10; ///<  CPOL: 1  CPHA: 0
const uint8_t SPI_MODE3 = 0x11; ///<  CPOL: 1  CPHA: 1


class SPISettings {
public:
  SPISettings() :_clock(1000000), _bitOrder(LSBFIRST), _dataMode(SPI_MODE0){}
  SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) :_clock(clock), _bitOrder(bitOrder), _dataMode(dataMode){}
  uint32_t _clock;
  uint8_t  _bitOrder;
  uint8_t  _dataMode;
};

class SPIClass {
public:
  SPIClass();
  void begin();
  void end();
  bool pins(int8_t ser, int8_t rclk, int8_t srclk, int8_t miso, int8_t mosi);
  void setHwCs(bool use);
  void setBitOrder(uint8_t bitOrder);  
  void setDataMode(uint8_t dataMode);
  void setFrequency(uint32_t freq);
  void setClockDivider(uint32_t clockDiv);
  void beginTransaction(SPISettings settings);
  uint8_t transfer(uint8_t data);
  uint16_t transfer16(uint16_t data);
  void transfer(void *buf, uint16_t count);
  void write(uint8_t data);
  void write16(uint16_t data);
  void write16(uint16_t data, bool msb);
  void write32(uint32_t data);
  void write32(uint32_t data, bool msb);
  void writeBytes(const uint8_t * data, uint32_t size);
  void writePattern(const uint8_t * data, uint8_t size, uint32_t repeat);
  void transferBytes(const uint8_t * out, uint8_t * in, uint32_t size);
  void endTransaction(void);
private:
  bool useHwCs;
  uint8_t pinSet, dataMode;
  uint8_t ser, rclk, srclk, miso, mosi, sck;
  double_t clockDiv;
  uint8_t bitOrder;
  uint64_t nextCycleTime;
  void writeBytes_(const uint8_t * data, uint8_t size);
  void transferBytes_(const uint8_t * out, uint8_t * in, uint8_t size);
  void transferBytesAligned_(const uint8_t * out, uint8_t * in, uint8_t size);
  inline void setDataBits(uint16_t bits);
};

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
extern SPIClass SPI;
#endif

#endif