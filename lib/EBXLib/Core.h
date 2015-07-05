#ifndef Core_h
#define Core_h

#include "Arduino.h"
//#include <EEPROM.h> 
//#include "EEPROMAnything.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Menu.h>

// Pines 0 y 4 a 9 reservados

// MEMORY http://www.arduino.cc/en/Reference/EEPROMUpdate
// Reserva de bytes en memoria para fermentador
#define ADDR_FERMENTATION_TEMP_HIGH         0  // Guarda la temp máxima que dispara el enfriado del fermantador
#define ADDR_FERMENTATION_TEMP_LOW          4  // Guarda la temp mínima que dispara el calentado del fermentador
#define ADDR_FERMENTATION_TEMP_HIGH_OFFSET  8  // Offset de temp máxima
#define ADDR_FERMENTATION_TEMP_LOW_OFFSET   12 // Offset de temp mínima

// Direcciones de 16 bytes
#define ADDR_SENSOR_HTL                     0x88 // Direccion HEX para el sensor HLT
#define ADDR_SENSOR_MT                      0x90 // Direccion HEX para el sensor MT
#define ADDR_SENSOR_FC                      0x98 // Direccion HEX para el sensor FC
// 160

class Core
{
    private:
        OneWire oneWire;                    // SENSOR_ONEWIRE_PIN
        DallasTemperature sensorsBus;       // &oneWire

        float fermentationTempHigh;         // 18.50 C
        float fermentationTempLow;          // 14.50 C
        float fermentationTempHighOffset;   // 0.50 C
        float fermentationTempLowOffset;    // 0.50 C 

        DeviceAddress addrSensorHlt;
        DeviceAddress addrSensorMt;
        DeviceAddress addrSensorFc;
        DeviceAddress addrSensorTest;//  = { 0x28, 0xFF, 0xBA, 0x51, 0x4B, 0x04, 0x00, 0x26 };
            
        int RELE_R1_PWM_PIN;
        int RELE_R2_PWM_PIN;
        int RELE_HT_PWM_PIN;
        int RELE_LT_PWM_PIN;
        int RELE_PUMP_PWM_PIN;

        bool array_cmp(uint8_t *a, uint8_t *b, int len_a, int len_b);

    public:
        Core(
                Menu* menu,
                int SENSOR_ONEWIRE_PIN,
                int RELE_R1_PWM_PIN,
                int RELE_R2_PWM_PIN,
                int RELE_HT_PWM_PIN,
                int RELE_LT_PWM_PIN,
                int RELE_PUMP_PWM_PIN
                );

        Menu* _menu;
        void main();
        float getTemperature(DeviceAddress device);
        void discoverSensors();
        String _sensorsPrintAddress(DeviceAddress deviceAddress);

};

#endif
