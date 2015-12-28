#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include <LiquidCrystal.h>
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include "Buttons.h"

// libs
#include <avr/pgmspace.h>
#include <MemoryFree.h>
#include <pgmStrToRAM.h>

// debug
#define DEBUG
#include "DebugUtils.h"

// Menu principal
#define SUBSTAGE_INDEX              0

#define MENU_MANUAL                 0
#define MENU_BREW                   1
#define MENU_FERMENTATION           2
#define MENU_CONFIGURATION          3
#define MENU_START                  4
#define MAX_MENU_ITEMS              5 // define cuantos elementos tiene el menu

// Brew
#define BREW_STAGE_MASH             0
#define BREW_STAGE_RECIRCULATION    1
#define BREW_STAGE_SPARGING         2
#define BREW_STAGE_BOIL             3
#define BREW_STAGE_WHIRLPOOL        4
#define BREW_STAGE_OPTIONS_OFFSET   5

#define MAX_BREW_STAGE_ITEMS        6 // define cuantos elementos tiene el stage
#define MAX_BREW_MASH_ITEMS         6 // define cuantos elementos tiene el substage

// Fermentation
// implementar!

// HEXA MEMORY ADDRESS
// desde 0x000 hasta 0x200
// int = 2
// float = 6
// bool = 1
// address = 8
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

#define ADDR_BREW_SPARGING_TEMP             0x43 // Temperatura del agua de lavado
#define ADDR_BREW_SPARGING_TIME             0x49 // Tiempo total del lavado

#define ADDR_BREW_BOIL_TIME                 0x4B // Indica cuanto tiempo ha de hervir (desde el inicio de hervor)
#define ADDR_BREW_BOIL_HOPS0_TIME           0x4D // Indica el tiempo de la 1er adesion de lupulo (desde el inicio de hervor)
#define ADDR_BREW_BOIL_HOPS1_TIME           0x4F // 2da
#define ADDR_BREW_BOIL_HOPS2_TIME           0x51 // 3ra
#define ADDR_BREW_BOIL_HOPS3_TIME           0x53 // 4ta

// DESACTIVADO POR SOBREDIMENSION DEL ARRAY
//#define ADDR_BREW_BOIL_HOPS4_TIME           0x55 // 5ta

// Ajusta la direccion a 0x55
#define ADDR_BREW_WHIRLPOOL_TIME            0x57 // Tiempo de whirpool, 5, 10, 15 min.
#define ADDR_BREW_WHIRLPOOL_DELAY           0x59 // Tiempo de epsera para completar el whirpool, 5, 10, 15 min.

#define ADDR_BREW_COOLING_TIME              0x5B // Tiempo encendida de la bomba de enfriado

// Offset y correciones y otros
#define ADDR_BREW_MASH_TEMP_OFFSET          0x5D // Offset para corregir la perdida de temperatura al momento de macerar +5º
#define ADDR_BREW_MASHIN_TEMP_OFFSET        0x63 // Offset para corregir la perdida de temperatura al momento de macerar +5º
#define ADDR_BREW_SPARGING_TEMP_OFFSET      0x69 // Offset para corregir la perdida de temperatura al momento de lavar +1º
// 0x6F


class Menu
{
    private:
        uint8_t _lcd_columns;
        uint8_t _lcd_rows;
        bool _blink;
        OneWire oneWire;                    // SENSOR_ONEWIRE_PIN
        DallasTemperature sensorsBus;       // &oneWire
    
        DeviceAddress addrSensorHlt;
        DeviceAddress addrSensorMt;
        DeviceAddress addrSensorFc;
            
        uint8_t _rele_r1_pwm_pin;
        uint8_t _rele_r2_pwm_pin;
        uint8_t _rele_ht_pwm_pin;
        uint8_t _rele_lt_pwm_pin;
        uint8_t _rele_pump_a_pwm_pin;
        uint8_t _rele_pump_b_pwm_pin;
        bool _RELE_NA;

        bool _HOT_ELEMENTS_STATUS;
        bool _PUMPS_STATUS;
        bool _SPARGING_WATER_IN_PROGRESS;

        //float __temp;
        //uint8_t __time;
        //uint8_t __delay;
        //bool __bool;

    public:

        uint8_t arrowUp;
        uint8_t arrowDown;
        uint8_t arrowLeft;
        uint8_t arrowRight;
        uint8_t cursorDot;
        uint8_t cursorSpace;
        uint8_t hotElement;
        uint8_t pumpOn;
        uint8_t pointer_cursor;

        uint8_t arrowUpByte[7];
        uint8_t arrowDownByte[8];
        uint8_t arrowLeftByte[8];
        uint8_t arrowRightByte[8];
        uint8_t cursorDotByte[8];
        uint8_t cursorSpaceByte[8];
        uint8_t hotElementByte[8];
        uint8_t pumpOnByte[8];

        float brewMashStep0Temp;            // 67 C
        float brewMashStep1Temp;            // 67 C
        float brewMashStep2Temp;            // 67 C
        float brewMashStep3Temp;            // 67 C
        float brewMashStep4Temp;            // 67 C

