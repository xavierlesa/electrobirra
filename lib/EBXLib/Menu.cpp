#include "Arduino.h"
#include <LiquidCrystal.h>
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buttons.h"
#include "Menu.h"

// libs
#include <avr/pgmspace.h>
#include <MemoryFree.h>
#include <pgmStrToRAM.h>

Menu::Menu(
        uint8_t BUTTONS_PIN, 
        uint8_t LCD_RS,
        uint8_t LCD_ENABLE,
        uint8_t LCD_D4,
        uint8_t LCD_D5,
        uint8_t LCD_D6,
        uint8_t LCD_D7,
        uint8_t LCD_COLUMNS, 
        uint8_t LCD_ROWS, 
        uint8_t LCD_BACKLIGHT,
        uint8_t SENSOR_ONEWIRE_PIN,
        uint8_t RELE_R1_PWM_PIN,
        uint8_t RELE_R2_PWM_PIN,
        uint8_t RELE_HT_PWM_PIN,
        uint8_t RELE_LT_PWM_PIN,
        uint8_t RELE_PUMP_A_PWM_PIN,
        uint8_t RELE_PUMP_B_PWM_PIN,
        bool RELE_NA
        ): 
    buttons(BUTTONS_PIN, 50),
    lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7),
    oneWire(SENSOR_ONEWIRE_PIN),
    sensorsBus(&oneWire)
{
    // backlight
    _lcd_columns = LCD_COLUMNS;
    _lcd_rows = LCD_ROWS;

    // PINes
    _rele_r1_pwm_pin = RELE_R1_PWM_PIN;
    _rele_r2_pwm_pin = RELE_R2_PWM_PIN;
    _rele_ht_pwm_pin = RELE_HT_PWM_PIN;
    _rele_lt_pwm_pin = RELE_LT_PWM_PIN;
    _rele_pump_a_pwm_pin = RELE_PUMP_A_PWM_PIN;
    _rele_pump_b_pwm_pin = RELE_PUMP_B_PWM_PIN;
    _RELE_NA = RELE_NA;

    // LCD chars
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
 
    // Carga los items para la creacion del menu
    menuItems[MENU_MANUAL] = "Manual/Monitor";
    menuItems[MENU_BREW] = "Coccion";
    menuItems[MENU_FERMENTATION] = "Fermentador";
    menuItems[MENU_CONFIGURATION] = "Configuracion";
    menuItems[MENU_START] = "Comenzar !!!";

    menuItemsBrew[BREW_STAGE_MASH][SUBSTAGE_INDEX] = "Macerado";
    menuItemsBrew[BREW_STAGE_RECIRCULATION][SUBSTAGE_INDEX] = "Recirculado";
    menuItemsBrew[BREW_STAGE_SPARGING][SUBSTAGE_INDEX] = "Lavado";
    menuItemsBrew[BREW_STAGE_BOIL][SUBSTAGE_INDEX] = "Hervor";
    menuItemsBrew[BREW_STAGE_WHIRLPOOL][SUBSTAGE_INDEX] = "Whirlpool";
    menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][SUBSTAGE_INDEX] = "Opciones offset";

    menuItemsBrew[BREW_STAGE_MASH][1] = "Primer Escalon";
    menuItemsBrew[BREW_STAGE_MASH][2] = "Segundo Escalon";
    menuItemsBrew[BREW_STAGE_MASH][3] = "Tercero Escalon";
    menuItemsBrew[BREW_STAGE_MASH][4] = "Cuarto Escalon";
    menuItemsBrew[BREW_STAGE_MASH][5] = "Quinto Escalon";

    menuItemsBrew[BREW_STAGE_BOIL][SUBSTAGE_INDEX] = "Hervor";
    menuItemsBrew[BREW_STAGE_BOIL][1] = "Tiempo de hervor";
    menuItemsBrew[BREW_STAGE_BOIL][2] = "Lupulos 1";
    menuItemsBrew[BREW_STAGE_BOIL][3] = "Lupulos 2";
    menuItemsBrew[BREW_STAGE_BOIL][4] = "Lupulos 3";
    menuItemsBrew[BREW_STAGE_BOIL][5] = "Lupulos 4";

    menuItemsBrew[BREW_STAGE_WHIRLPOOL][SUBSTAGE_INDEX] = "Whirlpool";
    menuItemsBrew[BREW_STAGE_WHIRLPOOL][1] = "Tiempo de whirlpool";

    menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][SUBSTAGE_INDEX] = "Opciones offset";
    menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][1] = "Offset Macerador";
    menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][2] = "Offset Mash-in";
    menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][3] = "Offset Lavado";
    //menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][3] = "Offset Mash-out";

    _HOT_ELEMENTS_STATUS = false;
    _PUMPS_STATUS = false;
    _SPARGING_WATER_IN_PROGRESS = false;
}

///////////////////////////////////////////////////////////////////////////////
// MENU HELPERS
///////////////////////////////////////////////////////////////////////////////

void Menu::stageSelector(uint8_t stage, uint8_t subpointer, bool asc)
{
    /*
     * Selector de stage
     */

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
    lcd.print(F("    Guardar?    "));
    lcd.setCursor(0, 1);
    lcd.write(arrowLeft);
    lcd.setCursor(15, 1);
    lcd.write(arrowRight);
    lcd.setCursor(1, 1);
    lcd.print(F("NO          SI"));
}

void Menu::_showSaved()
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("  Guardando...  "));
    delay(1000);
}

