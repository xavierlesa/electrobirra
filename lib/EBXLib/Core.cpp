#include "Arduino.h"
#include <EEPROM.h> 
#include <EEPROMAnything.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Menu.h>
#include <Core.h>

Core::Core(
        Menu* menu,
        int SENSOR_ONEWIRE_PIN,
        int rele_r1_pwm_pin,
        int rele_r2_pwm_pin,
        int rele_ht_pwm_pin,
        int rele_lt_pwm_pin,
        int rele_pump_pwm_pin
        ):
    oneWire(SENSOR_ONEWIRE_PIN),
    sensorsBus(&oneWire)
{

    RELE_R1_PWM_PIN = rele_r1_pwm_pin;
    RELE_R2_PWM_PIN = rele_r2_pwm_pin;
    RELE_HT_PWM_PIN = rele_ht_pwm_pin;
    RELE_LT_PWM_PIN = rele_lt_pwm_pin;
    RELE_PUMP_PWM_PIN = rele_pump_pwm_pin;

    _menu = menu;
    DeviceAddress addrSensorTest = { 0x28, 0xFF, 0xBA, 0x51, 0x4B, 0x04, 0x00, 0x26 };
    EEPROM_writeAnything(ADDR_SENSOR_HTL, addrSensorTest);

     // Carga los datos de memoria
    EEPROM_readAnything(ADDR_FERMENTATION_TEMP_HIGH, fermentationTempHigh);
    EEPROM_readAnything(ADDR_FERMENTATION_TEMP_LOW, fermentationTempLow);
    EEPROM_readAnything(ADDR_FERMENTATION_TEMP_HIGH_OFFSET, fermentationTempHighOffset);
    EEPROM_readAnything(ADDR_FERMENTATION_TEMP_LOW_OFFSET, fermentationTempLowOffset);

    EEPROM_readAnything(ADDR_SENSOR_HTL, addrSensorHtl);
    EEPROM_readAnything(ADDR_SENSOR_MT, addrSensorMt);
    EEPROM_readAnything(ADDR_SENSOR_FC, addrSensorFc);

    // inicia la librería de sensores
    sensorsBus.begin();
}
    
bool array_cmp(uint8_t *a, uint8_t *b, int len_a, int len_b){
     int n;

     // if their lengths are different, return false
     if (len_a != len_b) return false;

     // test each element to be the same. if not, return false
     for (n=0;n<len_a;n++) if (a[n]!=b[n]) return false;

     //ok, if we have not returned yet, they are equal :)
     return true;
}

String _sensorsPrintAddress(DeviceAddress deviceAddress)
{
    /*
     * Obtiene la dirección ADDR HEX del sensor
     */

    String address = "";

    for (uint8_t i = 0; i < 8; i++)
    {
        if (deviceAddress[i] < 16) 
        {
            address += "0" + String(deviceAddress[i], HEX);
        }
        else address += String(deviceAddress[i], HEX);
    }

    Serial.println(address);
    return address;
}

