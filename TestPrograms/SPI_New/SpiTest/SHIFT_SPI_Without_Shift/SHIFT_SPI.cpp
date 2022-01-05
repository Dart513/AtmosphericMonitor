/**
 * @file SHIFT_SPI.cpp
 * 
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-19
 * 
 * @copyright Copyright (c) 2021
 * 
 *//* 
 SPI.cpp - SPI library for esp8266

 Copyright (c) 2015 Hristo Gochkov. All rights reserved.
 This file is part of the esp8266 core for Arduino environment.
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "HardwareSerial.h"

#include "SHIFT_SPI.h"



typedef union {
        uint32_t regValue;
        struct {
                unsigned regL :6;
                unsigned regH :6;
                unsigned regN :6;
                unsigned regPre :13;
                unsigned regEQU :1;
        };
} spiClk_t;

SPIClass::SPIClass() {}


//TODO: PINS SHOULD ALSO INCLUDE VIRTUAL PINS
bool SPIClass::pins(int8_t ser, int8_t rclk, int8_t srclk, int8_t miso, int8_t mosi)
{
    (*this).ser = ser;
    (*this).rclk = rclk;
    (*this).srclk = srclk;
    (*this).miso = miso;
    (*this).mosi = mosi;
    (*this).sck = sck;

    return true;
}

void SPIClass::begin() {

    pinMode(ser, OUTPUT);
    pinMode(rclk, OUTPUT);
    pinMode(srclk, OUTPUT);
    pinMode(miso, INPUT);
    
    setFrequency(1000000); ///< 1MHz
}

void SPIClass::end() {
    pinMode(ser, INPUT);
    pinMode(rclk, INPUT);
    pinMode(srclk, INPUT);
    pinMode(miso, INPUT);
}

void SPIClass::setHwCs(bool use) {
    //WE DONT USE THIS.
}

void SPIClass::beginTransaction(SPISettings settings) {
    setFrequency(settings._clock);
    setBitOrder(settings._bitOrder);
    setDataMode(settings._dataMode);
}

void SPIClass::endTransaction() {
    //lmao we don't do shit with this
}

void SPIClass::setDataMode(uint8_t dataMode) {
    this->dataMode = dataMode;
}

void SPIClass::setBitOrder(uint8_t bitOrder) {
    this->bitOrder = bitOrder;
}

//match freq to closest possible clockdiv
void SPIClass::setFrequency(uint32_t freq) {
    switch (freq) {
        case 8000000:
            clockDiv = SPI_CLOCK_DIV2;
            break;

        case 4000000:
            clockDiv = SPI_CLOCK_DIV4;
            break;
        
        case 2000000:
            clockDiv = SPI_CLOCK_DIV8;
            break;  

        case 1000000:
            clockDiv = SPI_CLOCK_DIV16;
            break;

        case 500000:
            clockDiv = SPI_CLOCK_DIV32;
            break;

        case 250000:
            clockDiv = SPI_CLOCK_DIV64;
            break;

        case 125000:
            clockDiv = SPI_CLOCK_DIV128;
            break;  
            
    }
}

void SPIClass::setClockDivider(uint32_t clockDiv) {
    clockDiv = clockDiv; //microseconds
}

inline void SPIClass::setDataBits(uint16_t bits) {
    /*
    const uint32_t mask = ~((SPIMMOSI << SPILMOSI) | (SPIMMISO << SPILMISO));
    bits--;
    SPI1U1 = ((SPI1U1 & mask) | ((bits << SPILMOSI) | (bits << SPILMISO)));
    */
   ///shrug
}

/*
Transfer 8 bits of data through SPI, then return the 8 bits of data read from the SPI bus.
*/
uint8_t SPIClass::transfer(uint8_t data) {
    
    setDataBits(8); //we're transferring 8 bits ig
    uint8_t ret = 0;

    switch (dataMode) {
        case SPI_MODE0:
            //transfer data with lsb first and read data into ret from SPI bus on SPI mode 0
            if (bitOrder == LSBFIRST) {
                for (uint8_t i = 0; i < 8; i++) {
                    nextCycleTime = micros64() + clockDiv;
                    digitalWrite(srclk, LOW);
                    digitalWrite(ser, (data & 0b00000001));
                    data >>= 1;
                    digitalWrite(srclk, HIGH);
                    ret >>= 1;
                    ret |= digitalRead(miso) << 7;

                    while (micros64() < nextCycleTime) {
                        //wait
                    }
                }
                digitalWrite(rclk, HIGH); //rising pulse
                digitalWrite(rclk, LOW); //low again
            } else {
                
                //digitalWrite(rclk, HIGH); //rising pulse
                //digitalWrite(rclk, LOW); //low again
                for (uint8_t i = 0; i < 8; i++) { //Quickly! Put out the info!
                    nextCycleTime = micros64() + clockDiv;
                    digitalWrite(srclk, LOW);
                    digitalWrite(ser, (data & 0b10000000));
                    data <<= 1;
                    digitalWrite(srclk, HIGH);
                    ret <<= 1;
                    ret |= digitalRead(miso);
                    while (micros64() < nextCycleTime) {
                        //wait
                    }
                }
                digitalWrite(rclk, HIGH); //rising pulse
                digitalWrite(rclk, LOW); //low again
                //For some reason it works like this
                //delayMicroseconds(1000);
                //Serial.println(rclk);
            }
            break;
        
        //We'll see when we get here
        default:
            Serial.println("Well fuck someone wanted SPI mode something else");
            break;
    }

    return ret;


}


