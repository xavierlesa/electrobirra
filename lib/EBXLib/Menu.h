#ifndef Menu_h
#define Menu_h

#include "Arduino.h"
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <LiquidCrystal.h>
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


// Reserva de bytes para programa de coccion
#define ADDR_BREW_MASH_STEP0_TEMP           16 // Indica el tiempo del 1er escalon de macerado
#define ADDR_BREW_MASH_STEP1_TEMP           20 // 2do
#define ADDR_BREW_MASH_STEP2_TEMP           24 // 3er
#define ADDR_BREW_MASH_STEP3_TEMP           28 // 4to
#define ADDR_BREW_MASH_STEP4_TEMP           32 // 5to

#define ADDR_BREW_MASH_STEP0_TIME           36 // Indica el tiempo del 1er escalon de macerado
#define ADDR_BREW_MASH_STEP1_TIME           40 // 2do
#define ADDR_BREW_MASH_STEP2_TIME           44 // 3er
#define ADDR_BREW_MASH_STEP3_TIME           48 // 4to
#define ADDR_BREW_MASH_STEP4_TIME           52 // 5to

#define ADDR_BREW_RECIRCULATION_CONT        54 // Booleano, indica si es recirculado continuo, si es entonces al momento del macerado comienza a recircular
#define ADDR_BREW_RECIRCULATION_TIME        56 // Tiempo total de recirculado, incluso si es continuo

#define ADDR_BREW_SPARGING_TEMP             60 // Temperatura del agua de lavado
#define ADDR_BREW_SPARGING_TIME             64 // Tiempo total del lavado

#define ADDR_BREW_BOIL_TIME                 68 // Indica cuanto tiempo ha de hervir (desde el inicio de hervor)
#define ADDR_BREW_BOIL_HOPS0_TIME           72 // Indica el tiempo de la 1er adesion de lupulo (desde el inicio de hervor)
#define ADDR_BREW_BOIL_HOPS1_TIME           76 // 2da
#define ADDR_BREW_BOIL_HOPS2_TIME           80 // 3ra
#define ADDR_BREW_BOIL_HOPS3_TIME           84 // 4ta
#define ADDR_BREW_BOIL_HOPS4_TIME           88 // 5ta

#define ADDR_BREW_WHIRLPOOL_TIME            92 // Tiempo de whirpool, 5, 10, 15 min.
#define ADDR_BREW_WHIRLPOOL_DELAY           96 // Tiempo de epsera para completar el whirpool, 5, 10, 15 min.

#define ADDR_BREW_COOLING_TIME              100 // Tiempo encendida de la bomba de enfriado

// Offset y correciones y otros
#define ADDR_BREW_MASH_TEMP_OFFSET          104 // Offset para corregir la perdida de temperatura al momento de macerar +5º
#define ADDR_BREW_SPARGING_TEMP_OFFSET      108 // Offset para corregir la perdida de temperatura al momento de lavar +1º


class Menu
{
    private:
        int _lcd_columns;
        int _lcd_rows;
        bool _blink;
    
    public:

        int arrowUp;
        int arrowDown;
        int cursorDot;
        int cursorSpace;
        int pointer_cursor;

        uint8_t arrowUpByte[7];
        uint8_t arrowDownByte[8];
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
                int LCD_BACKLIGHT
                );

        Buttons buttons;
        LiquidCrystal lcd;
        int stageSize(int stage, int max_substage_index);
        void stageSelector(int stage, int subpointer = 1, bool asc = true);
        void stepSetInt(int _default, int _step, int *buffer, int cur_x, int cur_y);
        void stepSetFloat(float _default, float _step, float *buffer, int cur_x, int cur_y);

        void menuNext();
        void menuPrev();

        //void showMenu();

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
};
#endif
