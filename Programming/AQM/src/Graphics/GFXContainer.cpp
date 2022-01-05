#include "GFXContainer.h"
//TODO: Change textualInfo to linkedlist instead of vectors.

/*=========================================== Utilities ===========================================*/

TextualInfo splitTextualInfo(uint16_t position, TextualInfo * line, std::function<bool()> yieldingFunc);

/** Breaks a piece of textual info on a line break and returns the broken-off piece*/
TextualInfo breakTextualInfoOnLineBreak(TextualInfo * textualInfo, std::function<bool()> yieldingFunc);



/*=========================================== TextualInfo ===========================================*/
TextualInfo::TextualInfo() : textRef(NULL), text(""), color(0), size(0), x(0), y(0), isDonePrerendering(false) {
    Serial.println("Default TextualInfo Constructor Called");
}
TextualInfo::TextualInfo(String * text0, uint16_t color0, uint16_t size0) : textRef(text0), text(*text0), color(color0), size(size0), x(0), y(0), isDonePrerendering(false) {
    Serial.printf("Parameterized constructor called for %s\n", text);
}
TextualInfo::TextualInfo(const String text0, const uint16_t color0, const uint16_t size0)
{

    text = text0; //todo: check if destructor is called in between this and the bit in setup(). Trace the flow of the variable.
    textRef = (&text);
    color = color0;
    size = size0;


}
TextualInfo::TextualInfo(const char * text0, const uint16_t color0, const uint16_t size0)
{

    text = String(text0);
    textRef = &text;
    color = color0;
    size = size0;
}

TextualInfo::TextualInfo(const TextualInfo & other)
{
    this->text = other.text;
    if (other.textRef == &other.text)
    {
        this->textRef = &text;
    }
    else
    {
        this->textRef = other.textRef;
    }

    this->color = other.color;
    this->size = other.size;

}

//Move 
/*
TextualInfo::TextualInfo(const TextualInfo && other)
{
    this->text = other.text;
    if (other.textRef == &other.text)
    {
        this->textRef = &text;
    }
    else
    {
        this->textRef = other.textRef;
    }

    this->color = other.color;
    this->size = other.size;

    other.text = "";
    other.color = 0;
    other.size = 0;
    other.textRef = NULL;
    Serial.printf("Move constructor called for %s\n", this->text);
}*/



TextualInfo::~TextualInfo()
{
    text = "";
    textRef = NULL;
    color = 0;
    size = 0;
}


TextualInfo & TextualInfo::operator=(const TextualInfo & other)
{
    if (this != &other)
    {
        if (other.textRef == &other.text)
        {
            this->text = other.text;
            this->textRef = &this->text;
        }
        else
        {
            this->textRef = other.textRef;
            this->text = *(this->textRef);
        }

        this->color = other.color;
        this->size = other.size;

    }
    return *this;
}


void TextualInfo::setPos(uint16_t x0, uint16_t y0) {
    x = x0;
    y = y0;
    isDonePrerendering = true;
}

bool TextualInfo::isPrerendered() {
    return isDonePrerendering;
}

uint16_t TextualInfo::getX() {
    return x;
}

uint16_t TextualInfo::getY() {
    return y;
}

void TextualInfo::setX(uint16_t x0) {
    x = x0;
}

void TextualInfo::setY(uint16_t y0) {
    y = y0;
}


/*=========================================== GFXContainer ===========================================*/
GFXContainer::GFXContainer() : color(0), isActive(false)
{
    data = std::vector<TextualInfo>();
    prerender = callback = [](GFXContainer & ref){};
    yieldingFunc = []{return false;};

    movementFunc = [](int8_t move){return false;};
    margin = 6;

    Serial.printf("Are the things nullpointers? %d %d %d %d\n", prev == nullptr, next == nullptr, up == nullptr, down == nullptr);
}

GFXContainer::GFXContainer(std::function<bool()> yieldingFunc0) : color(0), isActive(false)
{
    data = std::vector<TextualInfo>();
    prerender = callback = [](GFXContainer & ref){};
    yieldingFunc = yieldingFunc0;
    movementFunc = [](int8_t move){return false;};
    margin = 10;
}

