#ifndef INTERFACE_SCREENS_H
#define INTERFACE_SCREENS_H

#include "Graphics/GFXContainer.h"
#include "Encoder.h"
#include "ClimateMonitor.h"

Encoder enc(RE_PIN_A, RE_PIN_B, RE_BUTTON);

bool interruptingFunction()
{
    static uint32_t oldTime = millis();
    if (millis() - oldTime > 0 || millis() < oldTime)
    {
        oldTime = millis();
        enc.update();
    }

    enc.update();

    uint16_t prevActionHistoryLength = 0;
    uint16_t actionHistoryLength = enc.getActionHistoryLength();
    bool ret = false;
    while (actionHistoryLength - prevActionHistoryLength > 0) {
        ret = true;
        for (int i = 0; i < 8; i++)
        {
            enc.update();
            delay(1);
        }
        prevActionHistoryLength = actionHistoryLength;
        actionHistoryLength = enc.getActionHistoryLength();
    }
    return ret;
}


namespace Nav {
    //Top layer
    GFXContainer splashScreen;
    GFXContainer climate;
    GFXContainer airQuality;
    GFXContainer settings;

    //Climate layer
    GFXContainer escapeFromClimate;
    GFXContainer temperature;
    GFXContainer humidity;
    GFXContainer pressure;

    //Air Quality layer
    GFXContainer escapeFromAirQuality;
    GFXContainer iaq; //use the static one
    GFXContainer bVoc;
    GFXContainer co2;

    //Settings layer
    GFXContainer escapeFromSettings;
    GFXContainer brightness;
    GFXContainer volume;

    String dataString;
    String dataString2;
    String dataString3;

    const String messages[] = {"Starting sensor", "Gathering data\nSensor may\nbe inaccurate", "Calibrating sensor", ""};
    const String messages2[] = {"Data is not ready", "Data is not ready", "Please wait", ""};

