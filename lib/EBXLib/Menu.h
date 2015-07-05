#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include <LiquidCrystal.h>
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buttons.h"

// Menu principal
#define SUBSTAGE_INDEX              0

#define MENU_MANUAL                 0
#define MENU_BREW                   1
#define MENU_FERMENTATION           2
#define MENU_CONFIGURATION          3
#define MAX_MENU_ITEMS              4 // define cuantos elementos tiene el menu

// Manual / Monitor
#define MANUAL_STAGE_MONITOR        0
#define MANUAL_STAGE_HOT            1
#define MANUAL_STAGE_PUMP           2
#define MAX_MANUAL_STAGE_ITEMS      3
#define MAX_MANUAL_SUBSTAGE_ITEMS   1

// Brew
#define BREW_STAGE_MASH             0
#define BREW_STAGE_RECIRCULATION    1
#define BREW_STAGE_SPARGING         2
#define BREW_STAGE_BOIL             3
#define BREW_STAGE_WHIRLPOOL        4
#define BREW_STAGE_COOLING          5
#define MAX_BREW_STAGE_ITEMS        6 // define cuantos elementos tiene el stage
#define MAX_BREW_SUBSTAGE_ITEMS     6 // define cuantos elementos tiene el substage

// Fermentation
// implementar!

// HEXA MEMORY ADDRESS
// desde 0x000 hasta 0x200
#define ADDR_SENSOR_HTL                     0x00 // Direccion HEX para el sensor HLT
#define ADDR_SENSOR_MT                      0x08 // Direccion HEX para el sensor MT
#define ADDR_SENSOR_FC                      0x10 // Direccion HEX para el sensor FC

// Reserva de bytes para programa de coccion
#define ADDR_BREW_MASH_STEP0_TEMP           0x18 // Indica el tiempo del 1er escalon de macerado
#define ADDR_BREW_MASH_STEP1_TEMP           0x1E // 2do
#define ADDR_BREW_MASH_STEP2_TEMP           0x24 // 3er
#define ADDR_BREW_MASH_STEP3_TEMP           0x2A // 4to
#define ADDR_BREW_MASH_STEP4_TEMP           0x30 // 5to

#define ADDR_BREW_MASH_STEP0_TIME           0x36 // Indica el tiempo del 1er escalon de macerado
#define ADDR_BREW_MASH_STEP1_TIME           0x38 // 2do
#define ADDR_BREW_MASH_STEP2_TIME           0x3A // 3er
#define ADDR_BREW_MASH_STEP3_TIME           0x3C // 4to
#define ADDR_BREW_MASH_STEP4_TIME           0x3E // 5to
                                            
#define ADDR_BREW_RECIRCULATION_CONT        0x40 // Booleano, indica si es recirculado continuo, si es entonces al momento del macerado comienza a recircular
#define ADDR_BREW_RECIRCULATION_TIME        0x41 // Tiempo total de recirculado, incluso si es continuo
//0x43

class Menu
{
    private:
        int _lcd_columns;
        int _lcd_rows;
        bool _blink;
        OneWire oneWire;                    // SENSOR_ONEWIRE_PIN
        DallasTemperature sensorsBus;       // &oneWire
    
        DeviceAddress addrSensorHlt;
        DeviceAddress addrSensorMt;
        DeviceAddress addrSensorFc;
            
        int _rele_r1_pwm_pin;
        int _rele_r2_pwm_pin;
        int _rele_ht_pwm_pin;
        int _rele_lt_pwm_pin;
        int _rele_pump_pwm_pin;

        float __temp;
        int __time;
        bool __bool;

    public:

        int arrowUp;
        int arrowDown;
        int arrowLeft;
        int arrowRight;
        int cursorDot;
        int cursorSpace;
        int pointer_cursor;

        uint8_t arrowUpByte[7];
        uint8_t arrowDownByte[8];
        uint8_t arrowLeftByte[8];
        uint8_t arrowRightByte[8];
        uint8_t cursorDotByte[8];
        uint8_t cursorSpaceByte[8];

        float brewMashStep0Temp;            // 67 C
        float brewMashStep1Temp;            // 67 C
        float brewMashStep2Temp;            // 67 C
        float brewMashStep3Temp;            // 67 C
        float brewMashStep4Temp;            // 67 C

        int brewMashStep0Time;              // 90 min
        int brewMashStep1Time;              // 0 min
        int brewMashStep2Time;              // 0 min
        int brewMashStep3Time;              // 0 min
        int brewMashStep4Time;              // 0 min

        bool brewRecirculationCont;         // false
        int brewRecirculationTime;          // 30 min

        float brewSpargingTemp;             // 72 C
        int brewSpargingTime;               // 30 min

        int brewBoilTime;                   // 90 min
        int brewBoilHops0Time;              // 30 min (amargor)
        int brewBoilHops1Time;              // 45 min (amargor)
        int brewBoilHops2Time;              // 70 min (amargor/aroma)
        int brewBoilHops3Time;              // 80 min (aroma)
        int brewBoilHops4Time;              // 90 min (aroma)

        int brewWhirlpoolTime;              // 5 min
        int brewWhirlpoolDelay;             // 10 min

        int brewCoolingTime;                // 40 min
        
        float brewMashTempOffset;           // 5 C
        float brewSpargingTempOffset;       // 2 C

        char* menuItems[MAX_MENU_ITEMS];
        char* menuItemsManual[MAX_MANUAL_STAGE_ITEMS][MAX_MANUAL_SUBSTAGE_ITEMS];
        char* menuItemsBrew[MAX_BREW_STAGE_ITEMS][MAX_BREW_SUBSTAGE_ITEMS];
        
        Menu(
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
                int rele_r1_pwm_pin,
                int rele_r2_pwm_pin,
                int rele_ht_pwm_pin,
                int rele_lt_pwm_pin,
                int rele_pump_pwm_pin
                );

        Buttons buttons;
        LiquidCrystal lcd;
        int stageSize(int stage, int max_substage_index);
        void stageSelector(int stage, int subpointer = 1, bool asc = true);
        void stepSetInt(int _default, int _step, int *buffer, int cur_x, int cur_y);
        void stepSetFloat(float _default, float _step, float *buffer, int cur_x, int cur_y);
        void stepSetBool(bool _default, bool *buffer, int cur_x, int cur_y);
        void cursorFloat(float _f, int cur_x, int cur_y);
        void cursorInt(int _i, int cur_x, int cur_y);
        void cursorBool(bool _i, int cur_x, int cur_y);

        void _showSave();
        void _showSaved();

        void menuNext();
        void menuPrev();

        void showMenu();

        void home();
        void monitor(float htl, float mt, float fc, int _flags);
        int manual(int upPin, int downPin); //, int enterPin, int backPin);
        void brew(int pointer = 0);
        void brewMash();
        void brewRecirculation();
        void brewSparging();
        void brewBoil();
        void brewWhirlpool();
        void brewCooling();

        void configure_brewMashStep0();
        void configure_brewMashStep1();
        void configure_brewMashStep2();
        void configure_brewMashStep3();
        void configure_brewMashStep4();

};
#endif