GFXContainer::GFXContainer(std::shared_ptr<GFXContainer> next0, std::shared_ptr<GFXContainer> prev0, std::shared_ptr<GFXContainer> up0, std::shared_ptr<GFXContainer> down0, 
    uint16_t color0, uint8_t margin0)
    : next(next0), prev(prev0), up(up0), down(down0), color(color0), margin(margin0), isActive(false)
{
    data = std::vector<TextualInfo>();
    prerender = callback = [](GFXContainer & ref){};
    yieldingFunc = []{return false;};
    movementFunc = [](int8_t move){return false;};
}

/*================== Setters =================*/
GFXContainer & GFXContainer::setNext(std::shared_ptr<GFXContainer> next0) {
    next = next0;
    Serial.printf("Setting next! %d\n", next == self);
    if (!next->prev) {
        next->setPrev(self);
        if (next->self == nullptr)
        {
            next->setSelf(next);
        }
    }
    Serial.printf("Are prev and next nullptr? %d %d\n", next->prev == nullptr, next->next == nullptr);
        
    
    return *this;
}


GFXContainer & GFXContainer::setPrev(std::shared_ptr<GFXContainer> prev0) {
    prev = prev0;
    Serial.printf("Setting prev! %d\n", prev == self);
    if (!prev->next) {
        prev->setNext(self);
        if (prev->self == nullptr)
        {
            prev->setSelf(prev);
        }
    }

    Serial.printf("Are prev and next nullptr? %d %d\n", prev->prev == nullptr, prev->next == nullptr);

    return *this;
}

GFXContainer & GFXContainer::setUp(std::shared_ptr<GFXContainer> up0) {
    up = up0;
    Serial.printf("Setting up! %d\n", up == self);
    if (!up->down)
    {
        up->setDown(self);
        if (up->self == nullptr)
        {
            up->setSelf(up);
        }
    }
    Serial.printf("Are up and down nullptr? %d %d\n", up->up == nullptr, up->down == nullptr);
    return *this;
}

GFXContainer & GFXContainer::setDown(std::shared_ptr<GFXContainer> down0) {
    down = down0;
    Serial.printf("Setting down! %d\n", down == self);
    if (!down->up) {
        down->setUp(self);
        if (down->self == nullptr)
        {
            down->setSelf(down);
        }
    }
    Serial.printf("Are up and down nullptr? %d %d\n", down->up == nullptr, down->down == nullptr);

    return *this;
}

void GFXContainer::setData(std::vector<TextualInfo> data0) {
    data = data0;
}

GFXContainer & GFXContainer::setPrerender(std::function<void(GFXContainer &)> prerender0) {
    Serial.println("Setting prerender");
    prerender = prerender0;
    Serial.println("Done setting prerender");

    return *this;
}

GFXContainer & GFXContainer::setCallback(std::function<void(GFXContainer &)> callback0) {
    callback = callback0;

    return *this;
}



GFXContainer & GFXContainer::setColor(uint16_t color0) {
    color = color0;
    return *this;
}


GFXContainer & GFXContainer::addData(TextualInfo data0) {
    Serial.println("Adding data!");
    data.push_back(data0);
    Serial.println("Done adding data!");
    return *this;
}

GFXContainer & GFXContainer::setMargin(uint8_t margin0) {
    margin = margin0;
    return *this;
}

GFXContainer & GFXContainer::setInterruptingFunction(std::function<bool()> yieldingFunc0) {
    Serial.println("Setting interrupting function");
    yieldingFunc = yieldingFunc0;
    Serial.println("Done");
    return *this;
}

GFXContainer & GFXContainer::setMovementInterruptingFunction(std::function<bool(int8_t)> movementFunc0) {
    movementFunc = movementFunc0;
    return *this;
}

