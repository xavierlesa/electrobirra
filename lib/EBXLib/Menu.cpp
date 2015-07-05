#include "Arduino.h"
#include <LiquidCrystal.h>
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buttons.h"
#include "Menu.h"

Menu::Menu(
        int BUTTONS_PIN, 
        int LCD_RS,
        int LCD_ENABLE,
        int LCD_D4,
        int LCD_D5,
        int LCD_D6,
        int LCD_D7,
        int LCD_COLUMNS, 
        int LCD_ROWS, 
        int LCD_BACKLIGHT,
        int SENSOR_ONEWIRE_PIN,
        int RELE_R1_PWM_PIN,
        int RELE_R2_PWM_PIN,
        int RELE_HT_PWM_PIN,
        int RELE_LT_PWM_PIN,
        int RELE_PUMP_PWM_PIN
        ): 
    buttons(BUTTONS_PIN, 50),
    lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7),
    oneWire(SENSOR_ONEWIRE_PIN),
    sensorsBus(&oneWire)
{
    // backlight

    _lcd_columns = LCD_COLUMNS;
    _lcd_rows = LCD_ROWS;

    _rele_r1_pwm_pin = RELE_R1_PWM_PIN;
    _rele_r2_pwm_pin = RELE_R2_PWM_PIN;
    _rele_ht_pwm_pin = RELE_HT_PWM_PIN;
    _rele_lt_pwm_pin = RELE_LT_PWM_PIN;
    _rele_pump_pwm_pin = RELE_PUMP_PWM_PIN;

    arrowUp = 0;
    arrowUpByte[0] = B00000;
    arrowUpByte[1] = B00100;
    arrowUpByte[2] = B01110; 
    arrowUpByte[3] = B11111; 
    arrowUpByte[4] = B00000; 
    arrowUpByte[5] = B00000; 
    arrowUpByte[6] = B00000;

    arrowDown = 1;
    arrowDownByte[0] = B00000;
    arrowDownByte[1] = B00000; 
    arrowDownByte[2] = B00000; 
    arrowDownByte[3] = B11111; 
    arrowDownByte[4] = B01110; 
    arrowDownByte[5] = B00100;
    arrowDownByte[6] = B00000;

    arrowLeft = 2;
    arrowLeftByte[0] = B00001;
    arrowLeftByte[1] = B00011; 
    arrowLeftByte[2] = B00111; 
    arrowLeftByte[3] = B01111; 
    arrowLeftByte[4] = B00111; 
    arrowLeftByte[5] = B00011;
    arrowLeftByte[6] = B00001;

    arrowRight = 3;
    arrowRightByte[0] = B10000;
    arrowRightByte[1] = B11000; 
    arrowRightByte[2] = B11100; 
    arrowRightByte[3] = B11110; 
    arrowRightByte[4] = B11100; 
    arrowRightByte[5] = B11000;
    arrowRightByte[6] = B10000;

    cursorDot = 4;
    cursorDotByte[0] = B11000;
    cursorDotByte[1] = B11100; 
    cursorDotByte[2] = B11010; 
    cursorDotByte[3] = B11001; 
    cursorDotByte[4] = B11010; 
    cursorDotByte[5] = B11100; 
    cursorDotByte[6] = B11000;

    cursorSpace = 5;
    cursorSpaceByte[0] = B00000;
    cursorSpaceByte[1] = B00000;
    cursorSpaceByte[2] = B11111; 
    cursorSpaceByte[3] = B11111; 
    cursorSpaceByte[4] = B11111; 
    cursorSpaceByte[5] = B00000; 
    cursorSpaceByte[6] = B00000;
    
    pointer_cursor = 0;

    // config LCD
    pinMode(LCD_BACKLIGHT, OUTPUT);
    digitalWrite(LCD_BACKLIGHT, HIGH);

    lcd.createChar(arrowUp, arrowUpByte);
    lcd.createChar(arrowDown, arrowDownByte);
    lcd.createChar(arrowLeft, arrowLeftByte);
    lcd.createChar(arrowRight, arrowRightByte);
    lcd.createChar(cursorDot, cursorDotByte);
    lcd.createChar(cursorSpace, cursorSpaceByte);

    // init LCD
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.clear();

    // init sensors
    sensorsBus.begin();
 
    menuItems[MENU_MANUAL] = "Manual/Monitor";
    menuItems[MENU_BREW] = "Coccion";
    menuItems[MENU_FERMENTATION] = "Fermentador";
    menuItems[MENU_CONFIGURATION] = "Configuracion";

    menuItemsManual[MANUAL_STAGE_MONITOR][SUBSTAGE_INDEX] = "Monitor";
    menuItemsManual[MANUAL_STAGE_HOT][SUBSTAGE_INDEX] = "Calentar";
    menuItemsManual[MANUAL_STAGE_PUMP][SUBSTAGE_INDEX] = "Bombear";

    menuItemsBrew[BREW_STAGE_MASH][SUBSTAGE_INDEX] = "Macerado";
    menuItemsBrew[BREW_STAGE_MASH][1] = "Primer Escalon";
    menuItemsBrew[BREW_STAGE_MASH][2] = "Segundo Escalon";
    menuItemsBrew[BREW_STAGE_MASH][3] = "Tercero Escalon";
    menuItemsBrew[BREW_STAGE_MASH][4] = "Cuarto Escalon";
    menuItemsBrew[BREW_STAGE_MASH][5] = "Quinto Escalon";

    menuItemsBrew[BREW_STAGE_RECIRCULATION][SUBSTAGE_INDEX] = "Recirculado";
    //menuItemsBrew[BREW_STAGE_RECIRCULATION][1] = "Recirculado continuo";
    //menuItemsBrew[BREW_STAGE_RECIRCULATION][2] = "Tiempo de recirculado";
    
    menuItemsBrew[BREW_STAGE_SPARGING][SUBSTAGE_INDEX] = "Lavado";
    menuItemsBrew[BREW_STAGE_SPARGING][1] = "Temperatura del lavado";
    menuItemsBrew[BREW_STAGE_SPARGING][2] = "Tiempo de lavado";
    
    menuItemsBrew[BREW_STAGE_BOIL][SUBSTAGE_INDEX] = "Hervor";
    menuItemsBrew[BREW_STAGE_BOIL][1] = "Tiempo de hervor";
    menuItemsBrew[BREW_STAGE_BOIL][2] = "Adesión de lupulo[s]";
    menuItemsBrew[BREW_STAGE_BOIL][3] = "Escalones de adesión";

    menuItemsBrew[BREW_STAGE_WHIRLPOOL][SUBSTAGE_INDEX] = "Whirlpool";
    menuItemsBrew[BREW_STAGE_WHIRLPOOL][1] = "Tiempo de whirlpool";

    menuItemsBrew[BREW_STAGE_COOLING][SUBSTAGE_INDEX] = "Enfriado";

    // TODO: ELIMINAR ESTO ES SOLO PARA TESTING
    // TESTING TESTING TESTING TESTING TESTING TESTING TESTING TESTING

    //addrSensorHlt[0] = 0x28;
    //addrSensorHlt[1] = 0xFF;
    //addrSensorHlt[2] = 0xBA;
    //addrSensorHlt[3] = 0x51;
    //addrSensorHlt[4] = 0x4B;
    //addrSensorHlt[5] = 0x04;
    //addrSensorHlt[6] = 0x00;
    //addrSensorHlt[7] = 0x26;

    //for(int i = 0; i < 8; i++)
    //{
    //    EEPROM.write(i, addrSensorHlt[i]);
    //}

    EEPROM_readAnything(ADDR_SENSOR_HTL, addrSensorHlt);
    EEPROM_readAnything(ADDR_SENSOR_MT, addrSensorMt);
    EEPROM_readAnything(ADDR_SENSOR_FC, addrSensorFc);

    EEPROM_readAnything(ADDR_BREW_MASH_STEP0_TEMP, brewMashStep0Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP1_TEMP, brewMashStep1Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP2_TEMP, brewMashStep2Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP3_TEMP, brewMashStep3Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP4_TEMP, brewMashStep4Temp);

    EEPROM_readAnything(ADDR_BREW_MASH_STEP0_TIME, brewMashStep0Time);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP1_TIME, brewMashStep1Time);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP2_TIME, brewMashStep2Time);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP3_TIME, brewMashStep3Time);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP4_TIME, brewMashStep4Time);
    
    EEPROM_readAnything(ADDR_BREW_RECIRCULATION_CONT, brewRecirculationCont);
    EEPROM_readAnything(ADDR_BREW_RECIRCULATION_TIME, brewRecirculationTime);

    if(isnan(brewMashStep0Temp)) brewMashStep0Temp = 66.5;
    if(isnan(brewMashStep1Temp)) brewMashStep1Temp = 66.5;
    if(isnan(brewMashStep2Temp)) brewMashStep2Temp = 66.5;
    if(isnan(brewMashStep3Temp)) brewMashStep3Temp = 66.5;
    if(isnan(brewMashStep4Temp)) brewMashStep4Temp = 66.5;

    if(isinf(brewMashStep0Time) || brewMashStep0Time < 0) brewMashStep0Time = 90;
    if(isinf(brewMashStep1Time) || brewMashStep1Time < 0) brewMashStep1Time = 0;
    if(isinf(brewMashStep2Time) || brewMashStep2Time < 0) brewMashStep2Time = 0;
    if(isinf(brewMashStep3Time) || brewMashStep3Time < 0) brewMashStep3Time = 0;
    if(isinf(brewMashStep4Time) || brewMashStep4Time < 0) brewMashStep4Time = 0;
    
    if(isinf(brewRecirculationTime) || brewRecirculationTime < 0) brewRecirculationTime = 0;

    //Serial.begin(9600);
    //Serial.println("Que hay en EEPROM? ####################################");
    //for(int i = 0; i < 256; i++)
    //{
    //    Serial.print(String(i, HEX) + ": ");
    //    Serial.println(String(EEPROM.read(i), HEX));
    //}
}

