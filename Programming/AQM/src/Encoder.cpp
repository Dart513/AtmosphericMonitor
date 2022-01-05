#include "Encoder.h"

Encoder::Encoder()
{
    pinA = -1;
    pinB = -1;
    pinButton = -1;

    //zero all other private variables
    rotaryStore = 0;
    buttonStore = 0;

    rotaryValue = 0;
    velocity = 0;

    prevNextCode = 0;

    buttonState = false;
}

Encoder::Encoder(uint8_t pinA0, uint8_t pinB0, uint8_t pinButton0)
{
    pinA = pinA0;
    pinB = pinB0;
    pinButton = pinButton0;

    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);
    pinMode(pinButton, INPUT);

    //zero all other private variables
    rotaryStore = 0;
    buttonStore = 0;

    rotaryValue = 0;
    velocity = 0;

    prevNextCode = 0;

    buttonState = false;
}

void Encoder::init(uint8_t pinA0, uint8_t pinB0, uint8_t pinButton0)
{
    pinA = pinA0;
    pinB = pinB0;
    pinButton = pinButton0;

    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);
    pinMode(pinButton, INPUT);

    //zero all other private variables
    rotaryStore = 0;
    buttonStore = 0;

    rotaryValue = 0;
    velocity = 0;

    prevNextCode = 0;

    buttonState = false;
}


void Encoder::update()
{
    //First, read and debounce click button;
    bool button = false;

    buttonStore <<= 1;

    if (pinButton > -1)
    {
        if (digitalRead(pinButton)) {
            buttonStore |= 0x1;
        }

        if (buttonStore == 0xFF) button = true;
        //The program may have to distinguish a rising edge from a holding value.

        if (button != buttonState)
        {
            Serial.println("Button state changed");
            buttonState = button;
            if (button == true)
                actionHistory.push_back(BUTTON);
        }
    }

    if (pinA < 0 || pinB < 0) return; 

    
    //Now, read and debounce rotary encoder;

    static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

    prevNextCode <<= 2;
    if (digitalRead(pinA)) prevNextCode |= 0x02;
    if (digitalRead(pinB)) prevNextCode |= 0x01;
    
    //Serial.printf("A: %d, B: %d, prevNextCode: %d\n", digitalRead(pinA), digitalRead(pinB), prevNextCode);

    prevNextCode &= 0x0f;

    // If valid then store it
    if  (rot_enc_table[prevNextCode] ) {
        rotaryStore <<= 4;
        rotaryStore |= prevNextCode;

        if ((rotaryStore&0xff)==0x2b) {
            actionHistory.push_back(BWD);
            rotaryValue--;
        }
        if ((rotaryStore&0xff)==0x17) {
            actionHistory.push_back(FWD);
            rotaryValue++;
        }
   }   
}

int16_t Encoder::getTicks()
{
    return rotaryValue;
}

std::list<int8_t> Encoder::getNewActionHistory()
{       
    std::list<int8_t> tempActions = actionHistory;
    //Serial.printf("Original temp action size: %d\n", tempActions.size());
    actionHistory.clear();
    //Serial.printf("Actions: %d\n", tempActions.size());
    return tempActions;
}

bool Encoder::getButton()
{
    return buttonState;
}







