#ifndef Buttons_h
#define Buttons_h

#include "Arduino.h"

// botones
//#define NONE_VALUE      1024
#define BUTTON_NONE     0
//     o
// o o o o o

//#define SELECT_VALUE    600 ~ 650
#define BUTTON_SELECT   1
//     o
// x o o o o

//#define BACK_VALUE      400 ~ 450
#define BUTTON_LEFT     2
//     o
// o x o o o

//#define ENTER_VALUE     0 ~ 50
#define BUTTON_RIGHT    3
//     o
// o o o x o

//#define UP_VALUE        100 ~ 150
#define BUTTON_UP       4
//     x
// o o o o o

//#define DOWN_VALUE      250 ~ 300
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
        int BUTTONS_PIN;        // analog pin del keypad
        int BUTTON_SELECT_PIN;  // digital pin select
        int BUTTON_LEFT_PIN;    // digital pin left
        int BUTTON_RIGHT_PIN;   // digital pin right
        int BUTTON_UP_PIN;      // digital pin up
        int BUTTON_DOWN_PIN;    // digital pin down
        int DELAY_MS; // default MS
    
    public:
        Buttons(int pinButtons, int pinSelect, int pinLeft, int pinRight, int pinUp, int pinDown, const int delay_ms = 20);
        int getPressedButton();
        bool isUp();
        bool isDown();
        bool isSelect();
        bool isLeft();
        bool isBack();
        bool isRight();
        bool isEnter();
        bool isNone();
};
#endif