void Menu::cursorFloat(float _f, uint8_t cur_x, uint8_t cur_y)
{

    char _buffer[5];
    dtostrf(_f, 3, 1, _buffer);
    lcd.setCursor(cur_x, cur_y);
    if(_f < 10)
    { 
        lcd.setCursor(cur_x, cur_y);
        if(_blink)
        {
            lcd.print(F("__")); 
            lcd.print(_buffer);
        }
        else lcd.print(F("___._"));
    }
    if(_f >= 10 && _f < 100)
    {
        lcd.setCursor(cur_x, cur_y);
        if(_blink)
        {
            lcd.print(F("_"));
            lcd.print(_buffer);
        }
        else lcd.print(F("___._"));
    }
    if(_f >= 100)
    {
        lcd.setCursor(cur_x, cur_y);
        if(_blink) lcd.print(_buffer);
        else lcd.print(F("___._"));
    }
    _blink = !_blink;
}

void Menu::stepSetFloat(float _default, float _step, float *buffer, uint8_t cur_x, uint8_t cur_y)
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
    }

    *buffer = _default;
}

void Menu::cursorInt(uint8_t _i, uint8_t cur_x, uint8_t cur_y)
{
    lcd.setCursor(cur_x, cur_y);
    if(_i < 10)
    { 
        if(_blink)
        {
            lcd.print(F("__"));
            lcd.print(_i);
        }
        else lcd.print(F("___"));
    }
    if(_i >= 10 && _i < 100)
    {
        if(_blink)
        {
            lcd.print(F("_"));
            lcd.print(_i);
        }
        else lcd.print(F("___"));
    }
    if(_i >= 100)
    {
        if(_blink) lcd.print(_i);
        else lcd.print(F("___"));
    }
    _blink = !_blink;
}

void Menu::stepSetInt(uint8_t _default, uint8_t _step, uint8_t *buffer, uint8_t cur_x, uint8_t cur_y)
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
    }

    *buffer = _default;
}

void Menu::cursorBool(bool _i, uint8_t cur_x, uint8_t cur_y)
{
    lcd.setCursor(cur_x, cur_y);
    if(_blink) lcd.print( (_i ? "S" : "N") );
    else lcd.print(F("_"));
    _blink = !_blink;
}

void Menu::stepSetBool(bool _default, bool *buffer, uint8_t cur_x, uint8_t cur_y)
{
    while(!buttons.isBack() && !buttons.isSelect() && !buttons.isEnter())
    {
        if(buttons.isUp()) _default = true;
        if(buttons.isDown()) _default = false;
        Menu::cursorBool(_default, cur_x, cur_y);
    }

    *buffer = _default;
}

///////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////
void Menu::loadData()
{
    /*
     * Carga los datos de configuracion desde la EEPROM
     */

    //DEBUG_PRINT(freeMemory());
    //DEBUG_PRINT(F("Cargando los datos de configuracion desde la EEPROM"));

    EEPROM_readAnything(ADDR_SENSOR_HTL, addrSensorHlt);
    EEPROM_readAnything(ADDR_SENSOR_MT, addrSensorMt);
    EEPROM_readAnything(ADDR_SENSOR_FC, addrSensorFc);

    addrSensorHlt[0] = 0x28;
    addrSensorHlt[1] = 0xFF;
    addrSensorHlt[2] = 0xBA;
    addrSensorHlt[3] = 0x51;
    addrSensorHlt[4] = 0x4B;
    addrSensorHlt[5] = 0x04;
    addrSensorHlt[6] = 0x00;
    addrSensorHlt[7] = 0x26;

    //addrSensorMt[0] = 0x28;
    //addrSensorMt[1] = 0xFF;
    //addrSensorMt[2] = 0xBA;
    //addrSensorMt[3] = 0x51;
    //addrSensorMt[4] = 0x4B;
    //addrSensorMt[5] = 0x04;
    //addrSensorMt[6] = 0x00;
    //addrSensorMt[7] = 0x26;

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

    EEPROM_readAnything(ADDR_BREW_SPARGING_TEMP, brewSpargingTemp);
    EEPROM_readAnything(ADDR_BREW_SPARGING_TIME, brewSpargingTime);

    EEPROM_readAnything(ADDR_BREW_BOIL_TIME, brewBoilTime);
    EEPROM_readAnything(ADDR_BREW_BOIL_HOPS0_TIME, brewBoilHops0Time);
    EEPROM_readAnything(ADDR_BREW_BOIL_HOPS1_TIME, brewBoilHops1Time);
    EEPROM_readAnything(ADDR_BREW_BOIL_HOPS2_TIME, brewBoilHops2Time);
    EEPROM_readAnything(ADDR_BREW_BOIL_HOPS3_TIME, brewBoilHops3Time);
    // DESACTIVADO POR SOBREDIMENSION DEL ARRAY
    //EEPROM_readAnything(ADDR_BREW_BOIL_HOPS4_TIME, brewBoilHops4Time);

    EEPROM_readAnything(ADDR_BREW_WHIRLPOOL_TIME, brewWhirlpoolTime);
    EEPROM_readAnything(ADDR_BREW_WHIRLPOOL_DELAY, brewWhirlpoolDelay);

    //EEPROM_readAnything(ADDR_BREW_COOLING_TIME, brewCoolingTime);

    EEPROM_readAnything(ADDR_BREW_MASH_TEMP_OFFSET, brewMashTempOffset);
    EEPROM_readAnything(ADDR_BREW_MASHIN_TEMP_OFFSET, brewMashInTempOffset);
    EEPROM_readAnything(ADDR_BREW_SPARGING_TEMP_OFFSET, brewSpargingTempOffset);

    if(isnan(brewMashStep0Temp) || isinf(brewMashStep0Temp)) brewMashStep0Temp = 66.5;
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

    if(isnan(brewSpargingTemp)) brewSpargingTemp = 72;
    // Calcula en base al flujo de la carga del agua cuanto tiempo debe lavar
    if(isinf(brewSpargingTime) || brewSpargingTime < 0) brewSpargingTime = 35;

    if(isinf(brewBoilTime) || brewBoilTime < 0) brewBoilTime = 90;
    if(isinf(brewBoilHops0Time) || brewBoilHops0Time < 0) brewBoilHops0Time = 30;
    if(isinf(brewBoilHops1Time) || brewBoilHops1Time < 0) brewBoilHops1Time = 45;
    if(isinf(brewBoilHops2Time) || brewBoilHops2Time < 0) brewBoilHops2Time = 60;
    if(isinf(brewBoilHops3Time) || brewBoilHops3Time < 0) brewBoilHops3Time = 90;
    //if(isinf(brewBoilHops4Time) || brewBoilHops4Time < 0) brewBoilHops4Time = 90;

    if(isinf(brewWhirlpoolTime) || brewWhirlpoolTime < 0) brewWhirlpoolTime = 5;
    if(isinf(brewWhirlpoolDelay) || brewWhirlpoolDelay < 0) brewWhirlpoolDelay = 15;

    if(isnan(brewMashTempOffset)) brewMashTempOffset = 3.0;
    if(isnan(brewMashInTempOffset)) brewMashInTempOffset = 1.0;
    if(isnan(brewSpargingTempOffset)) brewSpargingTempOffset = 2.0;
    
    //DEBUG_PRINT(freeMemory());
}

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
    //DEBUG_PRINT(freeMemory());
}

