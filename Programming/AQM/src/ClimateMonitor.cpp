#include "ClimateMonitor.h"

namespace CM {

    const uint8_t bsec_config_iaq[] = {
        #include "config/generic_33v_3s_4d/bsec_iaq.txt"
    };


    Bsec iaqSensor; //Sensor object
    uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE]; //Historical States
    uint32_t stateUpdateCounter; //How many times we've updated the state


    //Sets up the BSEC object
    void init() {
        pinMode(CS_BSEC, OUTPUT);
        iaqSensor.begin(CS_BSEC, SPI);

        Serial.printf("\nBSEC: BSEC library version %d.%d.%d.%d\n", iaqSensor.version.major, 
            iaqSensor.version.minor, iaqSensor.version.major_bugfix, iaqSensor.version.minor_bugfix);
        checkIaqSensorStatus();

        bsec_virtual_sensor_t sensorList[8] = {
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
            BSEC_OUTPUT_RAW_PRESSURE,
            BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
            BSEC_OUTPUT_RAW_GAS,
            BSEC_OUTPUT_IAQ,
            BSEC_OUTPUT_STATIC_IAQ,
            BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
            BSEC_OUTPUT_CO2_EQUIVALENT
        };

        iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
        checkIaqSensorStatus();

        // Print the header
        Serial.println(F("Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%]"));
    }


    void loop() {
        if (iaqSensor.run()) { // If new data is available, post it to the console
            Serial.printf("%d, %f, %f, %f, %f, %f, %f, %d, %d, %f, %f\n", 
                millis(), 
                iaqSensor.rawTemperature, 
                iaqSensor.pressure, 
                iaqSensor.rawHumidity,
                iaqSensor.gasResistance,
                iaqSensor.iaq,
                iaqSensor.staticIaq,
                iaqSensor.iaqAccuracy,
                iaqSensor.staticIaqAccuracy, 
                iaqSensor.temperature,
                iaqSensor.humidity);
        } else {
            checkIaqSensorStatus();
        }
    }

    void checkIaqSensorStatus() {
        if (iaqSensor.status != BSEC_OK) {
            if (iaqSensor.status < BSEC_OK) {
            Serial.printf("BSEC error code : %d\n", iaqSensor.status);
            for (;;)
                errLeds(); /* Halt in case of failure */
            } else {
            Serial.printf("BSEC warning code : %d\n", iaqSensor.status);
            }
        }

        if (iaqSensor.bme680Status != BME680_OK) {
            if (iaqSensor.bme680Status < BME680_OK) {
            Serial.printf("BME680 error code : %d\n", iaqSensor.bme680Status);
            for (;;)
                errLeds(); /* Halt in case of failure */
            } else {
            Serial.printf("BME680 warning code : %d\n", iaqSensor.bme680Status);
            }
        }
        iaqSensor.status = BSEC_OK;
    }

    void errLeds() {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(100);
        digitalWrite(LED_BUILTIN, LOW);
        delay(100);
    }  

    //BSEC Config
}