//TODO: Complete this

uint16_t SPIClass::transfer16(uint16_t data) {
    union {
            uint16_t val;
            struct {
                    uint8_t lsb;
                    uint8_t msb;
            };
    } in, out;
    in.val = data;

    if(bitOrder == LSBFIRST) {
        //LSBFIRST
        out.lsb = transfer(in.lsb);
        out.msb = transfer(in.msb);
    } else {
        //MSBFIRST
        out.msb = transfer(in.msb);
        out.lsb = transfer(in.lsb);
    }
    return out.val;
}

//todo, actually implement this

/*
void SPIClass::transfer(void *buf, uint16_t count) {
    uint8_t *cbuf = reinterpret_cast<uint8_t*>(buf);

    // cbuf may not be 32bits-aligned
    for (; (((unsigned long)cbuf) & 3) && count; cbuf++, count--)
        *cbuf = transfer(*cbuf);

    // cbuf is now aligned
    // count may not be a multiple of 4
    uint16_t count4 = count & ~3;
    transferBytes(cbuf, cbuf, count4);

    // finish the last <4 bytes
    cbuf += count4;
    count -= count4;
    for (; count; cbuf++, count--)
        *cbuf = transfer(*cbuf);
}


void SPIClass::write(uint8_t data) {
    while(SPI1CMD & SPIBUSY) {}
    // reset to 8Bit mode
    setDataBits(8);
    SPI1W0 = data;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}

void SPIClass::write16(uint16_t data) {
    write16(data, !(SPI1C & (SPICWBO | SPICRBO)));
}

void SPIClass::write16(uint16_t data, bool msb) {
    while(SPI1CMD & SPIBUSY) {}
    // Set to 16Bits transfer
    setDataBits(16);
    if(msb) {
        // MSBFIRST Byte first
        SPI1W0 = (data >> 8) | (data << 8);
    } else {
        // LSBFIRST Byte first
        SPI1W0 = data;
    }
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}

void SPIClass::write32(uint32_t data) {
    write32(data, !(SPI1C & (SPICWBO | SPICRBO)));
}

void SPIClass::write32(uint32_t data, bool msb) {
    while(SPI1CMD & SPIBUSY) {}
    // Set to 32Bits transfer
    setDataBits(32);
    if(msb) {
        union {
                uint32_t l;
                uint8_t b[4];
        } data_;
        data_.l = data;
        // MSBFIRST Byte first
        data = (data_.b[3] | (data_.b[2] << 8) | (data_.b[1] << 16) | (data_.b[0] << 24));
    }
    SPI1W0 = data;
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}
*/

/*

/**
 * Note:
 *  data need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param data uint8_t *
 * @param size uint32_t
 */
/*
void SPIClass::writeBytes(const uint8_t * data, uint32_t size) {
    while(size) {
        if(size > 64) {
            writeBytes_(data, 64);
            size -= 64;
            data += 64;
        } else {
            writeBytes_(data, size);
            size = 0;
        }
    }
}

void SPIClass::writeBytes_(const uint8_t * data, uint8_t size) {
    while(SPI1CMD & SPIBUSY) {}
    // Set Bits to transfer
    setDataBits(size * 8);

    uint32_t * fifoPtr = (uint32_t*)&SPI1W0;
    const uint32_t * dataPtr = (uint32_t*) data;
    uint32_t dataSize = ((size + 3) / 4);

    while(dataSize--) {
        *fifoPtr = *dataPtr;
        dataPtr++;
        fifoPtr++;
    }

    __sync_synchronize();
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}
}
*/

/*
/**
 * @param data uint8_t *
 * @param size uint8_t  max for size is 64Byte
 * @param repeat uint32_t
 */
