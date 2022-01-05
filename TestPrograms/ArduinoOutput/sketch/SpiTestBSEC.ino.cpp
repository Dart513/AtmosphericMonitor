#include <Arduino.h>
#line 1 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino"
#include <ShiftExpander.h>
#include <SPI.h>
#include <bsec_SE.h>
#include <EEPROM.h>

//PINS
#define PIN_A           5
#define PIN_B           4
#define SELECTOR_BUTTON 15
#define CS_BSEC         18

//Misc
#define STATE_SAVE_PERIOD  UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day


//BSEC Config
const uint8_t bsec_config_iaq[] = {
#include "config/generic_33v_3s_4d/bsec_iaq.txt"
};


//Predefine Functions
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);
void drawLCD(uint16_t rotation);



// Create an object of the class Bsec
Bsec iaqSensor;
uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE] = {0};
uint16_t stateUpdateCounter = 0;




#line 39 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino"
void setup();
#line 83 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino"
void loop();
#line 39 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino"
void setup() {
  //Set pins
  pinMode(PIN_A, INPUT);
  pinMode(PIN_B, INPUT);
  pinMode(14, OUTPUT);
  pinMode(12, INPUT);
  pinMode(13, INPUT);
  pinMode(CS_BSEC, OUTPUT);
  
  ShiftExpander::setPins(PIN_A, PIN_B, 16);
  Serial.begin(500000);
  SPI.begin();

  //Initialize BSEC
  EEPROM.begin(BSEC_MAX_STATE_BLOB_SIZE + 1); // 1st address for the length
  iaqSensor.begin(CS_BSEC, SPI); //Pin 2 will be the CS pin

  //BSEC Debug Info
  Serial.printf("\nBSEC library version %d.%d.%d.%d\n", iaqSensor.version.major, iaqSensor.version.minor, iaqSensor.version.major_bugfix, iaqSensor.version.minor_bugfix);
  checkIaqSensorStatus();

  //iaqSensor.setConfig(bsec_config_iaq); //setConfig doesn't work on ESP8266
  checkIaqSensorStatus();

  loadState();

  bsec_virtual_sensor_t sensorList[7] = {
    BSEC_OUTPUT_RAW_TEMPERATURE,
    BSEC_OUTPUT_RAW_PRESSURE,
    BSEC_OUTPUT_RAW_HUMIDITY,
    BSEC_OUTPUT_RAW_GAS,
    BSEC_OUTPUT_IAQ,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
    BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
  };

  iaqSensor.updateSubscription(sensorList, 7, BSEC_SAMPLE_RATE_LP);
  checkIaqSensorStatus();

  // Print the header
  Serial.println("Timestamp [ms], raw temperature [°C], pressure [hPa], raw relative humidity [%], gas [Ohm], IAQ, IAQ accuracy, temperature [°C], relative humidity [%]");
}


void loop() {
  unsigned long time_trigger = millis();
  if (iaqSensor.run()) { // If new data is available
    Serial.printf("%d, %d, %d, %d, %d, %d, %d, %d, %d\n", 
        time_trigger, 
        iaqSensor.rawTemperature, 
        iaqSensor.pressure, 
        iaqSensor.rawHumidity,
        iaqSensor.gasResistance,
        iaqSensor.iaq,
        iaqSensor.iaqAccuracy, 
        iaqSensor.temperature,
        iaqSensor.humidity);
    updateState();
  } else {
    checkIaqSensorStatus();
  }
}


// Helper function definitions
void checkIaqSensorStatus(void)
{
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

void errLeds(void)
{
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
}

void loadState(void)
{
  if (EEPROM.read(0) == BSEC_MAX_STATE_BLOB_SIZE) {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE; i++) {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], HEX);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  } else {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");
  
    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE + 1; i++)
      EEPROM.write(i, 0);

  }
}

void updateState(void)
{
  bool update = false;
  /* Set a trigger to save the state. Here, the state is saved every STATE_SAVE_PERIOD with the first state being saved once the algorithm achieves full calibration, i.e. iaqAccuracy = 3 */
  if (stateUpdateCounter == 0) {
    if (iaqSensor.iaqAccuracy >= 3) {
      update = true;
      stateUpdateCounter++;
    }
  } else {
    /* Update every STATE_SAVE_PERIOD milliseconds */
    if ((stateUpdateCounter * STATE_SAVE_PERIOD) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < BSEC_MAX_STATE_BLOB_SIZE ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], HEX);
    }

    EEPROM.write(0, BSEC_MAX_STATE_BLOB_SIZE);
    EEPROM.commit();
  }
}




