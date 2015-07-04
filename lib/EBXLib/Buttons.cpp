#include "Arduino.h"
#include "Buttons.h"

Buttons::Buttons(int pin, const int delay_ms)
{
    pinMode(pin, INPUT);
    KEYPAD_PIN = pin;
    DELAY_MS = delay_ms;
}

int Buttons::getPressedButton()
{
    int lastPressedBtn;
    int press_key = analogRead(KEYPAD_PIN);      // read the value from the sensor
    // my buttons when read are centered at these valies: 0, 144, 329, 504, 741
    // we add approx 50 to those values and check to see if we are close
    
    //Serial.println("pressed key");
    //Serial.println(press_key);

    delay(DELAY_MS);

    if (press_key > NONE_VALUE) 
    {
        return BUTTON_NONE; // We make this the 1st option for speed reasons since it will be the most likely result
    }

    if (press_key < ENTER_VALUE) 
    {
        lastPressedBtn = BUTTON_ENTER;
        return BUTTON_ENTER; //BUTTON_RIGHT; 
    }

    if (press_key < UP_VALUE) 
    {
        lastPressedBtn = BUTTON_UP;
        return BUTTON_UP;
    }

    if (press_key < DOWN_VALUE) 
    {
        lastPressedBtn = BUTTON_DOWN;
        return BUTTON_DOWN;
    }

    if (press_key < BACK_VALUE) 
    {
        lastPressedBtn = BUTTON_BACK;
        return BUTTON_BACK; //BUTTON_LEFT;
    }

    if (press_key < SELECT_VALUE) 
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