/*
void SPIClass::writePattern(const uint8_t * data, uint8_t size, uint32_t repeat) {
    if(size > 64) return; //max Hardware FIFO

    while(SPI1CMD & SPIBUSY) {}

    uint32_t buffer[16];
    uint8_t *bufferPtr=(uint8_t *)&buffer;
    const uint8_t *dataPtr = data;
    volatile uint32_t * fifoPtr = &SPI1W0;
    uint8_t r;
    uint32_t repeatRem;
    uint8_t i;

    if((repeat * size) <= 64){
        repeatRem = repeat * size;
        r = repeat;
        while(r--){
            dataPtr = data;
            for(i=0; i<size; i++){
                *bufferPtr = *dataPtr;
                bufferPtr++;
                dataPtr++;
            }
        }

        r = repeatRem;
        if(r & 3) r = r / 4 + 1;
        else r = r / 4;
        for(i=0; i<r; i++){
            *fifoPtr = buffer[i];
            fifoPtr++;
        }
        SPI1U = SPIUMOSI | SPIUSSE;
    } else {
        //Orig
        r = 64 / size;
        repeatRem = repeat % r * size;
        repeat = repeat / r;

        while(r--){
            dataPtr = data;
            for(i=0; i<size; i++){
                *bufferPtr = *dataPtr;
                bufferPtr++;
                dataPtr++;
            }
        }

        //Fill fifo with data
        for(i=0; i<16; i++){
            *fifoPtr = buffer[i];
            fifoPtr++;
        }

        r = 64 / size;

        SPI1U = SPIUMOSI | SPIUSSE;
        setDataBits(r * size * 8);
        while(repeat--){
            SPI1CMD |= SPIBUSY;
            while(SPI1CMD & SPIBUSY) {}
        }
    }
    //End orig
    setDataBits(repeatRem * 8);
    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    SPI1U = SPIUMOSI | SPIUDUPLEX | SPIUSSE;
}
*/

/**
 * @param out uint8_t *
 * @param in  uint8_t *
 * @param size uint32_t
 */
/*
void SPIClass::transferBytes(const uint8_t * out, uint8_t * in, uint32_t size) {
    while(size) {
        if(size > 64) {
            transferBytes_(out, in, 64);
            size -= 64;
            if(out) out += 64;
            if(in) in += 64;
        } else {
            transferBytes_(out, in, size);
            size = 0;
        }
    }
}
*/
/**
 * Note:
 *  in and out need to be aligned to 32Bit
 *  or you get an Fatal exception (9)
 * @param out uint8_t *
 * @param in  uint8_t *
 * @param size uint8_t (max 64)
 */
/*
void SPIClass::transferBytesAligned_(const uint8_t * out, uint8_t * in, uint8_t size) {
    if (!size)
        return;

    while(SPI1CMD & SPIBUSY) {}
    // Set in/out Bits to transfer

    setDataBits(size * 8);

    volatile uint32_t *fifoPtr = &SPI1W0;

    if (out) {
        uint8_t outSize = ((size + 3) / 4);
        uint32_t *dataPtr = (uint32_t*) out;
        while (outSize--) {
            *(fifoPtr++) = *(dataPtr++);
        }
    } else {
        uint8_t outSize = ((size + 3) / 4);
        // no out data only read fill with dummy data!
        while (outSize--) {
            *(fifoPtr++) = 0xFFFFFFFF;
        }
    }

    SPI1CMD |= SPIBUSY;
    while(SPI1CMD & SPIBUSY) {}

    if (in) {
        uint32_t *dataPtr = (uint32_t*) in;
        fifoPtr = &SPI1W0;
        int inSize = size;
        // Unlike outSize above, inSize tracks *bytes* since we must transfer only the requested bytes to the app to avoid overwriting other vars.
        while (inSize >= 4) {
            *(dataPtr++) = *(fifoPtr++);
            inSize -= 4;
            in += 4;
        }
        volatile uint8_t *fifoPtrB = (volatile uint8_t *)fifoPtr;
        while (inSize--) {
            *(in++) = *(fifoPtrB++);
        }
    }
}


void SPIClass::transferBytes_(const uint8_t * out, uint8_t * in, uint8_t size) {
    if (!((uint32_t)out & 3) && !((uint32_t)in & 3)) {
        // Input and output are both 32b aligned or NULL
        transferBytesAligned_(out, in, size);
    } else {
        // HW FIFO has 64b limit and ::transferBytes breaks up large xfers into 64byte chunks before calling this function
        // We know at this point at least one direction is misaligned, so use temporary buffer to align everything
        // No need for separate out and in aligned copies, we can overwrite our out copy with the input data safely
        uint8_t aligned[64]; // Stack vars will be 32b aligned
        if (out) {
            memcpy(aligned, out, size);
        }
        transferBytesAligned_(out ? aligned : nullptr, in ? aligned : nullptr, size);
        if (in) {
            memcpy(in, aligned, size);
        }
    }
}
*/

#if !defined(NO_GLOBAL_INSTANCES) && !defined(NO_GLOBAL_SPI)
SPIClass SPI;
#endif
