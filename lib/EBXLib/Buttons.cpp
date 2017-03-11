#include "Arduino.h"
#include "Buttons.h"

Buttons::Buttons(int pinSelect, int pinBack, int pinEnter, int pinUp, int pinDown, const int delay_ms)
{
    pinMode(pinSelect, INPUT_PULLUP);
    pinMode(pinBack, INPUT_PULLUP);
    pinMode(pinEnter, INPUT_PULLUP);
    pinMode(pinUp, INPUT_PULLUP);
    pinMode(pinDown, INPUT_PULLUP);

    //KEYPAD_PIN = pin;
    BUTTON_SELECT_PIN = pinSelect;
    BUTTON_BACK_PIN = pinBack;
    BUTTON_ENTER_PIN = pinEnter;
    BUTTON_UP_PIN = pinUp;
    BUTTON_DOWN_PIN = pinDown;
    DELAY_MS = delay_ms;
}

int Buttons::getPressedButton()
{
    int lastPressedBtn;

    delay(DELAY_MS);

    if (digitalRead(BUTTON_ENTER_PIN) == LOW)
    {
        lastPressedBtn = BUTTON_ENTER;
        return BUTTON_ENTER; //BUTTON_RIGHT; 
    }

    if (digitalRead(BUTTON_UP_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_UP;
        return BUTTON_UP;
    }

    if (digitalRead(BUTTON_DOWN_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_DOWN;
        return BUTTON_DOWN;
    }

    if (digitalRead(BUTTON_BACK_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_BACK;
        return BUTTON_BACK; //BUTTON_LEFT;
    }

    if (digitalRead(BUTTON_SELECT_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_SELECT;
        return BUTTON_SELECT;
    }

    return BUTTON_NONE;  // when all others fail, return this...
}

bool Buttons::isUp()
{
    return Buttons::getPressedButton() == BUTTON_UP;
}

bool Buttons::isDown()
{
    return Buttons::getPressedButton() == BUTTON_DOWN;
}

bool Buttons::isSelect()
{
    return Buttons::getPressedButton() == BUTTON_SELECT;
}

bool Buttons::isBack()
{
    return Buttons::getPressedButton() == BUTTON_BACK;
}

bool Buttons::isEnter()
{
    return Buttons::getPressedButton() == BUTTON_ENTER;
}

bool Buttons::isNone()
{
    return Buttons::getPressedButton() == BUTTON_NONE;
}