///////////////////////////////////////////////////////////////////////////////
// MENU HELPERS
///////////////////////////////////////////////////////////////////////////////
int Menu::stageSize(int stage, int max_substage_index)
{
    /*
     * Helper para saber cuentos elementos "reales" hay en un substage
     */

    int a = 0;
    for(int i = 0; i < max_substage_index; i++)
    {
        if(String(menuItemsBrew[stage][i]).length() > 0) a++;
    }

    return a;
}

void Menu::stageSelector(int stage, int subpointer, bool asc)
{
    /*
     * Selector de stage
     */

    int max_items = Menu::stageSize(stage, MAX_BREW_SUBSTAGE_ITEMS) - 1;
    lcd.clear();
    lcd.setCursor(0, 0);
    if(asc)
    { 
        lcd.write(cursorDot);
        lcd.print(menuItemsBrew[stage][subpointer]);
        lcd.setCursor(0, 1);
        lcd.print(menuItemsBrew[stage][subpointer+1]);
    }
    else
    {
        lcd.print(menuItemsBrew[stage][subpointer-1]);
        lcd.setCursor(0, 1);
        lcd.write(cursorDot);
        lcd.print(menuItemsBrew[stage][subpointer]);
    }

}

void Menu::menuNext()
{
    if(pointer_cursor < MAX_MENU_ITEMS-1)
    {
        pointer_cursor++;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print(menuItems[pointer_cursor-1]);
        lcd.setCursor(0, 1);
        lcd.write(cursorDot);
        lcd.print(menuItems[pointer_cursor]);
    }
}

