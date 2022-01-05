#ifndef BATT_MONITOR_H
#define BATT_MONITOR_H

#include <Arduino.h>
#include "../LinearLookupTable.h"


class BatteryMonitor {
    private:
    uint8_t adcPin;
    LookupTable<double, double> voltageToCapacity;
    double maxVoltage;
    double minVoltage;
    double chargeVoltage;
    double maxCapacity;
    double adcMultiplier;
    uint16_t maxADC; //Assume adc goes from 0 to maxADC

    static double dMap(double in, double inMin, double inMax, double outMin, double outMax)
    {
        return outMin + (outMax - outMin) * (in - inMin) / (inMax - inMin);
    }

    public:
    BatteryMonitor() {
        maxVoltage = minVoltage = maxCapacity = chargeVoltage = adcMultiplier =  0;
        maxADC = 1024;
    }

    BatteryMonitor(uint8_t adcPin0, uint16_t maxADC0, double chargeVoltage0, double adcMultiplier0, LookupTable<double, double> voltageToCapacity0) {
        adcPin = adcPin0;
        maxADC = maxADC0;
        voltageToCapacity = voltageToCapacity0;
        adcMultiplier = adcMultiplier0;

        chargeVoltage = chargeVoltage0;

        maxVoltage = voltageToCapacity.getMaxX();
        minVoltage = voltageToCapacity.getMinX();

        maxCapacity = voltageToCapacity.getMaxY();
        pinMode(adcPin, INPUT);
    }

    int getBatteryState(bool * isCharging) {

        double currVoltage = dMap((double) analogRead(adcPin), 0.0, maxADC, 0.0, adcMultiplier);
        //return currVoltage * 100;

        if (currVoltage >= chargeVoltage) 
            *isCharging = true;
        else
            *isCharging = false;

        return (int)(voltageToCapacity.getValue(currVoltage) / maxCapacity * 100); //use arduino's map instead of normal linear mapping 
    }

    int getBatteryState()
    {
        static bool isCharging = false;
        return getBatteryState(&isCharging);
    }
};



#endif