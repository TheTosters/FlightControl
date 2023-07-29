#ifndef __ALT_METER_GUARD__
#define __ALT_METER_GUARD__

#include <Arduino.h>
#include <pico.h>
#include "DFRobot_BMP280.h"
#include "Wire.h"
#include "telemetry_storage.hpp"

/* Wiring:
BMP280 -- rPi
--------------
VDD ----> 3.3V
GND ----> GND
SDA ----> GPIO14, (PIN 19)
SCL ----> GPIO15, (PIN 20)
*/
class AltMeter {
    private:
        static constexpr float BASELINE_P0 = 1015.0f;
        static constexpr int GPIO_14 = 14;
        static constexpr int GPIO_15 = 15;

        DFRobot_BMP280_IIC* bmp;

    public:
        /// @brief in ms between reads. According to BMP280 speed is up to 157Hz
        static constexpr int NEEDED_DELAY = 1000 / 157;

        void begin() {
            bmp = new DFRobot_BMP280_IIC(&Wire1, DFRobot_BMP280_IIC::eSdoLow);
            bmp->reset();
            int status = bmp->begin();

            bmp->setConfigFilter(DFRobot_BMP280_IIC::eConfigFilter_off);        // set config filter
            bmp->setConfigTStandby(DFRobot_BMP280_IIC::eConfigTStandby_0_5);    // set standby time
            bmp->setCtrlMeasSamplingTemp(DFRobot_BMP280_IIC::eSampling_X8);     // set temperature over sampling
            bmp->setCtrlMeasSamplingPress(DFRobot_BMP280_IIC::eSampling_X8);    // set pressure over sampling
            bmp->setCtrlMeasMode(DFRobot_BMP280_IIC::eCtrlMeasModeNormal);     // set control measurement mode to make these settings effective

            Serial.print("BMP begin status=");
            Serial.println(status);
            if(status == 0) {
                Serial.println("BMP init success!");
            } else {
                Serial.println("BMP init failed!");   
            }
        }

        inline void getAltitude(uint32_t* pressure, float* altitude) {
            float temperature = bmp->getTemperature();
            // Serial.print("temp=");
            // Serial.println(temperature);
            *pressure = bmp->getPressure();
            *altitude = bmp->calAltitude(BASELINE_P0, *pressure);
        }

        inline void fillTelemetryEntry(TelemetryInfo* entry) {
            uint32_t pres = bmp->getPressure();
            entry->pressure = pres;
            entry->altitude = bmp->calAltitude(BASELINE_P0, pres);
        }
};

#endif