void Menu::menuPrev()
{
    if(pointer_cursor > 0)
    {
        pointer_cursor--;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.write(cursorDot);
        lcd.print(menuItems[pointer_cursor]);
        lcd.setCursor(0, 1);
        lcd.print(menuItems[pointer_cursor+1]);
    }
}

void Menu::_showSave()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("    Guardar?    ");
    lcd.setCursor(0, 1);
    lcd.write(arrowLeft);
    lcd.setCursor(15, 1);
    lcd.write(arrowRight);
    lcd.setCursor(1, 1);
    lcd.print("NO          SI");
}

void Menu::_showSaved()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  Guardando...  ");
    delay(1000);
}

void Menu::cursorFloat(float _f, int cur_x, int cur_y)
{
    lcd.setCursor(cur_x, cur_y);
    if(_f < 10)
    { 
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print("__" + String(_f, 1));
        else lcd.print("___._");
    }
    if(_f >= 10 && _f < 100)
    {
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print("_" + String(_f, 1));
        else lcd.print("___._");
    }
    if(_f >= 100)
    {
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print(String(_f, 1));
        else lcd.print("___._");
    }
    _blink = !_blink;
}

void Menu::stepSetFloat(float _default, float _step, float *buffer, int cur_x, int cur_y)
{
    /*
     * Utilidad para setear en buffer un valor Float incrementando o decrementando
     * segun el step.
     * Sale con "Select" y muestra en LCD el valor
     */

    while(!buttons.isBack() && !buttons.isSelect() && !buttons.isEnter())
    {
        if(buttons.isUp()) _default += _step;
        if(buttons.isDown() && _default >= _step) _default -= _step;
        Menu::cursorFloat(_default, cur_x, cur_y);
        Serial.println("stepSetFloat " + String(_default, 1));
    }

    *buffer = _default;
}