void GFXContainer::enter(int8_t direction) {
    //Draw background

    isActive = true;
    clear(direction);
    /*
    GFX::tft.fillScreen(color);
    //Draw nav arrows;
    if (up != NULL)
    {
        GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, margin, TFT_WIDTH / 2 + margin, margin, TFT_WIDTH / 2, 0, ~color); //TODO: optimize
    }
    if (down != NULL)
    {
        GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2 + margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2, TFT_HEIGHT - 1, ~color); //Pixels are zero-indexed
    }
    if (prev != NULL)
    {
        GFX::tft.fillTriangle(margin, TFT_HEIGHT / 2 - margin, margin, TFT_HEIGHT / 2 + margin, 0, TFT_HEIGHT / 2, ~color);
    }
    if (next != NULL)
    {
        GFX::tft.fillTriangle(TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 - margin, TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 + margin, TFT_WIDTH - 1, TFT_HEIGHT / 2, ~color);
    }*/

    //Prerender positions
    Serial.println("Prerendering data");
    uint16_t totalHeight = 0;
    //first, look through the data. If there are lines that contain "\n", split it.
    
    for (int count = 0; count < data.size(); count++)
    {
        yieldingFunc();
        
        TextualInfo newTextualInfo = breakTextualInfoOnLineBreak(&data[count], yieldingFunc);
        yieldingFunc();
        if (newTextualInfo.text != String("This is a null textual info please ignore"))
        {
            data.insert(data.begin() + count + 1, newTextualInfo); //the error occurs somewhere in this bit
        }
    }
    

    /* TODO: Complete QOL line-breaking
    for (int count = 0; count < data.size(); count++)
    {        
        TextualInfo & line = data[count];
        //Properly break text lines for wrapping if needed
            //If the line is too long, just search from the beginning the number of words possible, break the line, then continue with the next line.
        GFX::canvas.setTextSize(line.size);

        //find the location of the first break
        uint16_t maxWidth = GFX::tft.width() - margin * 2;
        int16_t x1, y1 = 0;
        uint16_t w, h = 0;

        GFX::canvas.getTextBounds(*(line.getText()), 0, 0, &x1, &y1, &w, &h);

        //begin the process of breaking down strings
        if (w > maxWidth)
        {
            String refStr = *line.getText();
            String newStr = String("");
            
            uint16_t maxCharIndex = refStr.length();
            uint16_t wordIndex = 0;

            uint16_t charIndexAtLastWord = 0;
            
            for (int charIndex = 0; charIndex < maxCharIndex; charIndex++)
            {
                if (refStr(charIndex) == ' ')
            }

        }
    }
    */

    //Find the height of all the text on the page
    for (int count = 0; count < data.size(); count++)
    {
        yieldingFunc();

        TextualInfo & line = data[count];
        int16_t x1, y1 = 0;
        uint16_t w, h = 0;
        
        GFX::canvas.setTextSize(line.size);
        GFX::canvas.getTextBounds(*(line.textRef), 0, 0, &x1, &y1, &w, &h);
        yieldingFunc();
        if (h == 0 || *(line.textRef) == String("")) //if a zero-height string
        {
            GFX::canvas.getTextBounds(String("PH"), 0, 0, &x1, &y1, &w, &h);
            yieldingFunc();
        }

        totalHeight += h;
        line.setX(uint16_t((TFT_WIDTH / 2.0) - (w/2.0)));
        line.setY(h); //we'll just use this as a placeholder for height for now 
    }
    
    int overallYOffset = (TFT_HEIGHT / 2.0) - (totalHeight / 2.0);
    //set the positions
    for (int index = 0; index < data.size(); index++)
    {
        yieldingFunc();
        TextualInfo & line = data[index];
        int tempOverallYOffset = overallYOffset;
        overallYOffset += line.getY(); //retrieve the placeholder height
        line.setPos(line.getX(), tempOverallYOffset);
    }
    yieldingFunc();
    //Find the proper positions of each text piece
    //Only y-positions can be prerendered if
}

void GFXContainer::exit() {
    isActive = false;
}

