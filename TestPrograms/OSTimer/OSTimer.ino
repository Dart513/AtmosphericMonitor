#include "Arduino.h"
#include <Ticker.h>
#include "osapi.h"

volatile bool state;



void timerCallback() {
    state = !state;
    digitalWrite(12, state);
    timer1_write(5*1000 * 500);
}


void setup()
{
    Serial.begin(115200);
    pinMode(12, OUTPUT);

    timer1_attachInterrupt(timerCallback);
    timer1_enable(TIM_DIV16, TIM_EDGE, TIM_SINGLE);
    timer1_write(5*500); //500 us
}



void loop()
{
}