void Menu::cursorInt(int _i, int cur_x, int cur_y)
{
    lcd.setCursor(cur_x, cur_y);
    if(_i < 10)
    { 
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print("__" + String(_i));
        else lcd.print("___");
    }
    if(_i >= 10 && _i < 100)
    {
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print("_" + String(_i));
        else lcd.print("___");
    }
    if(_i >= 100)
    {
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print(String(_i));
        else lcd.print("___");
    }
    _blink = !_blink;
}

void Menu::stepSetInt(int _default, int _step, int *buffer, int cur_x, int cur_y)
{
    /*
     * Utilidad para setear en buffer un valor Int incrementando o decrementando
     * segun el step.
     * Sale con "Select" y muestra en LCD el valor
     */

    while(!buttons.isBack() && !buttons.isSelect() && !buttons.isEnter())
    {
        if(buttons.isUp()) _default += _step;
        if(buttons.isDown() && _default >= _step) _default -= _step;
        Menu::cursorInt(_default, cur_x, cur_y);
        Serial.println("stepSetInt " + String(_default));
    }

    *buffer = _default;
}

void Menu::cursorBool(bool _i, int cur_x, int cur_y)
{
    lcd.setCursor(cur_x, cur_y);
    if(_blink) lcd.print( (_i ? "S" : "N") );
    else lcd.print("_");
    _blink = !_blink;
}

