# 1 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino"
# 2 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino" 2
# 3 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino" 2
# 4 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino" 2
# 5 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino" 2

//PINS





//Misc



//BSEC Config
const uint8_t bsec_config_iaq[] = {
# 19 "c:\\Users\\darto\\Documents\\PROJECTS\\AQM\\TestPrograms\\SPI_New\\SpiTestBSEC\\SpiTestBSEC.ino" 2
};


//Predefine Functions
void checkIaqSensorStatus(void);
void errLeds(void);
void loadState(void);
void updateState(void);
void drawLCD(uint16_t rotation);



// Create an object of the class Bsec
Bsec iaqSensor;
uint8_t bsecState[(139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/] = {0};
uint16_t stateUpdateCounter = 0;




void setup() {
  //Set pins
  pinMode(5, 0x00);
  pinMode(4, 0x00);
  pinMode(14, 0x01);
  pinMode(12, 0x00);
  pinMode(13, 0x00);
  pinMode(18, 0x01);

  ShiftExpander::setPins(5, 4, 16);
  Serial.begin(500000);
  SPI.begin();

  //Initialize BSEC
  EEPROM.begin((139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/ + 1); // 1st address for the length
  iaqSensor.begin(18, SPI); //Pin 2 will be the CS pin

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

  iaqSensor.updateSubscription(sensorList, 7, (0.33333f) /*!< Sample rate in case of Low Power Mode */);
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

  if (iaqSensor.bme680Status != 0) {
    if (iaqSensor.bme680Status < 0) {
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
  pinMode(0, 0x01);
  digitalWrite(0, 0x1);
  delay(100);
  digitalWrite(0, 0x0);
  delay(100);
}

void loadState(void)
{
  if (EEPROM.read(0) == (139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/) {
    // Existing state in EEPROM
    Serial.println("Reading state from EEPROM");

    for (uint8_t i = 0; i < (139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/; i++) {
      bsecState[i] = EEPROM.read(i + 1);
      Serial.println(bsecState[i], 16);
    }

    iaqSensor.setState(bsecState);
    checkIaqSensorStatus();
  } else {
    // Erase the EEPROM with zeroes
    Serial.println("Erasing EEPROM");

    for (uint8_t i = 0; i < (139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/ + 1; i++)
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
    if ((stateUpdateCounter * 360 * 60 * 1000U /* 360 minutes - 4 times a day*/) < millis()) {
      update = true;
      stateUpdateCounter++;
    }
  }

  if (update) {
    iaqSensor.getState(bsecState);
    checkIaqSensorStatus();

    Serial.println("Writing state to EEPROM");

    for (uint8_t i = 0; i < (139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/ ; i++) {
      EEPROM.write(i + 1, bsecState[i]);
      Serial.println(bsecState[i], 16);
    }

    EEPROM.write(0, (139) /*!< Maximum size (in bytes) of the data blobs returned by bsec_get_state()*/);
    EEPROM.commit();
  }
}
