#ifndef PINOUT_CONSTANTS_H
#define PINOUT_CONSTANTS_H

enum UIAction {
    FWD = 1,
    BUTTON = 0,
    BWD = -1,
};

/**========================================================================
 *                                  PINS
 *========================================================================**/

/*================== SPI =================*/
#define AQM_SCK 14
#define AQM_SDO 13
#define AQM_SDI 12

/*================== ShiftExpander =================*/
#define AQM_SR_SER 5
#define AQM_SR_SRCLK 2
#define AQM_SR_RCLK 0


/*================== BSEC =================*/
#define CS_BSEC 20 //Expanded pin

/*================== Rotary Encoder =================*/
#define RE_PIN_A 4
#define RE_PIN_B 16
#define RE_BUTTON 15

/*================== ST7735 LCD TFT =================*/
#define CS_TFT 19
#define DC_TFT 18
#define RST_TFT -1

//todo  Add in dimming capabilities 

/*================== BUZZER =================*/
//todo  Add in buzzer  





#endif