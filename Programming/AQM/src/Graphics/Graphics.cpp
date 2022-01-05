#include "Graphics.h"



Adafruit_ST7735 GFX::tft = Adafruit_ST7735(CS_TFT, DC_TFT, RST_TFT);
GFXcanvas1 GFX::canvas = GFXcanvas1(GFX::tft.width(), GFX::tft.height());

uint16_t TFT_WIDTH;
uint16_t TFT_HEIGHT;


/*================== Colors =================*/
const uint16_t WHITE = GFX::RGB24_TO_RGB565(0x00FFFFFF);
const uint16_t LIGHT_BLUE = GFX::RGB24_TO_RGB565(0x0099CCFF);
const uint16_t LIGHT_GREEN = GFX::RGB24_TO_RGB565(0x0090EE90);
const uint16_t ORANGE = GFX::RGB24_TO_RGB565(0x00FF9900);
const uint16_t LIME = GFX::RGB24_TO_RGB565(0x0032FF32);
const uint16_t MUTED_CYAN = GFX::RGB24_TO_RGB565(0x00AAFFFF);
const uint16_t DARK_GREY = GFX::RGB24_TO_RGB565(0x00808080);
const uint16_t BLACK = GFX::RGB24_TO_RGB565(0x00000000);
const uint16_t RED = GFX::RGB24_TO_RGB565(0x00FF0000);


/*================================ FUNCTIONS ==============================*/
uint16_t GFX::RGB24_TO_RGB565(uint64_t value) {
  //value = value << 16;
  return (((uint16_t)((value >> 32) / 8) << 11) | ((uint16_t)((value << 16 >> 48) / 4) << 5) | (uint16_t)((value << 32 >> 32) / 8));
  //Serial.println('R' + String((value >> 32)) + 'G' + String(value << 16 >> 48) + 'B' + String(value << 32 >> 32));
}


void GFX::init() {
    pinMode(CS_TFT, OUTPUT);
    pinMode(DC_TFT, OUTPUT);

    tft.setSPISpeed(40000000);//40MHz
    tft.initR(INITR_144GREENTAB); //TODO: CHANGE IF THE LCD CHANGES
    tft.setRotation(0); //Landscape left
    tft.cp437(true); //turn on the correct charset
    tft.fillScreen(BLACK); //clear the screen

    TFT_WIDTH = GFX::tft.width();
    TFT_HEIGHT = GFX::tft.height();
    Serial.printf("GFX: Initialized. Width: %d  Height: %d\n", tft.width(), tft.height());
}

/*================================ GFX Functions ==============================*/
void GFX::drawText(String text, uint8_t textSize, uint16_t x, uint16_t y, uint16_t color) {
    tft.setCursor(x, y);
    tft.setTextSize(textSize);
    tft.setTextColor(color);
    tft.setTextWrap(true);
    tft.print(text);
}

/**========================================================================
 **                           drawTextCentered
    *?  Draws an overview-styles screen with the specified
    *@param text String the text to display
    *@param textSize uint16_t the size of the text in pts
    *@param x uint16_t the x coordinate to begin the drawing
    *@param y uint16_t the y coordinate to begin the drawing
    *@param color uint16_t the color for the text  
    *========================================================================**/
void GFX::drawTextCentered(String text, uint16_t textSize, uint16_t x, uint16_t y, uint16_t color) {
    static int16_t x1, y1;
    static uint16_t w, h;
    tft.setTextSize(textSize);
    tft.setTextWrap(false);
    tft.getTextBounds(text, x, y, &x1, &y1, &w, &h);

    tft.setCursor(x - w/2, y - h/2);
    tft.setTextColor(color);
    tft.print(text);

    Serial.printf("GFX: drawTextCentered text: %s x: %d y: %d\n", text.c_str(), x, y);
}