void Core::discoverSensors()
{
    /*
     * Utilidad para obtener las direcciones de los sensores conectados
     * La idea es poder visualizar los sensores conectados para luego
     * poder configurarlos desde el panel.
     */

    DeviceAddress devicesConnected;

    int totalDevices = sensorsBus.getDeviceCount();

    Serial.println("Busca los sensores (DS18*) conectados y devuelve las direcciones + indice");
    Serial.print("Buscando dispositivos...");
    Serial.print("Se encontraron ");
    Serial.print(totalDevices, DEC);
    Serial.println(" dispositivos.");

    _menu->lcd.clear();
    _menu->lcd.print("Buscando");
    _menu->lcd.setCursor(0, 1);
    _menu->lcd.print("dispositivos ...");

    // Invoca a la lectura de los sensores
    //sensorsBus.requestTemperatures();
    delay(2000);

    for(int i = 0; i < totalDevices; i++)
    {
        if(sensorsBus.getAddress(devicesConnected, i)) 
        {
            Serial.print("Sensor: "); Serial.print(i); Serial.print(" ADDR: ");
            _sensorsPrintAddress(devicesConnected);
            Serial.print(" Temperatura de lectura: ");

            if(sensorsBus.requestTemperaturesByAddress(devicesConnected))
            {
                Serial.println(String(sensorsBus.getTempC(devicesConnected)) + " C");
            }
        }
    }

    Serial.println("");
    Serial.println("Fin discoverSensors");

    while(!_menu->buttons.isSelect())
    {
        _menu->lcd.clear();
        _menu->lcd.print("Dispositivos");
        _menu->lcd.setCursor(0, 1);
        _menu->lcd.print("encontrados " + String(totalDevices));
        delay(700);
        _menu->lcd.clear();
        delay(700);

        // entra al modo "listar"
        if(_menu->buttons.isDown())
        {
            // muestra las direcciones mas indice
            // Sensor OK/?? NN
            // 28FFBA514B040026/Enter configurar
            int _pos = 0;
            bool _blink = true, is_setted;
            sensorsBus.getAddress(devicesConnected, _pos);
            is_setted = array_cmp(devicesConnected, addrSensorHtl, 8, 8);
            String address = _sensorsPrintAddress(devicesConnected);

            _menu->lcd.clear();
            _menu->lcd.print("Sensor " + String(_pos));

            if(is_setted) _menu->lcd.print(" OK");
            else _menu->lcd.print(" ??");

            _menu->lcd.setCursor(0, 1);
            _menu->lcd.print(address);

            delay(700); // espera para no continuar bajando por los sensores descubiertos

            while(!_menu->buttons.isBack())
            {
                if(_menu->buttons.isDown() && _pos < totalDevices - 1)
                {
                    _pos++;
                    sensorsBus.getAddress(devicesConnected, _pos);
                    address = _sensorsPrintAddress(devicesConnected);
                    is_setted = array_cmp(devicesConnected, addrSensorHtl, 8, 8);
                }

                if(_menu->buttons.isUp() && _pos > 0)
                {
                    _pos--;
                    sensorsBus.getAddress(devicesConnected, _pos);
                    address = _sensorsPrintAddress(devicesConnected);
                    is_setted = array_cmp(devicesConnected, addrSensorHtl, 8, 8);
                }

                _menu->lcd.clear();
                if(_blink)
                {
                    _menu->lcd.print("Sensor " + String(_pos));
                    if(is_setted) _menu->lcd.print(" OK");
                    else _menu->lcd.print(" ??");
                }
                else 
                {
                    _menu->lcd.print("Configurar ENTER");
                }

                _menu->lcd.setCursor(0, 1);
                _menu->lcd.print(address);

                delay(700);
                _blink = !_blink;

                // Espera al enter y entra al menu de configurar dispositivo
                // Asocia el dispositivo a una variable y guarda esa direccion
                // en la EEPROM para luego ser usada segun el dispositivo.
                // ADDR_SENSOR_HTL
                // ADDR_SENSOR_MT
                // ADDR_SENSOR_FC
            }
        
        }
    }

}

void Core::main()
{
    
    _menu->lcd.clear();
    _menu->lcd.print("  ElectroBirra");
    _menu->lcd.setCursor(0, 1);
    _menu->lcd.print("  v0.2 201506");

    delay(2500);
    
    _menu->home();

    while(true)
    {
        if(_menu->buttons.isUp()) 
        {
            _menu->menuPrev();
        }
        
        if(_menu->buttons.isDown())
        {
            _menu->menuNext();
        }

        // MANUAL/MONITOR
        if(_menu->buttons.isEnter() && _menu->pointer_cursor == MENU_MANUAL)
        {
            Serial.println("Entrando a modo monitor");
            float hlt = 0.0;
            float mt = 0.0;
            float fc = 0.0;

            while(!_menu->buttons.isSelect())
            {
                if(sensorsBus.requestTemperaturesByAddress(addrSensorHtl))
                    hlt = sensorsBus.getTempC(addrSensorHtl);

                if(sensorsBus.requestTemperaturesByAddress(addrSensorMt))
                    mt = sensorsBus.getTempC(addrSensorMt);

                if (sensorsBus.requestTemperaturesByAddress(addrSensorFc))
                    fc = sensorsBus.getTempC(addrSensorFc);
                
                _menu->monitor(hlt, mt, fc, _menu->manual(RELE_R1_PWM_PIN, RELE_PUMP_PWM_PIN));
            }
            _menu->home();
        }

        // BREW
        if(_menu->buttons.isEnter() && _menu->pointer_cursor == MENU_BREW)
        {
            Serial.println("Entrando a modo brew");
            _menu->brew();
            _menu->home();
        }

        // FERMENTATION
        if(_menu->buttons.isEnter() && _menu->pointer_cursor == MENU_FERMENTATION)
        {
            Serial.println("Entrando a modo fermentador");
            //Menu::fermentator();
        }

        // CONFIGURATION
        if(_menu->buttons.isEnter() && _menu->pointer_cursor == MENU_CONFIGURATION)
        {
            Serial.println("Entrando a modo configuracion");
            Core::discoverSensors();
        }

    }
}
