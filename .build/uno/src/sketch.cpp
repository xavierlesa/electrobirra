#include <Arduino.h>
#include <LiquidCrystal.h>
#include <Menu.h>
#include <Buttons.h>
#include <OneWire.h>
#include <DallasTemperature.h>
void setup();
void loop();
#line 1 "src/sketch.ino"
/*
 * ElectroBirra v 0.2
 * 
 * Proyecto con base arduino para hacer y controlar el proceso de la birra y el 
 * fermentador.
 *
 * autor: Xavier Lesa <xavierlesa@gmail.com>
 * git: http://github.com/ninjaotoko/electrobirra/
 *
 *
 *
 * Menu:
 *
 * 1- Monitor (muestra la temperatua actual)
 * 2- Cocción (rutina para una cocción)
 * 3- Fermentador (rutina para el control del fermentador)
 * 4- Configuracion (setea los valores generales del equipo, como sensores y pines)
 *
 *
 * Manual/Monitor
 * --------------
 *
 *  Temperatura del Sensor
 *  (select para menú principal)
 *  (up para calentar de forma manual)
 *  (down para prender la bomba)
 *
 *
 * Cocción
 * -------
 *
 *  1- Macerado
 *  1.1- Temperatura del macerado
 *  1.2- Escalones del macerado
 *
 *  2- Recirculado
 *  2.1- Recirculado continuo (durante todo el macerado)
 *  2.2- Tiempo de recirculado
 *
 *  3- Lavado
 *  3.1- Temperatura del lavado
 *  3.2- Tiempo de lavado
 *
 *  4- Hervor
 *  4.1- Tiempo de hervor
 *  4.2- Adesión de lupulo[s]
 *  4.3- Escalones de adesión
 *
 *  5- Whirlpool
 *  5.1- Tiempo de whirpool
 *
 *  6- Enfirado
 *  6.1- Traspaso al fermentador
 *
 *  7- Fin
 *
 *
 */

//#include <LiquidCrystal.h>
//#include <Menu.h>
//#include <Buttons.h>

//#include <OneWire.h>
//#include <DallasTemperature.h>
//#include <Core.h>

// PIN MAPPING
//
// ANALOGOS
//
// A0:  BUTTONS
// A1:  NC
// A2:  NC
// A3:  NC
// A4:  NC
// A5:  NC
// 
// DIGITAL
// 
// 0:   NC (RELE_LT_PWM_PIN)
// 1:   NC (RELE_R2_PWM_PIN)
// 2:   NC (RELE_HT_PWM_PIN)
// 3~:  NC (RELE_R1_PWM_PIN)
// 4:   LCD_D4
// 5~:  LCD_D5
// 6~:  LCD_D6
// 7:   LCD_D7
// 8:   LCD_RS
// 9~:  LCD_ENABLE
// 10~: LCD_BACKLIGHT
// 11~: RELE_PUMP_PWM_PIN
// 12:  NC
// 13:  SENSOR_ONEWIRE_PIN
//
// Pines A0 y 4 a 9 reservados

// Menu, LCD setting col/rows
#define BUTTONS_PIN     A0 // ANALOG

#define LCD_D4          4
#define LCD_D5          5
#define LCD_D6          6
#define LCD_D7          7
#define LCD_RS          8
#define LCD_ENABLE      9
#define LCD_BACKLIGHT   10

#define LCD_COLUMNS   16
#define LCD_ROWS      2

// Bus I2C
// Sensores de temp boil, mash, etc
#define SENSOR_ONEWIRE_PIN  13 //2

// RELES
// Reles para resistencias x 2
#define RELE_R1_PWM_PIN     12 // rele para r1
#define RELE_R2_PWM_PIN     12 // rele para r2
#define RELE_HT_PWM_PIN     12 // rele para fermentador temp >= H
#define RELE_LT_PWM_PIN     12 // rele para fermentador temp <= L

// Rele para bomba
#define RELE_PUMP_PWM_PIN   11 // rele para bomba recirculado, trasvase y enfriado

Menu menu(
        BUTTONS_PIN, 
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
        RELE_PUMP_PWM_PIN
        );

void setup()
{
    // init Serial 
    Serial.begin(9600);
    pinMode(RELE_R1_PWM_PIN, OUTPUT);
    pinMode(RELE_PUMP_PWM_PIN, OUTPUT);

    digitalWrite(RELE_R1_PWM_PIN, HIGH);
    digitalWrite(RELE_PUMP_PWM_PIN, HIGH);

    menu.lcd.clear();
    menu.lcd.print("  ElectroBirra");
    menu.lcd.setCursor(0, 1);
    menu.lcd.print("  v0.2 201507");

    delay(2500);
}

void loop()
{ 
    menu.showMenu();
}
