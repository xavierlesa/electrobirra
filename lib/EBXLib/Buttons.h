#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"

// botones
//#define NONE_VALUE      1000
#define BUTTON_NONE     0
//     o
// o o o o o

//#define SELECT_VALUE    790
#define BUTTON_SELECT   1
//     o
// x o o o o

//#define BACK_VALUE      555
#define BUTTON_BACK     2
//     o
// o x o o o

//#define ENTER_VALUE     50
#define BUTTON_ENTER    3
//     o
// o o o x o

//#define UP_VALUE        195
#define BUTTON_UP       4
//     x
// o o o o o

//#define DOWN_VALUE      380
#define BUTTON_DOWN     5
//     o
// o o x o o

//#define BUTTON_RST      8
//     o
// o o o o x
// Este boton no se accede si no se conecta a mano

class Buttons
{
    private:
        //int KEYPAD_PIN;         // analog pin del keypad
        int BUTTON_SELECT_PIN;  // digital pin select
        int BUTTON_BACK_PIN;    // digital pin back
        int BUTTON_ENTER_PIN;   // digital pin enter
        int BUTTON_UP_PIN;      // digital pin up
        int BUTTON_DOWN_PIN;    // digital pin down
        int DELAY_MS; // default MS
    
    public:
        Buttons(int pinSelect, int pinBack, int pinEnter, int pinUp, int pinDown, const int delay_ms = 100);
        int getPressedButton();
        bool isUp();
        bool isDown();
        bool isSelect();
        bool isBack();
        bool isEnter();
        bool isNone();
};
#endif