uint8_t Menu::manual(uint8_t upPin, uint8_t downPin) //, uint8_t enterPin, uint8_t backPin)
{
    /*
     * Ejecuta el modo manual, para controlos los dispositivos
     *
     * UP, controla las resistencias para calentar
     * DOWN, controla la bomba de recirtculado
     * ENTER y BACK reservados para futuros usos.
     *
     * Devuelve 4 bits (un uint8_t), indicando el estado segun el bit
     *
     * B10XX upPin on
     *  ^^^^
     *  ||||
     *  |||+- p0 backPin (reservado)
     *  ||+-- p1 enterPin (reservado)
     *  |+--- p2 downPin
     *  +---- p3 upPin
     */

    uint8_t r = 0, p0, p1, p2, p3;

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

void Menu::monitor(float hlt, float mt, float fc, uint8_t _flags)
{
    /*
     * Muestra el monitor de temperatura
     * hasta que se presione la tecla "select"
     * y titila en el control manual activo
     *
     * 0123456789ABCDEF
     * MT/FC 67.6 HOT *
     * HLT  67.6 PUMP *
     * HLT  MT   FC   H
     * 67.5 67.5 67.5 P
     */

    char hlt_buffer[5]; // 999.9 => 5 caracteres incluido el "."
    char mt_buffer[5];
    char fc_buffer[5];

    dtostrf(hlt, 3, 1, hlt_buffer);
    dtostrf(mt, 3, 1, mt_buffer);
    dtostrf(fc, 3, 1, fc_buffer);

    lcd.clear();
    lcd.print(F("HTL  MT   FC"));
    lcd.setCursor(15,0);
    if(_flags & B1000) lcd.print(F("H"));
    else lcd.print(F("h"));

    lcd.setCursor(0,1);
    lcd.print(hlt_buffer);
    lcd.setCursor(5,1);
    lcd.print(mt_buffer);
    lcd.setCursor(10,1);
    lcd.print(fc_buffer);
    lcd.setCursor(15,1);
    if(_flags & B0100) lcd.print(F("P"));
    else lcd.print(F("p"));
}


void Menu::_showBrewMenu(uint8_t pointer)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.write(cursorDot);
    lcd.print(menuItemsBrew[pointer][SUBSTAGE_INDEX]);
    lcd.setCursor(0, 1);
    lcd.print(menuItemsBrew[pointer+1][SUBSTAGE_INDEX]); 
    // serial monitor
    //DEBUG_PRINT(freeMemory());
}

void Menu::brewMenu(uint8_t pointer)
{
    /*
     * Muestra el programa de coccion y configuracion.
     * Desde aqui se configuran las variables de coccion.
     */

    //DEBUG_PRINT(freeMemory());
    Menu::_showBrewMenu(pointer);
    delay(250);

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
            //DEBUG_PRINT(F("Entrando a modo macerado"));
            Menu::brewMash();
            Menu::_showBrewMenu(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_RECIRCULATION)
        {
            //DEBUG_PRINT(F("Entrando a modo recirculado"));
            Menu::brewRecirculation();
            Menu::_showBrewMenu(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_SPARGING)
        {
            //DEBUG_PRINT(F("Entrando a modo lavado"));
            Menu::brewSparging();
            Menu::_showBrewMenu(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_BOIL)
        {
            //DEBUG_PRINT(F("Entrando a modo hervor"));
            Menu::brewBoil();
            Menu::_showBrewMenu(pointer);
        }

        if(buttons.isEnter() && pointer == BREW_STAGE_WHIRLPOOL)
        {
            //DEBUG_PRINT(F("Entrando a modo whirpool"));
            Menu::brewWhirlpool();
            Menu::_showBrewMenu(pointer);
        }

        //if(buttons.isEnter() && pointer == BREW_STAGE_COOLING)
        //{
        //    DEBUG_PRINT(F("Entrando a modo enfriado"));
        //    Menu::brewCooling();
        //    Menu::_showBrewMenu(pointer);
        //}

        if(buttons.isEnter() && pointer == BREW_STAGE_OPTIONS_OFFSET)
        {
            //DEBUG_PRINT(F("Entrando a modo opciones de offset"));
            Menu::brewOptionsOffset();
            Menu::_showBrewMenu(pointer);
        }
    }
}

void Menu::_showBrewMash(uint8_t pointer, bool asc)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    if(asc) 
    {
        lcd.write(cursorDot);
        lcd.print(menuItemsBrew[BREW_STAGE_MASH][pointer]);
        lcd.setCursor(0, 1);
        lcd.print(menuItemsBrew[BREW_STAGE_MASH][pointer+1]);
    }
    else
    {
        lcd.print(menuItemsBrew[BREW_STAGE_MASH][pointer-1]);
        lcd.setCursor(0, 1);
        lcd.write(cursorDot);
        lcd.print(menuItemsBrew[BREW_STAGE_MASH][pointer]);
    }
}

