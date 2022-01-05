/**
 * @file ClimateMonitor.hxx
 * @author Darton Li
 * @brief 
 * @version 0.1
 * @date 2021-12-24
 * 
 * Header file for climate monitor module
 * 
 */

/**========================================================================
 *                           Includes
 *========================================================================**/
#include "pins.h"
#include <bsec_SE.h>
#include "Arduino.h"
#include <SPI.h>

/*============================ END OF SECTION ============================*/



namespace CM {

/**========================================================================
 *                           Constants
 *========================================================================**/
    #define STATE_SAVE_PERIOD  UINT32_C(360 * 60 * 1000) // 360 minutes - 4 times a day
    extern const uint8_t bsec_config_iaq[];

/*============================ END OF SECTION ============================*/




/**========================================================================
 *                           Operating Variables
 *========================================================================**/
    extern Bsec iaqSensor; //Sensor object
    extern uint8_t bsecState[BSEC_MAX_STATE_BLOB_SIZE]; //Historical States
    extern uint32_t stateUpdateCounter; //How many times we've updated the state
/*============================ END OF SECTION ============================*/




/**========================================================================
 *                           Functions
 *========================================================================**/
    
    /**==============================================
     **              checkIaqSensorStatus
     *?  Checks if the BME680 sensor is doing okay
     *@param name type  
     *@return void
     *=============================================**/ 
    void checkIaqSensorStatus();
    

    /**==============================================
     **              errLeds
     *?  Indicates an error has occurred by flashing 
     *the built-in LED
     *
     *todo: Make this edit a flag to show the graphics 
     *module that an error has occurred
     *@param name type  
     *@param name type  
     *@return type
     *=============================================**/
    void errLeds();
    
    /*
    EEPROM Functions removed for longevity of flash memory

    /**==============================================
     **              loadState
     *?   Load in old sensor data 
     *?   or setup the EEPROM storage to load in 
     *?   old sensor data 
     *@param name type  
     *@param name type  
     *@return type
     *=============================================
    void loadState();
    
    void updateState();
    */


    /**==============================================
     **              init
     *?  Function run once on startup to setup bsec.
     *?  Has to be run after SPI.begin() and ShiftExpander.setPins();
     *=============================================**/
    void init();

    /**==============================================
     **              loop
     *?  Function run every loop to update bsec
     *=============================================**/    
    void loop();


/*============================ END OF SECTION ============================*/
}