void Menu::stepSetBool(bool _default, bool *buffer, int cur_x, int cur_y)
{
    while(!buttons.isBack() && !buttons.isSelect() && !buttons.isEnter())
    {
        if(buttons.isUp()) _default = true;
        if(buttons.isDown()) _default = false;
        Menu::cursorBool(_default, cur_x, cur_y);
        Serial.println("stepSetInt " + String(_default));
    }

    *buffer = _default;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
void Menu::home()
{
    /*
     * Inicio, muestra el menu principal
     */

    pointer_cursor = 0;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(cursorDot);
    lcd.print(menuItems[0]);
    lcd.setCursor(0, 1);
    lcd.print(menuItems[1]);
}

int Menu::manual(int upPin, int downPin) //, int enterPin, int backPin)
{
    /*
     * Ejecuta el modo manual, para controlos los dispositivos
     *
     * UP, controla las resistencias para calentar
     * DOWN, controla la bomba de recirtculado
     * ENTER y BACK reservados para futuros usos.
     *
     * Devuelve 4 bits (un int), indicando el estado segun el bit
     *
     * B10XX upPin on
     *  ^^^^
     *  ||||
     *  |||+- p0 backPin (reservado)
     *  ||+-- p1 enterPin (reservado)
     *  |+--- p2 downPin
     *  +---- p3 upPin
     */

    int r = 0, p0, p1, p2, p3;

    // Hot stop/start
    // TODO: inicio seguro, solo si hay liquido!!
    
    p3 = digitalRead(upPin);
    p2 = digitalRead(downPin);

    if(buttons.isUp()) digitalWrite(upPin, !p3);
    if(buttons.isDown()) digitalWrite(downPin, !p2);
    //if(buttons.isEnter()) digitalWrite(enterPin, !p1);
    //if(buttons.isBack()) digitalWrite(backPin, !p0);
    
    r |= (!p3 << 3);
    r |= (!p2 << 2);
    //r |= (!p1 << 1);
    //r |= (!p0 << 0);

    delay(100);
    return r;
}

void Menu::monitor(float hlt, float mt, float fc, int _flags)
{
    /*
     * Muestra el monitor de temperatura
     * hasta que se presione la tecla "select"
     * y titila en el control manual activo
     */

    lcd.clear();
    char buffer[10];
    String _hlt = dtostrf(hlt, 3, 1, buffer);
    String _mt = dtostrf(mt, 3, 1, buffer);
    String _fc = dtostrf(fc, 3, 1, buffer);

    lcd.print("HLT " + String(_hlt));
    lcd.setCursor(10,0); 
    lcd.print("Hot  "); 
    if(_flags & B1000) lcd.print("*");
    else lcd.write(arrowUp);
    
    lcd.setCursor(0,1);
    lcd.print("MT " + String(_mt));
    lcd.setCursor(10,1); 
    lcd.print("Pump "); 
    if(_flags & B0100) lcd.print("*");
    else lcd.write(arrowDown);
}

void Menu::brew(int pointer)
{
    /*
     * Muestra el programa de coccion y configuracion.
     * Desde aqui se configuran las variables de coccion.
     */

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(cursorDot);
    lcd.print(menuItemsBrew[pointer][SUBSTAGE_INDEX]);
    lcd.setCursor(0, 1);
    lcd.print(menuItemsBrew[pointer+1][SUBSTAGE_INDEX]);
        
    while(!buttons.isSelect())
    {
        if(buttons.isUp() && pointer > 0)
        {
            pointer--;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.write(cursorDot);
            lcd.print(menuItemsBrew[pointer][SUBSTAGE_INDEX]);
            lcd.setCursor(0, 1);
            lcd.print(menuItemsBrew[pointer+1][SUBSTAGE_INDEX]);
        }

        if(buttons.isDown() && pointer < MAX_BREW_STAGE_ITEMS - 1)
        {
            pointer++;
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[pointer-1][SUBSTAGE_INDEX]);
            lcd.setCursor(0, 1);
            lcd.write(cursorDot);
            lcd.print(menuItemsBrew[pointer][SUBSTAGE_INDEX]);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_MASH)
        {
            Serial.println("Entrando a modo macerado");
            Menu::brewMash();
            Menu::brew(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_RECIRCULATION)
        {
            Serial.println("Entrando a modo recirculado");
            Menu::brewRecirculation();
            Menu::brew(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_SPARGING)
        {
            Serial.println("Entrando a modo lavado");
            Menu::brewSparging();
            Menu::brew(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_BOIL)
        {
            Serial.println("Entrando a modo hervor");
            Menu::brewBoil();
            Menu::brew(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_WHIRLPOOL)
        {
            Serial.println("Entrando a modo whirpool");
            Menu::brewWhirlpool();
            Menu::brew(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_COOLING)
        {
            Serial.println("Entrando a modo enfriado");
            Menu::brewCooling();
            Menu::brew(pointer);
        }
    }
}

void Menu::brewMash()
{
    /*
     * Muestra el menu de configuracion del macerado.
     */

    int subpointer = 1;
    int max_items = Menu::stageSize(BREW_STAGE_MASH, MAX_BREW_SUBSTAGE_ITEMS) - 1;
    Menu::stageSelector(BREW_STAGE_MASH, subpointer);

    while(!buttons.isSelect())
    {
        if(buttons.isUp() && subpointer > 1)
        {
            subpointer--;
            Menu::stageSelector(BREW_STAGE_MASH, subpointer);
        }

        if(buttons.isDown() && subpointer < max_items)
        {

            subpointer++;
            Menu::stageSelector(BREW_STAGE_MASH, subpointer, false);
        }

        Serial.println("Configura el tiempo y temperatura del escalon " + String(subpointer) + " - max_items: " + String(max_items));

        if(buttons.isEnter())
        {
            // configura el tiempo y temperratura del escalon n

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[BREW_STAGE_MASH][subpointer]);
            lcd.setCursor(0, 1);
            lcd.print("___._C / ___ min");
            
            Serial.println("EEPROM_writeAnything brewMashStep (subpointer) " + String(subpointer)); 

            switch(subpointer-1)
            {
                case 0:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep0Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep0Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep0();
                    Menu::stageSelector(BREW_STAGE_MASH, subpointer);
                    break;

                case 1:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep1Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep1Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep1();
                    Menu::stageSelector(BREW_STAGE_MASH, subpointer);
                    break;

                case 2:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep2Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep2Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep2();
                    Menu::stageSelector(BREW_STAGE_MASH, subpointer);
                    break;

                case 3:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep3Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep3Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep3();
                    Menu::stageSelector(BREW_STAGE_MASH, subpointer);
                    break;

                case 4:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep4Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep4Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep4();
                    // ultimo item muestra el menu con modo up
                    Menu::stageSelector(BREW_STAGE_MASH, subpointer, false);
                    break;
            }
        } 
    }
}

void Menu::configure_brewMashStep0()
{
    float *_temp = &__temp;
    stepSetFloat(brewMashStep0Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    Serial.println("Setting save stepSetFloat " + String(*_temp));
    delay(500);

    int *_time = &__time;
    stepSetInt(brewMashStep0Time, 5, _time, 9, 1);
    Serial.println("Setting save stepSetInt " + String(*_time));
    delay(500);

    // guardar?
    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            // Guarda los datos
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP0_TEMP, *_temp);
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP0_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASH_STEP0_TEMP, brewMashStep0Temp);
            EEPROM_readAnything(ADDR_BREW_MASH_STEP0_TIME, brewMashStep0Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewMashStep1()
{
    float *_temp = &__temp;
    stepSetFloat(brewMashStep1Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    int *_time = &__time;
    stepSetInt(brewMashStep1Time, 5, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP1_TEMP, *_temp);
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP1_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASH_STEP1_TEMP, brewMashStep1Temp);
            EEPROM_readAnything(ADDR_BREW_MASH_STEP1_TIME, brewMashStep1Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewMashStep2()
{
    float *_temp = &__temp;
    stepSetFloat(brewMashStep2Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    int *_time = &__time;
    stepSetInt(brewMashStep2Time, 5, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP2_TEMP, *_temp);
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP2_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASH_STEP2_TEMP, brewMashStep2Temp);
            EEPROM_readAnything(ADDR_BREW_MASH_STEP2_TIME, brewMashStep2Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewMashStep3()
{
    float *_temp = &__temp;
    stepSetFloat(brewMashStep3Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    int *_time = &__time;
    stepSetInt(brewMashStep3Time, 5, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP3_TEMP, *_temp);
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP3_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASH_STEP3_TEMP, brewMashStep3Temp);
            EEPROM_readAnything(ADDR_BREW_MASH_STEP3_TIME, brewMashStep3Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewMashStep4()
{
    float *_temp = &__temp;
    stepSetFloat(brewMashStep4Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    int *_time = &__time;
    stepSetInt(brewMashStep4Time, 5, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP4_TEMP, *_temp);
            EEPROM_writeAnything(ADDR_BREW_MASH_STEP4_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASH_STEP4_TEMP, brewMashStep4Temp);
            EEPROM_readAnything(ADDR_BREW_MASH_STEP4_TIME, brewMashStep4Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::brewRecirculation()
{
    /*
     * Muestra el menu de configuracion del recirculado.
     */
    
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(menuItemsBrew[BREW_STAGE_RECIRCULATION][SUBSTAGE_INDEX]);
    lcd.setCursor(0, 1);
    lcd.print("CONT S / ___ min");
    
    _blink = true;
    // muestra los valores prefijatos/seteados
    cursorInt(brewRecirculationTime, 9, 1);

    Serial.println("brewRecirculationTime " + String(brewRecirculationTime));

    bool *_bool = &__bool;
    stepSetBool(brewRecirculationCont, _bool, 5, 1);
    _blink = true;
    cursorBool(*_bool, 5, 1);
    delay(500);

    int *_time = &__time;
    stepSetInt(brewRecirculationTime, 5, _time, 9, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorInt(*_time, 9, 1);
    Serial.println("Setting save stepSetInt for brewRecirculationTime " + String(*_time));
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_RECIRCULATION_CONT, *_bool);
            EEPROM_writeAnything(ADDR_BREW_RECIRCULATION_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_RECIRCULATION_CONT, brewRecirculationCont);
            EEPROM_readAnything(ADDR_BREW_RECIRCULATION_TIME, brewRecirculationTime);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }

    /*
    int subpointer = 1;
    int max_items = Menu::stageSize(BREW_STAGE_RECIRCULATION, MAX_BREW_SUBSTAGE_ITEMS) - 1;
    Menu::stageSelector(BREW_STAGE_RECIRCULATION, subpointer);

    while(!buttons.isSelect())
    {
        if(buttons.isUp() && subpointer > 1)
        {
            subpointer--;
            Menu::stageSelector(BREW_STAGE_RECIRCULATION, subpointer);
        }

        if(buttons.isDown() && subpointer < max_items)
        {
            subpointer++;
            Menu::stageSelector(BREW_STAGE_RECIRCULATION, subpointer, false);
        }

        Serial.println("Configura el tiempo y temperatura del escalon " + String(subpointer) + " - max_items: " + String(max_items));

        if(buttons.isEnter())
        {
            // configura recirculado continuo y/o el tiempo de recirculado
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[BREW_STAGE_RECIRCULATION][subpointer]);
            switch(subpointer-1)
            {
                case 0:
                    Menu::configure_recirculationCont();
                    Menu::stageSelector(BREW_STAGE_RECIRCULATION, subpointer);
                    break;

                case 1:
                    Menu::configure_recirculationTime();
                    Menu::stageSelector(BREW_STAGE_RECIRCULATION, subpointer, false);
                    break;
            }
        }
    }
    */

}

void Menu::brewSparging()
{
    /*
     * Muestra el menu de configuracion del lavado.
     */

    Menu::stageSelector(BREW_STAGE_SPARGING, 1);
}

void Menu::brewBoil()
{
    /*
     * Muestra el menu de configuracion del hervor.
     */

    Menu::stageSelector(BREW_STAGE_BOIL, 1);
}

void Menu::brewWhirlpool()
{
    /*
     * Muestra el menu de configuracion del whirpool.
     */

    Menu::stageSelector(BREW_STAGE_WHIRLPOOL, 1);
}

void Menu::brewCooling()
{
    /*
     * Muestra el menu de configuracion del traspaso.
     */

    Menu::stageSelector(BREW_STAGE_COOLING, 1);
}

void Menu::showMenu()
{
    Menu::home();

    while(true)
    {
        if(Menu::buttons.isUp()) 
        {
            Menu::menuPrev();
        }
        
        if(Menu::buttons.isDown())
        {
            Menu::menuNext();
        }

        // MANUAL/MONITOR
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_MANUAL)
        {
            Serial.println("Entrando a modo monitor");
            float hlt = 0.0;
            float mt = 0.0;
            float fc = 0.0;

            while(!Menu::buttons.isSelect())
            {
                if(sensorsBus.requestTemperaturesByAddress(addrSensorHlt))
                    hlt = sensorsBus.getTempC(addrSensorHlt);

                if(sensorsBus.requestTemperaturesByAddress(addrSensorMt))
                    mt = sensorsBus.getTempC(addrSensorMt);

                if (sensorsBus.requestTemperaturesByAddress(addrSensorFc))
                    fc = sensorsBus.getTempC(addrSensorFc);
                
                Menu::monitor(hlt, mt, fc, Menu::manual(_rele_r1_pwm_pin, _rele_pump_pwm_pin));
            }
            Menu::home();
        }

        // BREW
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_BREW)
        {
            Serial.println("Entrando a modo brew");
            Menu::brew();
            Menu::home();
        }

        // FERMENTATION
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_FERMENTATION)
        {
            Serial.println("Entrando a modo fermentador");
            //Menu::fermentator();
        }

        // CONFIGURATION
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_CONFIGURATION)
        {
            Serial.println("Entrando a modo configuracion");
            //Core::discoverSensors();
        }

    }
}

/*
 * DEPRECATED
void Menu::showMenu()
{    
    Menu::home();

    while(true)
    {

        if(buttons.isUp()) 
        {
            Menu::menuPrev();
        }
        
        if(buttons.isDown())
        {
            Menu::menuNext();
        }

        if(buttons.isEnter() && pointer_cursor == MENU_MANUAL)
        {
            Serial.println("Entrando a modo monitor");
            Menu::monitor(99.99, 99.99, 99.99);
            Menu::home();
        }

        if(buttons.isEnter() && pointer_cursor == MENU_BREW)
        {
            Serial.println("Entrando a modo brew");
            Menu::brew();
            Menu::home();
        }

        if(buttons.isEnter() && pointer_cursor == MENU_FERMENTATION)
        {
            Serial.println("Entrando a modo fermentador");
            //Menu::fermentator();
        }

        if(buttons.isEnter() && pointer_cursor == MENU_CONFIGURATION)
        {
            Serial.println("Entrando a modo configuracion");
            //Menu::home();
        }

    }
}
*/
