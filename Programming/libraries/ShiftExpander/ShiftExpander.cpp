//Implementation of SHIFT_EXPANDER.h

#include "ShiftExpander.h"
//#define DEBUG_PINS_ENABLED
//Checks if timer needs update, if it does, shift out
//NOTE: core_esp8266_wiring_digital.cpp has been changed.



namespace ShiftExpander {
    uint8_t pins = 0;
    uint8_t SER = 13; 
    uint8_t SRCLK = 14;
    uint8_t RCLK = 15;

    void setPins(uint8_t SER0, uint8_t SRCLK0, uint8_t RCLK0)
    {
        SER = SER0;
        SRCLK = SRCLK0;
        RCLK = RCLK0;

        pinMode(SER, OUTPUT);
        pinMode(SRCLK, OUTPUT);
        pinMode(RCLK, OUTPUT);
    }

    void updatePins() {
        //digitalWrite(RCLK, LOW);
        shiftOut(SER, SRCLK, MSBFIRST, pins);
        digitalWrite(RCLK, HIGH);
        digitalWrite(RCLK, LOW);

        #ifdef DEBUG_PINS_ENABLED
        Serial.printf("OOP UPDATIN' PINS, %#0x, %d %d %d\n", pins, SER, SRCLK, RCLK);
        #endif
    }

    void digitalWriteExpanded(uint8_t pin, uint8_t value) {
        #ifdef DEBUG_PINS_ENABLED
        Serial.printf("DigitalWrite Called %d %d\n", pin, value);
        #endif
        
        if (pin < 17) {
            digitalWrite(pin, value);
        }
        else{
            uint8_t editMask = (value << (pin - 17));
            pins &= (editMask | pinMasks[pin - 17]);
            pins |= editMask;
            updatePins();
        }
    }

}



/*
ShiftExpander::ShiftExpander(int SER0 = 14, int SRCLK0 = 15, int RCLK0 = 5)
{
    SER = SER0;
    SRCLK = SRCLK0;
    RCLK = RCLK0;
}

void ShiftExpander::begin()
{
    pinMode(SER, OUTPUT);
    pinMode(SRCLK, OUTPUT);
    pinMode(RCLK, OUTPUT);

    
    timer1_attachInterrupt(updatePins);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
    timer1_write(5*1000 * 500); //1000 us * 500* ///TODO: MAYBE IT'S THE BIND? TRY PARAMETERLESS.

}

void ShiftExpander::end()
{
    timer1_detachInterrupt();
}

*/