void Menu::brewMash(uint8_t pointer)
{
    /*
     * Muestra el menu de configuracion del macerado.
     *
     */
    
    Menu::_showBrewMash(pointer);
    delay(250);

    while(!buttons.isSelect())
    {
        if(buttons.isUp() && pointer > 1)
        {
            pointer--;
            Menu::_showBrewMash(pointer);
        }

        if(buttons.isDown() && pointer < MAX_BREW_MASH_ITEMS - 1)
        {
            pointer++;
            Menu::_showBrewMash(pointer, false);
        }

        if(buttons.isEnter())
        {
            // configura el tiempo y temperatura del escalon n
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[BREW_STAGE_MASH][pointer]);
            lcd.setCursor(0, 1);
            lcd.print(F("___._C / ___ min"));
            
            //DEBUG_PRINT(F("EEPROM_writeAnything brewMashStep"));

            //DEBUG_PRINT(freeMemory());
            switch(pointer)
            {
                case 1:
                    // muestra los valores prefijatos/seteados
                    //DEBUG_PRINT(F("brewMashStep0Temp config. temp actual: "));
                    //DEBUG_PRINT(brewMashStep0Temp);
                    //DEBUG_PRINT(F("C / time: "));
                    //DEBUG_PRINT(brewMashStep0Time);
                    //DEBUG_PRINT(F("sale de: cursorFloat(brewMashStep0Temp, 0, 1)"));

                    cursorFloat(brewMashStep0Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep0Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep0();
                    Menu::_showBrewMash(pointer);
                    break;

                case 2:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep1Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep1Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep1();
                    Menu::_showBrewMash(pointer);
                    break;

                case 3:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep2Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep2Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep2();
                    Menu::_showBrewMash(pointer);
                    break;

                case 4:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep3Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep3Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep3();
                    Menu::_showBrewMash(pointer);
                    break;

                case 5:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashStep4Temp, 0, 1);
                    _blink = true;
                    cursorInt(brewMashStep4Time, 9, 1);
                    // configura los valores
                    Menu::configure_brewMashStep4();
                    // ultimo item muestra el menu con modo up
                    Menu::_showBrewMash(pointer, false);
                    break;
            }
        } 
    }
}

void Menu::configure_brewMashStep0()
{
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashStep0Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    //DEBUG_PRINT(F("Setting save stepSetFloat: "));
    //DEBUG_PRINT(*_temp);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewMashStep0Time, 5, _time, 9, 1);
    //DEBUG_PRINT(F("Setting save stepSetInt: "));
    //DEBUG_PRINT(*_time);
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
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashStep1Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
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
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashStep2Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
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
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashStep3Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
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
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashStep4Temp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
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
    
    // Memory
    //DEBUG_PRINT(freeMemory());

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(menuItemsBrew[BREW_STAGE_RECIRCULATION][SUBSTAGE_INDEX]);
    lcd.setCursor(0, 1);
    lcd.print(F("CONT S / ___ min"));
    
    _blink = true;
    // muestra los valores prefijatos/seteados
    cursorInt(brewRecirculationTime, 9, 1);

    //DEBUG_PRINT(F("brewRecirculationTime ")); 
    //DEBUG_PRINT(brewRecirculationTime);

    bool __bool = true;
    bool *_bool = &__bool;
    stepSetBool(brewRecirculationCont, _bool, 5, 1);
    _blink = true;
    cursorBool(*_bool, 5, 1);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewRecirculationTime, 5, _time, 9, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorInt(*_time, 9, 1);
    //DEBUG_PRINT(F("Setting save stepSetInt for brewRecirculationTime: "));
    //DEBUG_PRINT(*_time);
    delay(500);

    //DEBUG_PRINT(freeMemory());
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

    // Memory
    //DEBUG_PRINT(freeMemory());
}

