#include "Arduino.h"
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <LiquidCrystal.h>
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
        int LCD_BACKLIGHT
        ): 
    buttons(BUTTONS_PIN, 50),
    lcd(LCD_RS, LCD_ENABLE, LCD_D4, LCD_D5, LCD_D6, LCD_D7)
{
    // backlight

    _lcd_columns = LCD_COLUMNS;
    _lcd_rows = LCD_ROWS;
    pinMode(LCD_BACKLIGHT, OUTPUT);
    digitalWrite(LCD_BACKLIGHT, HIGH);

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

    cursorDot = 2;
    cursorDotByte[0] = B10000;
    cursorDotByte[1] = B11000; 
    cursorDotByte[2] = B11100; 
    cursorDotByte[3] = B11110; 
    cursorDotByte[4] = B11100; 
    cursorDotByte[5] = B11000; 
    cursorDotByte[6] = B10000;

    cursorSpace = 3;
    cursorSpaceByte[0] = B00000;
    cursorSpaceByte[1] = B00000;
    cursorSpaceByte[2] = B01100; 
    cursorSpaceByte[3] = B11110; 
    cursorSpaceByte[4] = B01100; 
    cursorSpaceByte[5] = B00000; 
    cursorSpaceByte[6] = B00000;
    
    //EEPROM_writeAnything(ADDR_BREW_MASH_STEP0_TEMP, 55.5); // Set default
    EEPROM_readAnything(ADDR_BREW_MASH_STEP0_TEMP, brewMashStep0Temp);
    
    EEPROM_readAnything(ADDR_BREW_MASH_STEP1_TEMP, brewMashStep1Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP2_TEMP, brewMashStep2Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP3_TEMP, brewMashStep3Temp);
    EEPROM_readAnything(ADDR_BREW_MASH_STEP4_TEMP, brewMashStep4Temp);

    //EEPROM_writeAnything(ADDR_BREW_MASH_STEP0_TIME, 25); // Set default
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
    EEPROM_readAnything(ADDR_BREW_BOIL_HOPS4_TIME, brewBoilHops4Time);

    EEPROM_readAnything(ADDR_BREW_WHIRLPOOL_TIME, brewWhirlpoolTime);
    EEPROM_readAnything(ADDR_BREW_WHIRLPOOL_DELAY, brewWhirlpoolDelay);
     
    EEPROM_readAnything(ADDR_BREW_COOLING_TIME, brewCoolingTime);

    EEPROM_readAnything(ADDR_BREW_MASH_TEMP_OFFSET, brewMashTempOffset);
    EEPROM_readAnything(ADDR_BREW_SPARGING_TEMP_OFFSET, brewSpargingTempOffset);

    pointer_cursor = 0;

    lcd.createChar(arrowUp, arrowUpByte);
    lcd.createChar(arrowDown, arrowDownByte);
    lcd.createChar(cursorDot, cursorDotByte);
    lcd.createChar(cursorSpace, cursorSpaceByte);

    // init LCD
    lcd.begin(LCD_COLUMNS, LCD_ROWS);
    lcd.clear();
    
    menuItems[MENU_MANUAL] = "Manual/Monitor";
    menuItems[MENU_BREW] = "Coccion";
    menuItems[MENU_FERMENTATION] = "Fermentador";
    menuItems[MENU_CONFIGURATION] = "Configuracion";

    menuItemsManual[MANUAL_STAGE_MONITOR][SUBSTAGE_INDEX] = "Monitor";
    menuItemsManual[MANUAL_STAGE_HOT][SUBSTAGE_INDEX] = "Calentar";
    menuItemsManual[MANUAL_STAGE_PUMP][SUBSTAGE_INDEX] = "Bombear";

    menuItemsBrew[BREW_STAGE_MASH][SUBSTAGE_INDEX] = "Macerado";
    menuItemsBrew[BREW_STAGE_MASH][1] = "Escalon inicial";
    menuItemsBrew[BREW_STAGE_MASH][2] = "Escalon 1";
    menuItemsBrew[BREW_STAGE_MASH][3] = "Escalon 2";
    menuItemsBrew[BREW_STAGE_MASH][4] = "Escalon 3";
    menuItemsBrew[BREW_STAGE_MASH][5] = "Escalon 4";

    menuItemsBrew[BREW_STAGE_RECIRCULATION][SUBSTAGE_INDEX] = "Recirculado";
    menuItemsBrew[BREW_STAGE_RECIRCULATION][1] = "Recirculado continuo";
    menuItemsBrew[BREW_STAGE_RECIRCULATION][2] = "Tiempo de recirculado";
    
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
}