void GFXContainer::render() {

    if (yieldingFunc()) return;
    prerender(*this);
    if (yieldingFunc()) return;

    if (!isActive)
        enter(2);

    if (yieldingFunc()) return;

    
    //Serial.println("Finding x positions");
    //if the data is pre-rendered, just update x positions in case a piece of data has changed.
    for (int index = 0; index < data.size(); index++)
    {
        yieldingFunc();
        TextualInfo & line = data[index];
        int16_t x1, y1 = 0;
        uint16_t w, h = 0;
        
        GFX::canvas.setTextSize(line.size);
        GFX::canvas.getTextBounds(*(line.textRef), 0, 0, &x1, &y1, &w, &h);
        line.setX(uint16_t((TFT_WIDTH / 2.0) - (w/2.0)));
    }
    

    //write in data
    for (TextualInfo & datum : data) {
        GFX::tft.setCursor(datum.getX(), datum.getY());
        GFX::tft.setTextColor(datum.color);
        GFX::tft.setTextSize(datum.size);
        
        static const int MAX_FRAC = 20;
        for (int frac = 0; frac < MAX_FRAC; frac++)
        {
            if (yieldingFunc()) return;
            String str = *(datum.textRef);
            if (frac < MAX_FRAC)
            {
                GFX::tft.print(str.substring(frac * str.length() / MAX_FRAC, ((frac + 1) * str.length())/MAX_FRAC));
            }
            else
            {
                GFX::tft.print((*(datum.textRef)).substring((frac * str.length())/MAX_FRAC));
            }
            
        }
    }

    yieldingFunc();
    callback(*this);
}

