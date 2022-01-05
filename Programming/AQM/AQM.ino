/**
 * @file AQM.ino
 * @author Darton Li (darton.li@uwaterloo.com)
 * @brief 
 * @version 0.1
 * @date 2021-12-24
 * 
 * @copyright Copyright (c) 2021
 * 
 *  Code to run air quality monitoring device with BSEC sensor suite and various input/output devices 
 */

/**========================================================================
 *                                Includes
 *========================================================================**/

//#define DEBUG_PINS_ENABLED

#include <Arduino.h>
#include "src/pins.h"
#include "src/ClimateMonitor.h"
#include "src/Graphics/Graphics.h"
#include "src/Graphics/GFXContainer.h"
#include "src/Encoder.h"
#include "src/LinearLookupTable.h"
#include "src/batteryLookup.h"
#include <memory>


#include "src/interfaceScreens.h"//include the GUI screens here
#include "src/Graphics/BattMonitor.h"

std::shared_ptr<GFXContainer> screen(&Nav::splashScreen);


//Recall: Pointer decay prevents us from finding size of array inside the method. Remember this.
LookupTable<double, double> table(BatteryLookup::voltages, BatteryLookup::capacities, 
        (sizeof BatteryLookup::voltages) / (sizeof BatteryLookup::voltages[0]));
BatteryMonitor battMonitor(A0, 1024, 4.25, 5, table);

void setup()
{
    Serial.begin(500000);

    pinMode(AQM_SCK, OUTPUT);
    pinMode(AQM_SDO, OUTPUT);
    pinMode(AQM_SDI, INPUT);

    SPI.begin();
    ShiftExpander::setPins(AQM_SR_SER, AQM_SR_SRCLK, AQM_SR_RCLK);
    CM::init();
    GFX::init();
    Nav::init(screen);
}


void loop()
{
    //Serial.println(table.getValue(4.060803977272727), 3);
    //bool isCharging = false;
    //Serial.println(battMonitor.getBatteryState(&isCharging));
    //Serial.println("AA");
    
    CM::loop();

    //GFX::drawTextCentered(String("Hello World"), 1, GFX::tft.width()/2, GFX::tft.height()/2, ORANGE);
    uint16_t oldTime = millis();
    screen->render();
    //Serial.printf("The thing took %d millis to render\n", millis() - oldTime);
    oldTime = millis();

    enc.update();
    
    //Execute actions
    
    std::list<int8_t> actions = 
        enc.getNewActionHistory();
    actions.size();
    
    //Serial.printf("We're done printing actions %d\n", actions.size());
    


    
    if (actions.size() > 0) {
        Serial.println("We're in the if statement");
        for (auto & action : actions) {
            Serial.printf("Action: %d\n", action);
            std::shared_ptr<GFXContainer> newScreen = screen->move(action);
            if (newScreen!=nullptr)
            {
                Serial.printf("We're gonna go and change screens now %d\n", newScreen == nullptr);
                screen = newScreen;
            }
        }
    }    
    
    GFXContainer::setBattery(battMonitor.getBatteryState());


}