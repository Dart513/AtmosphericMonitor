/**=======================================================================================================================
 * ?                                                     ABOUT
 * @author         :  Darton Li
 * @email          :  
 * @repo           :  
 * @createdOn      :  2021-12-24
 * @description    :  Graphics for AQM
 *=======================================================================================================================**/


#ifndef AQM_GRAPHICS_H
#define AQM_GRAPHICS_H

/*================================ Includes ==============================*/
#include <Arduino.h>
#include <Adafruit_GFX_SE.h>
#include <Adafruit_ST7735_SE.h>
#include "../pins.h"


/*================================ Symbols & Constants ==============================*/

#define DEG_SYMBOL (char)(223)
#define OMEGA String("KOhm")
#define KPA String("KPA")
#define ATM String("atm")
#define DEG_C String(" C")
#define PERCENT String('%')
#define PPM String("PPM")
#define PI 3.14159265358979323846;

namespace GFX {
    extern Adafruit_ST7735 tft;
    extern GFXcanvas1 canvas; //offscreen bitmap for figuring out sizes
}

extern uint16_t TFT_WIDTH;
extern uint16_t TFT_HEIGHT;

/*================== Colors =================*/
extern const uint16_t WHITE;
extern const uint16_t LIGHT_BLUE;
extern const uint16_t LIGHT_GREEN;
extern const uint16_t ORANGE;
extern const uint16_t LIME;
extern const uint16_t MUTED_CYAN;
extern const uint16_t DARK_GREY;
extern const uint16_t BLACK;
extern const uint16_t RED;

namespace GFX {
/*================================ FUNCTIONS ==============================*/

    /**========================================================================
     **                           RGB24_TO_RGB565
    *?  Converts a 24-bit RGB color to a 16-bit RGB565 color. This function also pretends to be a macro.
    *?  Someone has to balance out the whole bit where the macro printf pretends to
    *?  be a function.
    *@param value uint64_t  
    *@return uint16_t
    *========================================================================**/
    uint16_t RGB24_TO_RGB565(uint64_t value);



    /**========================================================================
     **                           init
    *?  Initializes graphics
    *========================================================================**/
    void init();


    /**========================================================================
     **                           drawText
     *? Draws text in a location with a specified length and color
     * @param text The text to draw String
     * @param textSize the size of text in pts uint8_t
     * @param x the x coordinate to begin the drawing uint16_t
     * @param y the y coordinate to begin the drawing uint16_t
     * @param color the color for the text  uint16_t
     *========================================================================**/
    void drawText(String text, uint8_t textSize, uint16_t x, uint16_t y, uint16_t color);


    /**========================================================================
     **                           drawTextCentered
     *?  Draws an overview-styles screen with the specified
     *@param text String the text to display
     *@param textSize uint16_t the size of the text in pts
     *@param x uint16_t the x coordinate to begin the drawing
     *@param y uint16_t the y coordinate to begin the drawing
     *@param color uint16_t the color for the text  
     *========================================================================**/
    void drawTextCentered(String text, uint16_t textSize, uint16_t x, uint16_t y, uint16_t color);

}

#endif