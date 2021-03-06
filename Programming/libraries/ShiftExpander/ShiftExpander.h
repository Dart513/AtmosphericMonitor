/*
#ifndef SHIFT_EXPANDER_H
#define SHIFT_EXPANDER_H

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

#define SHIFT_0 3
#define SHIFT_1 6
#define SHIFT_2 9
#define SHIFT_3 12
#define SHIFT_4 15
#define SHIFT_5 18
#define SHIFT_6 21
#define SHIFT_7 24


#define MASK_0 0x00000008
#define MASK_1 0x00000080
#define MASK_2 0x00000800
#define MASK_3 0x00008000
#define MASK_4 0x00080000
#define MASK_5 0x00800000
#define MASK_6 0x08000000
#define MASK_7 0x80000000


extern volatile uint32_t tempPins;
extern volatile uint32_t oldPins;
extern volatile uint8_t outputRegister;
extern volatile uint8_t SER, SRCLK, RCLK;

//not strictly necessary
enum EXPANDED_PINS_0
{
    EP16 = 16,
    EP17 = 17,
    EP18 = 18,
    EP19 = 19,
    EP20 = 20,
    EP21 = 21,
    EP22 = 22,
    EP23 = 23
};

//Class for holding shift register
class ShiftExpander
{
    public:
    ShiftExpander(int SER, int SRCLK, int RCLK);
    void begin();
    void end();
    //std::tuple<ShiftExpander> getAsTuple()
};

void updatePins();

#endif
*/


#ifndef SHIFT_EXPANDER_H
#define SHIFT_EXPANDER_H

#include <Arduino.h>

namespace ShiftExpander {
    const uint8_t pinMasks[8] = {
        0xFE, 0xFD, 0xFB, 0xF7, 0xEF, 0xDF, 0xBF, 0x7F
    };

    extern uint8_t pins, SER, SRCLK, RCLK;
    void setPins(uint8_t SER0, uint8_t SRCLK0, uint8_t RCLK0);
    void updatePins();
    void digitalWriteExpanded(uint8_t pin, uint8_t value);
}



#endif