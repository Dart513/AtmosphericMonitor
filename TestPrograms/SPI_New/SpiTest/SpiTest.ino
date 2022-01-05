#include <Arduino.h>
#include "ShiftExpander.h"
#include "SPI.h"

//using namespace arduino;



void setup()
{
    ShiftExpander::setPins(13, 14, 15);
    Serial.begin(115200);
}

void loop()
{
    //digitalWriteExpanded(18, HIGH);
    delay(500);
    //digitalWriteExpanded(18, LOW);
    delay(500);
}