void Menu::brewSparging()
{
    /*
     * Muestra el menu de configuracion del lavado.
     */

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(menuItemsBrew[BREW_STAGE_SPARGING][SUBSTAGE_INDEX]);
    lcd.setCursor(0, 1);
    lcd.print(F("___._C / ___ min"));
    
    cursorFloat(brewSpargingTemp, 0, 1);
    _blink = true;
    // muestra los valores prefijatos/seteados
    cursorInt(brewSpargingTime, 9, 1);
    
    //DEBUG_PRINT(F("brewSpargingTemp: "));
    //DEBUG_PRINT(brewSpargingTemp);
    //DEBUG_PRINT(F(" brewSpargingTime: "));
    //DEBUG_PRINT(brewSpargingTime);

    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewSpargingTemp, 0.5, _temp, 0, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 0, 1);
    delay(500);

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewSpargingTime, 5, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_SPARGING_TEMP, *_temp);
            EEPROM_writeAnything(ADDR_BREW_SPARGING_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_SPARGING_TEMP, brewSpargingTemp);
            EEPROM_readAnything(ADDR_BREW_SPARGING_TIME, brewSpargingTime);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::brewBoil()
{
    /*
     * Muestra el menu de configuracion del hervor.
     *
     * Primero se configura el tiempo total de Hervor, y es evaluado
     * a partir de que la temp. llega a los 100ºC.
     *
     * Llegado a los 100ºC comienza a contar el tiempo de Hervor y las 
     * adhesiones de lupulos.
     *
     * Las adhesiones se cuentan partiendo del tiempo final de hervor al 
     * inicial, es decir si seteamos 90 min totales de hervor, la tabla seria:
     *
     * 1- 90 min. (90 min. antes de terminar el hervor)
     * 2- 60 min.
     * 3- 30 min.
     * 4- 10 min.
     *
     */

    uint8_t subpointer = 1;
    uint8_t max_items = 5; //BREW_STAGE_BOIL
    Menu::stageSelector(BREW_STAGE_BOIL, subpointer);

    //DEBUG_PRINT(F("Configura el tiempo y temperatura del escalon: "));
    //DEBUG_PRINT(freeMemory());

    while(!buttons.isSelect())
    {
        if(buttons.isUp() && subpointer > 1)
        {
            subpointer--;
            Menu::stageSelector(BREW_STAGE_BOIL, subpointer);
        }

        if(buttons.isDown() && subpointer < max_items)
        {
            subpointer++;
            Menu::stageSelector(BREW_STAGE_BOIL, subpointer, false);
        }

        if(buttons.isEnter())
        {
            // configura el tiempo y temperatura del escalon n
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[BREW_STAGE_BOIL][subpointer]);
            lcd.setCursor(0, 1);
            lcd.print(F("Tiempo:  ___ min"));
            
            //DEBUG_PRINT(F("EEPROM_writeAnything brewMashStep (subpointer): "));
            //DEBUG_PRINT(subpointer); 

            switch(subpointer-1)
            {
                case 0:
                    // muestra los valores prefijatos/seteados
                    cursorInt(brewBoilTime, 9, 1);
                    _blink = true;
                    // configura los valores
                    Menu::configure_brewBoilTime();
                    Menu::stageSelector(BREW_STAGE_BOIL, subpointer);
                    break;

                case 1:
                    // configura los valores
                    Menu::configure_brewBoilHops0();
                    // ultimo item muestra el menu con modo up
                    Menu::stageSelector(BREW_STAGE_BOIL, subpointer);
                    break;

                case 2:
                    // configura los valores
                    Menu::configure_brewBoilHops1();
                    // ultimo item muestra el menu con modo up
                    Menu::stageSelector(BREW_STAGE_BOIL, subpointer);
                    break;

                case 3:
                    // configura los valores
                    Menu::configure_brewBoilHops2();
                    // ultimo item muestra el menu con modo up
                    Menu::stageSelector(BREW_STAGE_BOIL, subpointer);
                    break;

                case 4:
                    // configura los valores
                    Menu::configure_brewBoilHops3();
                    // ultimo item muestra el menu con modo up
                    Menu::stageSelector(BREW_STAGE_BOIL, subpointer, false);
                    break;

                // DESACTIVADO POR SOBREDIMENSION DEL ARRAY
                //case 5:
                //    // configura los valores
                //    Menu::configure_brewBoilHops4();
                //    // ultimo item muestra el menu con modo up
                //    Menu::stageSelector(BREW_STAGE_BOIL, subpointer, false);
                //    break;
            }
        }
    }

    // Calculo cuanto me queda de ram despues de esta funcion
    //DEBUG_PRINT(freeMemory());
}

void Menu::configure_brewBoilTime()
{
    /*
     * Setea el tiempo total de la coccion a partir del Hervor (mas de 100ºC)
     */

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewBoilTime, 5, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_BOIL_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_BOIL_TIME, brewBoilTime);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
    //free(_time);
}

