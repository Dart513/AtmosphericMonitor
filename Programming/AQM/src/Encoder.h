#ifndef ENCODER_H
#define ENCODER_H

#include <Arduino.h>
#include <list>
#include "pins.h"


class Encoder {
    private:
    uint8_t pinA;
    uint8_t pinB;
    uint8_t pinButton;
    
    uint16_t rotaryStore;
    uint16_t buttonStore;

    int16_t rotaryValue;

    uint8_t prevNextCode;

    int16_t velocity;//TODO: Maybe implement velocity-based stuff? 
    
    std::list<int8_t> actionHistory;


    //State
    bool buttonState;
    
    public:
    Encoder();
    Encoder(uint8_t pinA0, uint8_t pinB0, uint8_t pinButton0);
    void init(uint8_t pinA0, uint8_t pinB0, uint8_t pinButton0);

    int16_t getTicks();
    std::list<int8_t> getNewActionHistory();
    uint16_t getActionHistoryLength() {
        return actionHistory.size();
    }
    bool getButton();
    void update();

};

#endif