GFXContainer & GFXContainer::clear(int8_t direction) { //if direction is -1, clear from right to left. If direction is 1, clear from ltr. If direction is 0, clear from centre vertically, and redraw arrows.
    //redraw the background in chunks
    switch(direction)
    {
        case 1: {
            const uint16_t x0 = 0;
            const uint16_t x1 = TFT_WIDTH/2 - margin;
            const uint16_t x2 = TFT_WIDTH - margin - 1;

            for (int x = TFT_WIDTH - 1; x >= 0; x--)
            {
                if (x % 20 == 0)
                {
                    if (yieldingFunc()) return *this;
                }

                GFX::tft.drawFastVLine(x, 0, TFT_HEIGHT, this->color);

                //Draw nav arrows
                if (x == x0 &&prev != NULL)
                {
                    GFX::tft.fillTriangle(margin, TFT_HEIGHT / 2 - margin, margin, TFT_HEIGHT / 2 + margin, 0, TFT_HEIGHT / 2, ~color);
                }
                else if (x == x1)
                {
                    if (up != NULL)
                    {
                        GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, margin, TFT_WIDTH / 2 + margin, margin, TFT_WIDTH / 2, 0, ~color); //TODO: optimize
                    }
                    if (down != NULL)
                    {
                        GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2 + margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2, TFT_HEIGHT - 1, ~color); //Pixels are zero-indexed
                    }
                }
                else if (x == x2 && next != NULL)
                {
                    GFX::tft.fillTriangle(TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 - margin, TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 + margin, TFT_WIDTH - 1, TFT_HEIGHT / 2, ~color);
                }
            }
        }
        break;
        
        case -1: {
            const uint16_t x0 = margin;
            const uint16_t x1 = TFT_WIDTH/2.0 + margin;
            const uint16_t x2 = TFT_WIDTH-1;

            for (int x = 0; x < TFT_WIDTH; x++)
            {
                if (x % 20 == 0)
                    if (yieldingFunc()) return *this;

                GFX::tft.drawFastVLine(x, 0, TFT_HEIGHT, this->color);

                //Draw nav arrows
                if (x == x0 &&prev != NULL)
                {
                    GFX::tft.fillTriangle(margin, TFT_HEIGHT / 2 - margin, margin, TFT_HEIGHT / 2 + margin, 0, TFT_HEIGHT / 2, ~color);
                }
                else if (x == x1)
                {
                    if (up != NULL)
                    {
                        GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, margin, TFT_WIDTH / 2 + margin, margin, TFT_WIDTH / 2, 0, ~color); //TODO: optimize
                    }
                    if (down != NULL)
                    {
                        GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2 + margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2, TFT_HEIGHT - 1, ~color); //Pixels are zero-indexed
                    }
                }
                else if (x == x2 && next != NULL)
                {
                    GFX::tft.fillTriangle(TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 - margin, TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 + margin, TFT_WIDTH - 1, TFT_HEIGHT / 2, ~color);
                }
            }
        }
            break;

        case 2: {
            const uint16_t y0 = margin;
            const uint16_t y1 = TFT_HEIGHT/2.0 + margin;
            const uint16_t y2 = TFT_HEIGHT-1;

            for (int y = 0; y < TFT_HEIGHT; y++)
            {
                if (y % 20 == 0)
                    if (yieldingFunc()) return *this;

                GFX::tft.drawFastHLine(0, y, TFT_WIDTH, this->color);

                //Draw nav arrows
                if (y == y0 && up != NULL)
                {
                    GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, margin, TFT_WIDTH / 2 + margin, margin, TFT_WIDTH / 2, 0, ~color);
                }
                else if (y == y1)
                {
                    if (prev != NULL)
                    {
                        GFX::tft.fillTriangle(margin, TFT_HEIGHT / 2 - margin, margin, TFT_HEIGHT / 2 + margin, 0, TFT_HEIGHT / 2, ~color);//TODO: optimize
                    }
                    if (next != NULL)
                    {
                        GFX::tft.fillTriangle(TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 - margin, TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 + margin, TFT_WIDTH - 1, TFT_HEIGHT / 2, ~color);
                         //Pixels are zero-indexed
                    }
                }
                else if (y == y2 && down != NULL)
                {
                    GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2 + margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2, TFT_HEIGHT - 1, ~color);
                }
            }
        } break;

        case -2: {
            const uint16_t y0 = 0;
            const uint16_t y1 = TFT_HEIGHT/2 - margin;
            const uint16_t y2 = TFT_HEIGHT - margin - 1;

            for (int y = TFT_HEIGHT - 1; y >= 0; y--)
            {
                if (y % 20 == 0)
                    if (yieldingFunc()) return *this;

                GFX::tft.drawFastHLine(0, y, TFT_WIDTH, this->color);

                //Draw nav arrows
                if (y == y0 && up != NULL)
                {
                    GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, margin, TFT_WIDTH / 2 + margin, margin, TFT_WIDTH / 2, 0, ~color);
                }
                else if (y == y1)
                {
                    if (prev != NULL)
                    {
                        GFX::tft.fillTriangle(margin, TFT_HEIGHT / 2 - margin, margin, TFT_HEIGHT / 2 + margin, 0, TFT_HEIGHT / 2, ~color);//TODO: optimize
                    }
                    if (next != NULL)
                    {
                        GFX::tft.fillTriangle(TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 - margin, TFT_WIDTH - margin - 1, TFT_HEIGHT / 2 + margin, TFT_WIDTH - 1, TFT_HEIGHT / 2, ~color);
                         //Pixels are zero-indexed
                    }
                }
                else if (y == y2 && down != NULL)
                {
                    GFX::tft.fillTriangle(TFT_WIDTH / 2 - margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2 + margin, TFT_HEIGHT - margin - 1, TFT_WIDTH / 2, TFT_HEIGHT - 1, ~color);
                }
            }    
        } 
        break;
    }

    drawBattery();

    return *this;
}

//unprint whatever's been printed before
GFXContainer & GFXContainer::unprint() {
    Serial.println("We're unprinting");
    for (TextualInfo & datum : data) {
        GFX::tft.setCursor(datum.getX(), datum.getY());
        GFX::tft.setTextColor(this->color, this->color);
        GFX::tft.setTextSize(datum.size);
        
        static const int MAX_FRAC = 20;
        for (int frac = 0; frac < MAX_FRAC; frac++)
        {
            if (yieldingFunc()) return *this;
            String str = *(datum.textRef);
            if (frac < MAX_FRAC)
            {
                GFX::tft.print(str.substring(frac * str.length() / MAX_FRAC, ((frac + 1) * str.length())/MAX_FRAC));
            }
            else
            {
                GFX::tft.print((*(datum.textRef)).substring((frac * str.length())/MAX_FRAC));
            }
            
        }
    }
    return *this;
}

