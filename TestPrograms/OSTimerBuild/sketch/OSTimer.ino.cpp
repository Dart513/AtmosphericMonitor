#line 1 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino"
#include "Arduino.h"
#include <Ticker.h>
#include "osapi.h"

volatile bool state;



#line 9 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino"
void timerCallback();
#line 16 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino"
void setup();
#line 28 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino"
void loop();
#line 9 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\OSTimer\\OSTimer.ino"
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