void Menu::configure_brewBoilHops0()
{
    /*
     * Configura las adesiones de lupulos y escalones de tiempo
     * El tiempo de adhesion se calcula desde a partir de caunto falta para finaliar el hervor,
     * es decir 90 min, significa a 90 min antes de terminar; 10 min es a 10 min antes de terminar, etc.
     */

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewBoilHops0Time, 1, _time, 9, 1);
    delay(500);


    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_BOIL_HOPS0_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_BOIL_HOPS0_TIME, brewBoilHops0Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewBoilHops1()
{
    /*
     * Configura las adesiones de lupulos y escalones de tiempo
     */

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewBoilHops1Time, 1, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_BOIL_HOPS1_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_BOIL_HOPS1_TIME, brewBoilHops1Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewBoilHops2()
{
    /*
     * Configura las adesiones de lupulos y escalones de tiempo
     */

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewBoilHops2Time, 1, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_BOIL_HOPS2_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_BOIL_HOPS2_TIME, brewBoilHops2Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewBoilHops3()
{
    /*
     * Configura las adesiones de lupulos y escalones de tiempo
     */

    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewBoilHops3Time, 1, _time, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_BOIL_HOPS3_TIME, *_time);
            // reload
            EEPROM_readAnything(ADDR_BREW_BOIL_HOPS3_TIME, brewBoilHops3Time);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

// DESACTIVADO POR SOBREDIMENSION DEL ARRAY
//void Menu::configure_brewBoilHops4()
//{
//    /*
//     * Configura las adesiones de lupulos y escalones de tiempo
//     */
//
//    uint8_t __time = 0;
//    uint8_t *_time = &__time;
//    stepSetInt(brewBoilHops4Time, 1, _time, 9, 1);
//    delay(500);
//
//    while(true)
//    {
//        Menu::_showSave();
//        if(buttons.isEnter())
//        {
//            EEPROM_writeAnything(ADDR_BREW_BOIL_HOPS4_TIME, *_time);
//            // reload
//            EEPROM_readAnything(ADDR_BREW_BOIL_HOPS4_TIME, brewBoilHops4Time);
//            Menu::_showSaved();
//            break;
//        }
//        if(buttons.isBack()) break;
//    }
//}

void Menu::brewWhirlpool()
{
    /*
     * Muestra el menu de configuracion del whirpool.
     */

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("Tiempo:  ___ min"));
    lcd.setCursor(0, 1);
    lcd.print(F("Espera:  ___ min"));
    
    _blink = true;
    // muestra los valores prefijatos/seteados
    cursorInt(brewWhirlpoolDelay, 9, 1);
    
    //DEBUG_PRINT(F("brewWhirlpoolTime: "));
    //DEBUG_PRINT(brewWhirlpoolTime);
    //DEBUG_PRINT(F(" brewWhirlpoolDelay: "));
    //DEBUG_PRINT(brewWhirlpoolDelay);

    _blink = true;
    uint8_t __time = 0;
    uint8_t *_time = &__time;
    stepSetInt(brewWhirlpoolTime, 1, _time, 9, 0);
    delay(500);

    _blink = true;
    uint8_t __delay = 0;
    uint8_t *_delay = &__delay;
    stepSetInt(brewWhirlpoolDelay, 1, _delay, 9, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_WHIRLPOOL_TIME, *_time);
            EEPROM_writeAnything(ADDR_BREW_WHIRLPOOL_DELAY, *_delay);
            // reload
            EEPROM_readAnything(ADDR_BREW_WHIRLPOOL_TIME, brewWhirlpoolTime);
            EEPROM_readAnything(ADDR_BREW_WHIRLPOOL_DELAY, brewWhirlpoolDelay);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

//void Menu::brewCooling()
//{
//    /*
//     * Muestra el menu de configuracion del traspaso.
//     */
//
//    Menu::stageSelector(BREW_STAGE_COOLING, 1);
//}

void Menu::brewOptionsOffset()
{
    /*
     * Muestra el menu de opciones de offset
     */

    uint8_t subpointer = 1;
    uint8_t max_items = 3; //BREW_STAGE_OPTIONS_OFFSET
    Menu::stageSelector(BREW_STAGE_OPTIONS_OFFSET, subpointer);
        
    //DEBUG_PRINT(freeMemory());
    //DEBUG_PRINT(F("Configura offset de temperatura: "));

    while(!buttons.isSelect())
    {
        if(buttons.isUp() && subpointer > 1)
        {
            subpointer--;
            Menu::stageSelector(BREW_STAGE_OPTIONS_OFFSET, subpointer);
        }

        if(buttons.isDown() && subpointer < max_items)
        {
            subpointer++;
            Menu::stageSelector(BREW_STAGE_OPTIONS_OFFSET, subpointer, false);
        }

        if(buttons.isEnter())
        {
            // configura el tiempo y temperatura del escalon n

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[BREW_STAGE_OPTIONS_OFFSET][subpointer]);
            lcd.setCursor(0, 1);
            lcd.print(F("Offset:   ___._C"));
            
            //DEBUG_PRINT(F("EEPROM_writeAnything brewMashStep (subpointer): "));
            //DEBUG_PRINT(subpointer); 

            switch(subpointer-1)
            {
                case 0:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashTempOffset, 10, 1);
                    _blink = true;
                    // configura los valores
                    Menu::configure_brewMashTempOffset();
                    Menu::stageSelector(BREW_STAGE_OPTIONS_OFFSET, subpointer);
                    break;

                case 1:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewMashInTempOffset, 10, 1);
                    _blink = true;
                    // configura los valores
                    Menu::configure_brewMashInTempOffset();
                    Menu::stageSelector(BREW_STAGE_OPTIONS_OFFSET, subpointer);
                    break;

                case 2:
                    // muestra los valores prefijatos/seteados
                    cursorFloat(brewSpargingTempOffset, 10, 1);
                    _blink = true;
                    // configura los valores
                    Menu::configure_brewSpargingTempOffset();
                    Menu::stageSelector(BREW_STAGE_OPTIONS_OFFSET, subpointer, false);
                    break;
            }
        }
    }

    // calculo la memoria libre para saber cuanto consume esta funcion
    //DEBUG_PRINT(freeMemory());
}

