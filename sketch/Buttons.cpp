#include "Arduino.h"
#include "Buttons.h"


Buttons::Buttons(int pinButtons, int pinSelect, int pinLeft, int pinRight, int pinUp, int pinDown, const int delay_ms)
{
#if BUTTONS_ANALOG == true
    BUTTONS_PIN = pinButtons;
#else
    pinMode(pinSelect, INPUT_PULLUP);
    pinMode(pinLeft, INPUT_PULLUP);
    pinMode(pinRight, INPUT_PULLUP);
    pinMode(pinUp, INPUT_PULLUP);
    pinMode(pinDown, INPUT_PULLUP);

    BUTTON_SELECT_PIN = pinSelect;
    BUTTON_LEFT_PIN = pinLeft;
    BUTTON_RIGHT_PIN = pinRight;
    BUTTON_UP_PIN = pinUp;
    BUTTON_DOWN_PIN = pinDown;
#endif
    DELAY_MS = delay_ms;
}

int Buttons::getPressedButton()
{
    delay(DELAY_MS);

    int lastPressedBtn;
#if BUTTONS_ANALOG == true
    int adc_key_in = analogRead(BUTTONS_PIN);

    // UP
    if (adc_key_in < 150 and adc_key_in > 100)
    {
        lastPressedBtn = BUTTON_UP;
        return BUTTON_UP;
    }

    // DOWN
    if (adc_key_in < 300 and adc_key_in > 250)
    {
        lastPressedBtn = BUTTON_DOWN;
        return BUTTON_DOWN;
    }

    // LEFT
    if (adc_key_in < 450 and adc_key_in > 400)
    {
        lastPressedBtn = BUTTON_LEFT;
        return BUTTON_LEFT; //BUTTON_LEFT;
    }

    // RIGHT
    if (adc_key_in < 50 and adc_key_in > 10)
    {
        lastPressedBtn = BUTTON_RIGHT;
        return BUTTON_RIGHT; //BUTTON_RIGHT; 
    }

    // SELECT
    if (adc_key_in < 850 and adc_key_in > 600)
    {
        lastPressedBtn = BUTTON_SELECT;
        return BUTTON_SELECT;
    }
#else

    // UP
    if (digitalRead(BUTTON_UP_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_UP;
        return BUTTON_UP;
    }

    // DOWN
    if (digitalRead(BUTTON_DOWN_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_DOWN;
        return BUTTON_DOWN;
    }

    // LEFT
    if (digitalRead(BUTTON_LEFT_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_LEFT;
        return BUTTON_LEFT; //BUTTON_LEFT;
    }

    // RIGHT
    if (digitalRead(BUTTON_RIGHT_PIN) == LOW)
    {
        lastPressedBtn = BUTTON_RIGHT;
        return BUTTON_RIGHT; //BUTTON_RIGHT; 
    }

    // SELECT
    if (digitalRead(BUTTON_SELECT_PIN) == LOW) 
    {
        lastPressedBtn = BUTTON_SELECT;
        return BUTTON_SELECT;
    }
#endif
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

bool Buttons::isLeft()
{
    return Buttons::getPressedButton() == BUTTON_LEFT;
}
// Alias left
bool Buttons::isBack()
{
    return Buttons::isLeft();
}

bool Buttons::isRight()
{
    return Buttons::getPressedButton() == BUTTON_RIGHT;
}
// Alias right
bool Buttons::isEnter()
{
    return Buttons::isRight();
}

bool Buttons::isNone()
{
    return Buttons::getPressedButton() == BUTTON_NONE;
}
