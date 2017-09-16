////////////////////////////////////////////////////////////////////////////////
//
// ElectroBirra v 0.2
// 
// Proyecto con base arduino para hacer y controlar el proceso de la birra y el 
// fermentador.
//
// autor: Xavier Lesa <xavierlesa@gmail.com>
// git: http://github.com/ninjaotoko/electrobirra/
//
////////////////////////////////////////////////////////////////////////////////
//
//
// Menu:
//
// 1- Monitor (muestra la temperatua actual)
// 2- Cocción (rutina para una cocción)
// 3- Fermentador (rutina para el control del fermentador)
// 4- Configuracion (setea los valores generales del equipo, como sensores y pines)
//
//
// Manual/Monitor
// --------------
//
//  Temperatura del Sensor
//  (select para menú principal)
//  (up para calentar de forma manual)
//  (down para prender la bomba)
//
//
// Cocción
// -------
//
//  1- Macerado
//  1.1- Temperatura del macerado
//  1.2- Escalones del macerado
//
//  2- Recirculado
//  2.1- Recirculado continuo (durante todo el macerado)
//  2.2- Tiempo de recirculado
//
//  3- Lavado
//  3.1- Temperatura del lavado
//  3.2- Tiempo de lavado
//
//  4- Hervor
//  4.1- Tiempo de hervor
//  4.2- Adesión de lupulo[s]
//  4.3- Escalones de adesión
//
//  5- Whirlpool
//  5.1- Tiempo de whirpool
//
//  6- Enfirado
//  6.1- Traspaso al fermentador
//
//  7- Fin
////////////////////////////////////////////////////////////////////////////////

#include <LiquidCrystal.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <PID_v1.h>
#include "Buttons.h"
#include "Menu.h"
#include "config.h"

//#if BUTTONS_ANALOG == true
//Menu menu(
//        BUTTONS_PIN,
//        BUTTON_DELAY,
//        LCD_RS, 
//        LCD_ENABLE, 
//        LCD_D4, 
//        LCD_D5, 
//        LCD_D6, 
//        LCD_D7, 
//        LCD_COLUMNS, 
//        LCD_ROWS, 
//        LCD_BACKLIGHT,
//        SENSOR_ONEWIRE_PIN,
//        RELE_R1_PWM_PIN,
//        RELE_R2_PWM_PIN,
//        RELE_HT_PWM_PIN,
//        RELE_LT_PWM_PIN,
//        RELE_PUMP_A_PWM_PIN,
//        RELE_PUMP_B_PWM_PIN,
//        RELE_NA
//        );
//#else
Menu menu(
        BUTTONS_PIN,
        BUTTON_SELECT_PIN,
        BUTTON_BACK_PIN,
        BUTTON_ENTER_PIN,
        BUTTON_UP_PIN,
        BUTTON_DOWN_PIN,
        BUTTON_DELAY,
        LCD_RS, 
        LCD_ENABLE, 
        LCD_D4, 
        LCD_D5, 
        LCD_D6, 
        LCD_D7, 
        LCD_COLUMNS, 
        LCD_ROWS, 
        LCD_BACKLIGHT,
        SENSOR_ONEWIRE_PIN,
        RELE_R1_PWM_PIN,
        RELE_R2_PWM_PIN,
        RELE_HT_PWM_PIN,
        RELE_LT_PWM_PIN,
        RELE_PUMP_A_PWM_PIN,
        RELE_PUMP_B_PWM_PIN,
        RELE_NA
        );
//#endif

void setup()
{
    // init Serial 
    Serial.begin(9600);
    Serial.println("//////////////////////////////////////// Inicio setup");

#if BUTTONS_ANALOG == true
    Serial.println("keypad mode ANALOG");
#else
    Serial.println("keypad mode DIGITAL A1,A2,A3,A4,A5");
#endif

    pinMode(RELE_R1_PWM_PIN, OUTPUT);
#if SINGLE_R_PWN == false
    pinMode(RELE_R2_PWM_PIN, OUTPUT);
#endif
    pinMode(RELE_PUMP_A_PWM_PIN, OUTPUT);
    pinMode(RELE_PUMP_B_PWM_PIN, OUTPUT);

    digitalWrite(RELE_R1_PWM_PIN, RELE_NA ^ LOW);
#if SINGLE_R_PWN == false
    digitalWrite(RELE_R2_PWM_PIN, RELE_NA ^ LOW);
#endif
    digitalWrite(RELE_PUMP_A_PWM_PIN, RELE_NA ^ LOW);
    digitalWrite(RELE_PUMP_B_PWM_PIN, RELE_NA ^ LOW);

    menu.lcd.clear();
    menu.lcd.print("  ElectroBirra");
    menu.lcd.setCursor(0, 1);
    //menu.lcd.print("             ");
    menu.lcd.setCursor(2, 1);
    menu.lcd.print(EBX_VERSION);

    Serial.println("Cargando los datos de configuracion desde la EEPROM");
    menu.loadData();
    delay(2500);
    Serial.println("//////////////////////////////////////// Fin setup");
}

void loop()
{ 
    menu.showMenu();
}
