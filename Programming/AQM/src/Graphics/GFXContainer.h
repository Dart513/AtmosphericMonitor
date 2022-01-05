#ifndef GFX_CONTAINER_H
#define GFX_CONTAINER_H

#include <Arduino.h>
#include "Graphics.h"
#include <vector>
#include <functional>
#include <memory>
#include "../pins.h"


//A wrapper to a string to be able to use it with TextualInfos

class TextualInfo {
    private:
        uint16_t x, y; // default to 0 before being set upon completion of pre-rendering;
        bool isDonePrerendering;

    public:
    String text;
    String * textRef; //Pointer to textual data
    uint16_t color;
    uint16_t size;

    //Default constructor
    TextualInfo();

    //Copy constructor
    TextualInfo(const TextualInfo & other);

    //Move constructor
    //TextualInfo(TextualInfo && other);

    //Data constructor
    TextualInfo(String * text0, uint16_t color0, uint16_t size0);

    //Data constructor 2
    TextualInfo(const String text0, const uint16_t color0, const uint16_t size0);

    //Data constructor 3
    TextualInfo(const char * text0, const uint16_t color0, const uint16_t size0);

    //Destructor
    ~TextualInfo();


    TextualInfo & operator=(const TextualInfo & other);


    bool isPrerendered();

    //finishes prerendering
    void setPos(uint16_t x0, uint16_t y0);

    uint16_t getX();
    uint16_t getY();

    void setX(uint16_t x0);
    void setY(uint16_t y0);
};


class GFXContainer {
    private:
    std::shared_ptr<GFXContainer> self;
    //Links
    std::shared_ptr<GFXContainer> next;
    std::shared_ptr<GFXContainer> prev;
    std::shared_ptr<GFXContainer> up;
    std::shared_ptr<GFXContainer> down;
    
    //Contents
    std::vector<TextualInfo> data;

    std::function<void(GFXContainer &)> prerender;
    std::function<void(GFXContainer &)> callback;

    //interrupting function, if true, return from function
    std::function<bool()> yieldingFunc;

    //movement interruption
    std::function<bool(int8_t)> movementFunc;

    //Attributes
    uint16_t color;

    static int batteryPercent;

    bool isActive;

    public:
    uint8_t margin;
    //Default constructor
    GFXContainer();

    GFXContainer(std::function<bool()> yieldingFunc0);

    //Data constructor
    GFXContainer(std::shared_ptr<GFXContainer> next0, std::shared_ptr<GFXContainer> prev0, std::shared_ptr<GFXContainer> up0, 
        std::shared_ptr<GFXContainer> down0, uint16_t color0, uint8_t margin0);
    

    //Copying is not intended, so we'll TODO: add later

    //setters
    GFXContainer & setNext(std::shared_ptr<GFXContainer> next0);
    GFXContainer & setPrev(std::shared_ptr<GFXContainer> prev0);
    GFXContainer & setUp(std::shared_ptr<GFXContainer> up0);
    GFXContainer & setDown(std::shared_ptr<GFXContainer> down0);

    GFXContainer & setNext(GFXContainer * next0) {
        return setNext(std::shared_ptr<GFXContainer>(next0));
    }
    GFXContainer & setPrev(GFXContainer * prev0) {
        return setPrev(std::shared_ptr<GFXContainer>(prev0));
    }
    GFXContainer & setUp(GFXContainer * up0) {
        return setUp(std::shared_ptr<GFXContainer>(up0));
    }
    GFXContainer & setDown(GFXContainer * down0){
        return setDown(std::shared_ptr<GFXContainer>(down0));
    }

    GFXContainer & setColor(uint16_t color0);
    GFXContainer & setMargin(uint8_t margin0);

    GFXContainer & setInterruptingFunction(std::function<bool()> yieldingFunc0);
    GFXContainer & setMovementInterruptingFunction(std::function<bool(int8_t)> movementFunc0);

    void setData(std::vector<TextualInfo> data0);

    GFXContainer & setPrerender(std::function<void(GFXContainer &)> prerender0);
    GFXContainer & setCallback(std::function<void(GFXContainer &)> callback0);
    GFXContainer & setSelf(std::shared_ptr<GFXContainer> self0) {
        self = self0;
        return *this;
    }

    GFXContainer & addData(TextualInfo data0);

    //functions

    //Things to draw once on entry
    void enter(int8_t direction);

    void exit();

    //Things to draw each loop
    void render();

    static void drawBattery();
    static void setBattery(int batteryPercent0);

    GFXContainer & clear(int8_t direction = 1);

    GFXContainer & unprint();

    std::shared_ptr<GFXContainer> move(int8_t action)
    {  
        if (movementFunc(action)) {
            return nullptr;
        }

        //Serial.println("Beginning move");
        switch(action)
        {
            case FWD:
                return moveNext();
                break;
            case BWD:
                return movePrev();
                break;
            case BUTTON:
                if (up)
                    return moveUp();
                else
                    return moveDown();
                break;
        }

        return nullptr;
    }

    //Signals that we want to move to the given position;
    std::shared_ptr<GFXContainer> moveNext()
    {
        if (next == nullptr) return nullptr;
        if (up != nullptr) {
            Serial.printf("Moving to next, %d\n", next == nullptr);
            self->exit();
            next->enter(1);
            Serial.println("Returning next");
            return next;
        }
    
    };

    std::shared_ptr<GFXContainer> movePrev()
    {
        if (prev == nullptr) return nullptr;
        if (up != nullptr) {
            Serial.printf("Moving to prev, %d\n", prev == nullptr);
            self->exit();
            prev->enter(-1);
            Serial.println("Returning prev");
            return prev;
        }
    };

    std::shared_ptr<GFXContainer> moveUp()
    {
        if (up == nullptr) return nullptr;
        if (up != nullptr) {
            Serial.println("Moving to up");
            self->exit();
            up->enter(2);
            Serial.println("Returning up");
            return up;
        }
    };

    std::shared_ptr<GFXContainer> moveDown()
    {
        if (down == nullptr) return nullptr;
        if (down != nullptr) {
            Serial.println("Moving to down");
            self->exit();
            down->enter(-2);
            Serial.println("Returning down");
            return down;
        }
    };

}; 


#endif