    void init(std::shared_ptr<GFXContainer> splashScreenPtr) {

        /*=========================================== Populate Data ===========================================*/
        splashScreen.addData(TextualInfo("Climate\nMonitoring\nSystem", ORANGE, 1.5))
            .addData(TextualInfo(String("Version 1.0.0\nBy Darton Li"), ORANGE, 1))
            .setInterruptingFunction(&interruptingFunction);

        climate.addData(TextualInfo("Climate", DARK_GREY, 2))
            .setColor(LIGHT_BLUE)
            .setInterruptingFunction(&interruptingFunction);

        airQuality.addData(TextualInfo("Air\nQuality", DARK_GREY, 2))
            .setColor(MUTED_CYAN)
            .setInterruptingFunction(&interruptingFunction);

        settings.addData(TextualInfo("Settings", DARK_GREY, 2))
            .setColor(LIME)
            .setInterruptingFunction(&interruptingFunction);

        /*================================ Climate ==============================*/
        escapeFromClimate.addData(TextualInfo("Back", WHITE, 2))
            .setColor(BLACK)
            .setInterruptingFunction(&interruptingFunction);
        
        temperature.addData(TextualInfo("Temperature", DARK_GREY, 1.5))
            .addData(TextualInfo(&dataString, LIME, 2))
            .setColor(LIGHT_BLUE)
            .setPrerender([](GFXContainer & gfxScreen) -> void
            {
                String tempString = String(CM::iaqSensor.temperature,1) + DEG_C;
                if (tempString != dataString)
                {
                    gfxScreen.unprint();
                    dataString = tempString;
                }
            })
            .setInterruptingFunction(&interruptingFunction);

        humidity.addData(TextualInfo("Humidity", DARK_GREY, 1.5))
            .addData(TextualInfo(&dataString, LIME, 2))
            .setColor(LIGHT_BLUE)
            .setPrerender([](GFXContainer & gfxScreen) -> void
            {
                String tempString = String(CM::iaqSensor.humidity,1) + PERCENT;
                if (tempString != dataString)
                {
                    gfxScreen.unprint();
                    dataString = tempString;
                }
            })
            .setInterruptingFunction(&interruptingFunction);

        pressure.addData(TextualInfo("Pressure", DARK_GREY, 1.5))
            .addData(TextualInfo(&dataString, LIME, 2))
            .setColor(LIGHT_BLUE)
            .setPrerender([](GFXContainer & gfxScreen) -> void
            {
                String tempString = String(CM::iaqSensor.pressure * 0.000009869233, 3) + ATM;
                if (tempString != dataString)
                {
                    gfxScreen.unprint();
                    dataString = tempString;
                }
            })
            .setInterruptingFunction(&interruptingFunction);

        /*================================ Air Quality ==============================*/
        escapeFromAirQuality.addData(TextualInfo("Back", WHITE, 2))
            .setColor(BLACK)
            .setInterruptingFunction(&interruptingFunction);

        iaq.addData(TextualInfo("IAQ", DARK_GREY, 2))
            .addData(TextualInfo(&dataString, ORANGE, 2))
            .addData(TextualInfo(&dataString2, RED, 1))
            .addData(TextualInfo(&dataString3, RED, 1))
            .setColor(MUTED_CYAN)
            .setPrerender([](GFXContainer & gfxScreen) -> void
            {
                String tempString = String(CM::iaqSensor.staticIaq);
                String tempString2 = String(messages[CM::iaqSensor.staticIaqAccuracy]);
                String tempString3 = String(messages2[CM::iaqSensor.staticIaqAccuracy]);
                if (tempString != dataString || tempString2 != dataString2 || tempString3 != dataString3)
                {
                    gfxScreen.unprint();
                    dataString = tempString;
                    dataString2 = tempString2;
                    dataString3 = tempString3;
                }
            })
            .setInterruptingFunction(&interruptingFunction);

        bVoc.addData(TextualInfo("Breath VOC\nEquivalent", DARK_GREY, 1))
            .addData(TextualInfo(&dataString, ORANGE, 2))
            .addData(TextualInfo(&dataString2, RED, 1))
            .addData(TextualInfo(&dataString3, RED, 1))
            .setColor(MUTED_CYAN)
            .setPrerender([](GFXContainer & gfxScreen) -> void
            {
                String tempString = String(CM::iaqSensor.breathVocEquivalent, 2) + PPM;
                String tempString2 = String(messages[CM::iaqSensor.iaqAccuracy]);
                String tempString3 = String(messages2[CM::iaqSensor.iaqAccuracy]);
                if (tempString != dataString || tempString2 != dataString2 || tempString3 != dataString3)
                {
                    gfxScreen.unprint();
                    dataString = tempString;
                    dataString2 = tempString2;
                    dataString3 = tempString3;
                }
            })
            .setInterruptingFunction(&interruptingFunction);

        co2.addData(TextualInfo("CO2\nPollution\nEquivalent", DARK_GREY, 1))
            .addData(TextualInfo(&dataString, ORANGE, 2))
            .addData(TextualInfo(&dataString2, RED, 1))
            .addData(TextualInfo(&dataString3, RED, 1))
            .setColor(MUTED_CYAN)
                        .setPrerender([](GFXContainer & gfxScreen) -> void
            {
                String tempString = String(CM::iaqSensor.co2Equivalent, 2) + PPM;
                String tempString2 = String(messages[CM::iaqSensor.iaqAccuracy]);
                String tempString3 = String(messages2[CM::iaqSensor.iaqAccuracy]);
                if (tempString != dataString || tempString2 != dataString2 || tempString3 != dataString3)
                {
                    gfxScreen.unprint();
                    dataString = tempString;
                    dataString2 = tempString2;
                    dataString3 = tempString3;
                }
            })
            .setInterruptingFunction(&interruptingFunction);

        /*================================ Settings ==============================*/
        //TODO: Later.


        //Link everything together
        splashScreen.setSelf(splashScreenPtr);

        splashScreen.setNext(&climate);
        climate.setNext(&airQuality);
        airQuality.setNext(&settings);

        climate.setDown(&escapeFromClimate);
        escapeFromClimate.setNext(&temperature);
        temperature.setNext(&humidity);
        humidity.setNext(&pressure);

        airQuality.setDown(&escapeFromAirQuality);
        escapeFromAirQuality.setNext(&iaq);
        iaq.setNext(&bVoc);
        bVoc.setNext(&co2);

    }
}

#endif