void Menu::configure_brewMashTempOffset()
{
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashTempOffset, 0.5, _temp, 10, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 10, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_MASH_TEMP_OFFSET, *_temp);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASH_TEMP_OFFSET, brewMashTempOffset);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewMashInTempOffset()
{
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewMashInTempOffset, 0.5, _temp, 10, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 10, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_MASHIN_TEMP_OFFSET, *_temp);
            // reload
            EEPROM_readAnything(ADDR_BREW_MASHIN_TEMP_OFFSET, brewMashInTempOffset);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::configure_brewSpargingTempOffset()
{
    float __temp = 0.0;
    float *_temp = &__temp;
    stepSetFloat(brewSpargingTempOffset, 0.5, _temp, 10, 1);
    // muestra el valor por si queda en _blink false
    _blink = true;
    cursorFloat(*_temp, 10, 1);
    delay(500);

    while(true)
    {
        Menu::_showSave();
        if(buttons.isEnter())
        {
            EEPROM_writeAnything(ADDR_BREW_SPARGING_TEMP_OFFSET, *_temp);
            // reload
            EEPROM_readAnything(ADDR_BREW_SPARGING_TEMP_OFFSET, brewSpargingTempOffset);
            Menu::_showSaved();
            break;
        }
        if(buttons.isBack()) break;
    }
}

void Menu::showMenu()
{
    //DEBUG_PRINT(freeMemory());
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
            //DEBUG_PRINT(freeMemory());
            //DEBUG_PRINT(F("Entrando a modo monitor"));

            uint8_t totalDevices = sensorsBus.getDeviceCount();

            //DEBUG_PRINT(F("Busca los sensores (DS18*) conectados y devuelve las direcciones + indice"));
            //DEBUG_PRINT(F("Buscando dispositivos..."));
            //DEBUG_PRINT(F("Se encontraron "));
            //DEBUG_PRINT(totalDevices);
            //DEBUG_PRINT(F(" dispositivos."));

            float hlt = 0.0;
            float mt = 0.0;
            float fc = 0.0;
            
            while(!Menu::buttons.isSelect())
            {
                if(totalDevices)
                {
                    if(sensorsBus.requestTemperaturesByAddress(addrSensorHlt))
                        hlt = sensorsBus.getTempC(addrSensorHlt);
                    
                    if(sensorsBus.requestTemperaturesByAddress(addrSensorMt))
                        mt = sensorsBus.getTempC(addrSensorMt);

                    if (sensorsBus.requestTemperaturesByAddress(addrSensorFc))
                        fc = sensorsBus.getTempC(addrSensorFc);
                }

                //DEBUG_PRINT(freeMemory());
                Menu::monitor(hlt, mt, fc, Menu::manual(_rele_r1_pwm_pin, _rele_pump_a_pwm_pin));
            }

            //free(htl);
            //free(mt);
            //free(fc);
            Menu::home();
        }

        // BREW
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_BREW)
        {
            //DEBUG_PRINT(freeMemory());
            //DEBUG_PRINT(F("Entrando a modo brew"));
            Menu::brewMenu();
            Menu::home();
        }

        // FERMENTATION
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_FERMENTATION)
        {
            DEBUG_PRINT(freeMemory());
            DEBUG_PRINT(F("Entrando a modo fermentador"));
            //Menu::fermentator();
        }

        // CONFIGURATION
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_CONFIGURATION)
        {
            DEBUG_PRINT(freeMemory());
            DEBUG_PRINT(F("Entrando a modo configuracion"));
        }

        // COMENZAR !!!
        if(Menu::buttons.isEnter() && Menu::pointer_cursor == MENU_START)
        {
            DEBUG_PRINT(freeMemory());
            DEBUG_PRINT(F("Comienza la coccion"));
            Menu::startBrew();
            Menu::home();
        }
    }
}

void Menu::startPump(uint8_t _pump)
{
    digitalWrite(_pump, _RELE_NA ^ HIGH);
}

void Menu::stopPump(uint8_t _pump)
{
    digitalWrite(_pump, _RELE_NA ^ LOW);
}

void Menu::startHotElement(uint8_t _he)
{
    digitalWrite(_he, _RELE_NA ^ HIGH);
}

void Menu::stopHotElement(uint8_t _he)
{
    digitalWrite(_he, _RELE_NA ^ LOW);
}

void Menu::purgePump(uint8_t _pump)
{
    uint8_t currentState = digitalRead(_pump);
    // Si la bomba ya esta corriendo, simplemente continua
    // sino purga 3 veces
    if(!_RELE_NA ^ currentState)
    {
        DEBUG_PRINT(F("Purga la bomba para comenzar a operar"));
        for(uint8_t i = 0; i < 5; i++){
            Menu::startPump(_pump);
            delay(250 * i+1);
            Menu::stopPump(_pump);
            delay(100 * i+1);
            //DEBUG_PRINT(F("PURGAR !"));
        }
    }
}

bool Menu::checkTemp(DeviceAddress addrSensor, float _temp, float *buffer, 
        bool recirculationCont, uint8_t _hotElement, uint8_t _pump)
{
    float temp;
    DEBUG_PRINT(F("obtiene la temp actual del dispositivo"));
    if(sensorsBus.requestTemperaturesByAddress(addrSensor))
    {
        temp = sensorsBus.getTempC(addrSensor);
        *buffer = temp;
        if(temp < _temp)
        {
            Menu::purgePump(_pump);
            Menu::startPump(_pump);
            Menu::startHotElement(_hotElement);
            return false;
        }
        else {
            return true;
        }
    }
    return false;
}

void Menu::_showConfirm(String msg)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
    lcd.setCursor(0, 1);
    lcd.write(arrowLeft);
    lcd.setCursor(15, 1);
    lcd.write(arrowRight);
    lcd.setCursor(1, 1);
    lcd.print(F("PARAR   SEGUIR"));
}

void Menu::_showStatus(String msg)
{
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(msg);
    lcd.setCursor(0, 1);
    lcd.write(arrowLeft);
    lcd.print(F("     PARAR    "));
    lcd.write(arrowRight);
}