int Menu::stageSize(int stage, int max_substage_index)
{
    /*
     * Helper para saber cuentos elementos "reales" hay en un substage
     */

    int a = 0;
    for(int i = 0; i < max_substage_index; i++)
    {
        if(strlen(menuItemsBrew[stage][i])>0) a++;
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
        
    /*
    while(!buttons.isSelect())
    {
        if(buttons.isUp() && subpointer > 1)
        {
            subpointer--;
            Menu::stageSelector(stage, subpointer);
        }

        if(buttons.isDown() && subpointer < max_items)
        {
            subpointer++;
            Menu::stageSelector(stage, subpointer, false);
        }
    }
    */
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
    
    //lcd.print("FC " + String(_fc));
    //Serial.println("HTL Temp " + String(_hlt) + "C");
    //Serial.println("MT Temp " + String(_mt) + "C");
    //Serial.println("FC Temp " + String(_fc) + "C");
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

        Serial.print("Configura el tiempo y temperatura del escalon "); 
        Serial.println(subpointer);

        if(buttons.isEnter())
        {
            // configura el tiempo y temperratura del escalon n

            Serial.print("EEPROM_writeAnything brewMashStep0Time"); Serial.println(String(brewMashStep0Time));
            

            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(menuItemsBrew[BREW_STAGE_MASH][subpointer]);
            lcd.setCursor(0, 1);
            lcd.print("   ");
            lcd.setCursor(3, 1);
            lcd.print(" min a ");
            lcd.setCursor(10, 1);
            lcd.print("    ");
            lcd.setCursor(14, 1);
            lcd.print("C");

            int __time;
            int *_time = &__time;
            stepSetInt(brewMashStep0Time, 5, _time, 0, 1);
            Serial.println("Setting save stepSetInt " + String(*_time));
            delay(1000);

            float __temp;
            float *_temp = &__temp;
            stepSetFloat(brewMashStep0Temp, 0.5, _temp, 10, 1);
            Serial.println("Setting save stepSetFloat " + String(*_temp));
            delay(1000);

            // guardar?
            while(true)
            {
                lcd.clear();
                lcd.setCursor(0, 0);
                lcd.print("    Guardar?    ");
                lcd.setCursor(0, 1);
                lcd.print("NO           SI");
                
                Serial.println("EEPROM_writeAnything brewMashStep0Time" + String(brewMashStep0Time));
                Serial.println("EEPROM_writeAnything brewMashStep0Temp" + String(brewMashStep0Temp));
                if(buttons.isEnter())
                {
                    // Guarda los datos
                    EEPROM_writeAnything(ADDR_BREW_MASH_STEP0_TIME, brewMashStep0Time);
                    EEPROM_writeAnything(ADDR_BREW_MASH_STEP0_TEMP, brewMashStep0Temp);
                    lcd.clear();
                    lcd.setCursor(0, 0);
                    lcd.print("  Guardando...  ");
                    delay(1000);

                    break;
                }

                if(buttons.isBack() || buttons.isSelect()) break;
            }
        }
    
        Menu::stageSelector(BREW_STAGE_MASH, subpointer);
    }
}

void Menu::stepSetFloat(float _default, float _step, float *buffer, int cur_x, int cur_y)
{
    /*
     * Utilidad para setear en buffer un valor Float incrementando o decrementando
     * segun el step.
     * Sale con "Select" y muestra en LCD el valor
     */

    float i = _default;
    while(!buttons.isBack() && !buttons.isSelect() && !buttons.isEnter())
    {
        if(buttons.isUp()) i += _step;
        if(buttons.isDown() && i >= _step) i -= _step;

        lcd.setCursor(cur_x, cur_y);
        if(i < 10)
        { 
            lcd.setCursor(cur_x, cur_y);
            lcd.print("  ");
            lcd.setCursor(cur_x+2, cur_y);
            lcd.print(String(i));
        }
        if(i >= 10 && i < 100)
        {
            lcd.setCursor(cur_x, cur_y);
            lcd.print(" " + String(i));
        }
        if(i >= 100)
        {
            lcd.setCursor(cur_x, cur_y);
            lcd.print(String(i));
        }
    
        Serial.println("stepSetFloat " + String(i));
    }

    *buffer = i;
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

        lcd.setCursor(cur_x, cur_y);
        if(_default < 10)
        { 
            lcd.setCursor(cur_x, cur_y);
            lcd.print("  ");
            lcd.setCursor(cur_x+2, cur_y);
            lcd.print(String(_default));
        }
        if(_default >= 10 && _default < 100)
        {
            lcd.setCursor(cur_x, cur_y);
            lcd.print(" " + String(_default));
        }
        if(_default >= 100)
        {
            lcd.setCursor(cur_x, cur_y);
            lcd.print(String(_default));
        }
        
        Serial.println("stepSetInt " + String(_default));
    }

    *buffer = _default;
}

void Menu::brewRecirculation()
{
    /*
     * Muestra el menu de configuracion del recirculado.
     */

    Menu::stageSelector(BREW_STAGE_RECIRCULATION, 1);
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