void GFXContainer::setBattery(int batteryPercent0)
{
    Serial.println("Battery setting");
    if (batteryPercent0 != GFXContainer::batteryPercent) 
    {
        GFXContainer::batteryPercent = batteryPercent0;
        drawBattery();
    }
}

void GFXContainer::drawBattery() {
    static Adafruit_ST7735 & tft = GFX::tft;
    static GFXcanvas1 & canvas = GFX::canvas;
    static const int TEXT_Y_OFFSET = 4;
    static const int TEXT_X_OFFSET = 4;
    static const int ICON_WIDTH = 25;
    
    String percent(batteryPercent);

    int16_t x, y;
    uint16_t w, h;
    canvas.setTextSize(1);
    canvas.setCursor(0,0);
    canvas.getTextBounds(percent, 0, 0, &x, &y, &w, &h);

    GFX::tft.drawRect(TFT_WIDTH - ICON_WIDTH - 2, ((h + 8) / 2) - 2, 4, 4, BLACK);
    GFX::tft.fillRect(TFT_WIDTH - ICON_WIDTH - 2, ((h + 8) / 2) - 2, 4, 4, LIME);
    GFX::tft.fillRoundRect(TFT_WIDTH - ICON_WIDTH, 0, ICON_WIDTH, h + 8, 2, LIME);
    GFX::tft.drawRoundRect(TFT_WIDTH - ICON_WIDTH, 0, ICON_WIDTH, h + 8, 2, BLACK);



    GFX::tft.setTextSize(1);
    GFX::tft.setTextColor(WHITE);
    GFX::tft.setCursor(TFT_WIDTH - TEXT_X_OFFSET - w, TEXT_Y_OFFSET);
    GFX::tft.print(batteryPercent);
}


int GFXContainer::batteryPercent = 100;

/*=========================================== UTILITIES ===========================================*/

TextualInfo splitTextualInfo(uint16_t position, TextualInfo * line, std::function<bool()> yieldingFunc)
{

    String text = line->text;
    //new string
    yieldingFunc();
    String newText = String(text.substring(position + 1, text.length()));
    yieldingFunc();
    TextualInfo newLine(newText, line->color, line->size);

    String newText1 = (line->text).substring(0,position);
    line->text = newText1;//(line->text).substring(0, position);
    line->textRef = &(line->text);
    yieldingFunc();

    Serial.printf("Splitting textual info: Old textual info: %s, New Textual Info: %s\n\n", text.c_str(), newLine.text.c_str());
    

    Serial.printf("Free memory: %d\n", ESP.getFreeHeap());

    return newLine;
}

/** Breaks a piece of textual info on a line break and returns the broken-off piece*/

TextualInfo breakTextualInfoOnLineBreak(TextualInfo * textualInfo, std::function<bool()> yieldingFunc)
{
    TextualInfo & line = *(textualInfo); //we made the param a pointer to make it apparent that it will be changed.
    uint16_t maxCharLen = (*(line.textRef)).length();
    char * refStr = new char[maxCharLen + 1];
    refStr[maxCharLen] = '\0';
    (*(line.textRef)).toCharArray(refStr, maxCharLen + 1);
    yieldingFunc();

    uint16_t charIndex = 0;
    

    char * lineBreak = std::strchr(refStr, '\n');
    yieldingFunc();

    if (lineBreak != nullptr)
    {
        std::ptrdiff_t index =  lineBreak - &refStr[0];

        TextualInfo newTextualInfo = splitTextualInfo((uint16_t) index, &line, yieldingFunc);
        yieldingFunc();

        delete[] refStr;
        return newTextualInfo;
    }
    delete[] refStr;

    
    return TextualInfo(String("This is a null textual info please ignore"), 0, 0);
}