        uint8_t brewMashStep0Time;              // 90 min
        uint8_t brewMashStep1Time;              // 0 min
        uint8_t brewMashStep2Time;              // 0 min
        uint8_t brewMashStep3Time;              // 0 min
        uint8_t brewMashStep4Time;              // 0 min

        bool brewRecirculationCont;         // false
        uint8_t brewRecirculationTime;          // 30 min

        float brewSpargingTemp;             // 72 C
        uint8_t brewSpargingTime;               // 30 min

        uint8_t brewBoilTime;                   // 90 min
        uint8_t brewBoilHops0Time;              // 30 min (amargor)
        uint8_t brewBoilHops1Time;              // 45 min (amargor)
        uint8_t brewBoilHops2Time;              // 70 min (amargor/aroma)
        uint8_t brewBoilHops3Time;              // 80 min (aroma)
        uint8_t brewBoilHops4Time;              // 90 min (aroma)

        uint8_t brewWhirlpoolTime;              // 5 min
        uint8_t brewWhirlpoolDelay;             // 10 min

        uint8_t brewCoolingTime;                // 40 min
        
        float brewMashTempOffset;           // 3 C
        float brewMashInTempOffset;         // 1 C
        float brewSpargingTempOffset;       // 2 C

        const char* menuItems[MAX_MENU_ITEMS];    // 5x1 items
        const char* menuItemsBrew[MAX_BREW_STAGE_ITEMS][MAX_BREW_STAGE_ITEMS]; // 6x6 items
        
        Menu(
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
                uint8_t rele_r1_pwm_pin,
                uint8_t rele_r2_pwm_pin,
                uint8_t rele_ht_pwm_pin,
                uint8_t rele_lt_pwm_pin,
                uint8_t rele_pump_a_pwm_pin,
                uint8_t rele_pump_b_pwm_pin,
                bool rele_nc
                );

        Buttons buttons;
        LiquidCrystal lcd;

        ///////////////////////////////////////////////////////////////////////////////
        // MENU HELPERS
        ///////////////////////////////////////////////////////////////////////////////
        void stageSelector(uint8_t stage, uint8_t subpointer = 1, bool asc = true);
        void menuNext();
        void menuPrev();
        void _showSave();
        void _showSaved();
        void cursorFloat(float _o, float _f, uint8_t cur_x, uint8_t cur_y);
        void stepSetFloat(float _default, float _step, float *buffer, uint8_t cur_x, uint8_t cur_y);
        void cursorInt(uint8_t _i, uint8_t cur_x, uint8_t cur_y);
        void stepSetInt(uint8_t _default, uint8_t _step, uint8_t *buffer, uint8_t cur_x, uint8_t cur_y);
        void cursorBool(bool _i, uint8_t cur_x, uint8_t cur_y);
        void stepSetBool(bool _default, bool *buffer, uint8_t cur_x, uint8_t cur_y);
        void setTempTime(float &temp, uint8_t &time, int addrTemp, int addrTime);
        bool _showConfirm(String msg);
        void _showStatus(String msg, bool status = false);

        ///////////////////////////////////////////////////////////////////////////////
        // FUNCTIONS
        ///////////////////////////////////////////////////////////////////////////////
        void loadData();
        void statusHotPump();
        void startPump(uint8_t _pump);
        void stopPump(uint8_t _pump);
        void startHotElement(uint8_t _he);
        void stopHotElement(uint8_t _he);
        void purgePump(uint8_t _pump);

        // ver de cambiar a PID
        bool checkTemp(DeviceAddress addrSensor, float _temp, float *_currentTemp, 
                bool recirculationCont, uint8_t _hotElement, uint8_t _pump);

        ///////////////////////////////////////////////////////////////////////////////
        // OPTIONS MENU
        ///////////////////////////////////////////////////////////////////////////////
        void home();
        void showMenu();
        void brewMenu(uint8_t pointer = 0);
        void _showBrewMenu(uint8_t pointer = 0);
        void configureMash(uint8_t pointer = 1);
        void _showBrewMash(uint8_t pointer = 1, bool asc = true);

        void configureRecirculation();
        void configureSparging();
        void configureBoil();
        void configureWhirlpool();
        //void configureCooling();

        void configure_brewBoilTimes(uint8_t &boilTime, int addrTime, int step = 1);
        void configureOffset();
        void configure_tempOffset(float &tempOffset, int addrOffset);
        bool _processMashStep(float stepTemp, uint8_t stepTime, bool forcePump = true);

        ///////////////////////////////////////////////////////////////////////////////
        // RUNTIME
        ///////////////////////////////////////////////////////////////////////////////
        void monitor(float htl, float mt, float fc, uint8_t _flags);
        uint8_t manual(uint8_t upPin, uint8_t downPin);
        void startBrew();
        void stop();
        bool prepareWater();
        bool loadMash();
        bool mashIn();
        bool recirculation();
        bool sparging();
        bool boiling();
        bool hops();
        bool whirlpool();
};
#endif