void Menu::startBrew()
{
    /*
     * Rutina para comenzar una conccion con los parametros preseteados en brew
     */

    DEBUG_PRINT(freeMemory());
    DEBUG_PRINT(F("Comienza el programa de coccion!!!"));
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(F("   Brew time !!!"));

    DEBUG_PRINT(freeMemory());

    delay(1500);

    // Todos los procesos son en cascada, y si se sale del uno (return false)
    // el programa de cocción termina porque es una parada TOTAL.
    //
    // Prepara el agua y comienza a calentar
    if(Menu::prepareWater())
    {
        // agua lista, comienza el mash
        if(Menu::loadMash())
        {
            // inicio de macerado
            if(Menu::mashIn())
            {
            
            }
        }
    }

    // STOP ALL!
    Menu::stopHotElement(_rele_r1_pwm_pin);
    Menu::stopHotElement(_rele_r2_pwm_pin);
    Menu::stopPump(_rele_pump_a_pwm_pin);
    Menu::stopPump(_rele_pump_b_pwm_pin);
}

bool Menu::prepareWater()
{
    /*
     * Proceso de carga y acondicionamento del agua para el mash
     */

    bool status = false;
    float currentTemp = 0.0;
    float *_currentTemp = &currentTemp;
    Menu::_showConfirm(F("Cargar AGUA"));
    while(!buttons.isBack())
    {
        if(buttons.isEnter())
        {
            delay(500); // espera para evitar propagar el trigger
            float temp = brewMashStep0Temp + brewMashTempOffset;
            bool _ckT;
            Menu::_showStatus(F("Agua a    ___._C"));
            while(!buttons.isBack())
            {
                _ckT = Menu::checkTemp(addrSensorHlt, temp, _currentTemp, brewRecirculationCont, _rele_r1_pwm_pin, _rele_pump_a_pwm_pin);
                delay(500); // espera para evitar propagar el trigger
                if(!_ckT)
                {
                    Menu::cursorFloat(currentTemp, 10, 0);
                } 
                else 
                {
                    Menu::stopHotElement(_rele_r1_pwm_pin);
                    Menu::stopPump(_rele_pump_a_pwm_pin);
                    status = true; // se termino el proceso de forma correcta
                    break; // termina el calentamiento
                }
            }
            break; // termina el proceso del agua
        }
        if(buttons.isBack()) break; // termina el proceso del agua
    }

    return status;
}

bool Menu::loadMash()
{
    /*
     * Procesos para cargar el macerador y comentar el macerado
     */

    bool status = false;
    Menu::_showConfirm(F("Cargar MACERADOR"));
    while(!buttons.isBack())
    {
        if(buttons.isEnter())
        {
            Menu::_showStatus(F("Purgando BOMBA"));
            delay(500); // espera para evitar propagar el trigger
            Menu::purgePump(_rele_pump_a_pwm_pin);
            Menu::startPump(_rele_pump_a_pwm_pin);
            while(!buttons.isBack())
            {
                Menu::_showStatus(F("MACERADOR listo?"));
                if(buttons.isEnter())
                {
                    Menu::stopPump(_rele_pump_a_pwm_pin);
                    status = true; // se termino el proceso de forma correcta
                    break;
                }
            }
            break; // termina la carga de agua
        }
        if(buttons.isBack()) break; // termina el proceso de carga
    }

    // si continua, pregunta por la precarga de agua para el lavado
    if(status)
    {
        _SPARGING_WATER_IN_PROGRESS = false;
        Menu::_showConfirm(F("Preparar LAVADO?"));
        while(!buttons.isBack())
        {
            if(buttons.isEnter())
            {
                _SPARGING_WATER_IN_PROGRESS = true;
                Menu::startHotElement(_rele_r1_pwm_pin);
                Menu::_showConfirm(F("   CALENTANDO   "));
                delay(2000);
                break;
            } 
        }
    }

    return status;
}

bool Menu::mashIn()
{

    bool _ckT;
    bool status = false;
    uint8_t _currentMashStep = 0;
    unsigned long _currentTime;
    float currentTemp = 0.0;
    float *_currentTemp = &currentTemp;
    Menu::_showConfirm(F("Cargar GRANOS"));
    delay(500); // espera para evitar propagar el trigger
    while(!buttons.isBack())
    {
        if(buttons.isEnter())
        {
            Menu::_showStatus(F("Mash en progreso"));

            while(_currentMashStep < MAX_BREW_MASH_ITEMS - 1)
            {
                switch(_currentMashStep)
                {
                    case 0:
                        _currentTime = millis();
                        while(brewMashStep0Time > (millis() - _currentTime)/1000)
                        {
                            _ckT = Menu::checkTemp(addrSensorMt, brewMashStep0Temp, _currentTemp, brewRecirculationCont, _rele_r1_pwm_pin, _rele_pump_a_pwm_pin);
                            delay(500); // espera para evitar propagar el trigger
                            if(!_ckT) 
                            {
                                _currentTime += 500;
                            } 
                            else
                            {
                                Menu::_showStatus(F("Mashing 1 ___._C"));
                                Menu::cursorFloat(currentTemp, 10, 0);
                            }
                        }
                        break;
                }

                _currentMashStep++;
            }
        }
    }

    return status;
}

bool Menu::recirculation()
{

}

bool Menu::sparging()
{

}

bool Menu::boiling()
{

}

bool Menu::hops()
{

}

bool Menu::whirlpool()
{

}
