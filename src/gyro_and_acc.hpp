#ifndef __GYRO_N_ACC_GUARD__
#define __GYRO_N_ACC_GUARD__

#include <Arduino.h>
#include <pico.h>
#include <MPU6500_WE.h>
#include <Wire.h>
#include "telemetry_storage.hpp"

/* Wiring:
MPU9250 -- rPi
--------------
VDD ----> 3.3V
GND ----> GND
SDA ----> GPIO14, (PIN 19)
SCL ----> GPIO15, (PIN 20)
*/
class GyroNAcc {
    private:
        static constexpr float BASELINE_P0 = 1015.0f;
        static constexpr uint8_t ADDRESS = 0x68;

        MPU6500_WE mpu = MPU6500_WE(&Wire1, ADDRESS);
    public:
        /// @brief in ms between reads. According to doc speed is up to 260 Hz on low pass filter but
        /// not sure if I understands this correctly and configured correctly :)
        static constexpr int NEEDED_DELAY = 1000 / 260;

        typedef struct {
            float yaw;
            float pitch;
            float roll;
            float linearAccX;
            float linearAccY;
            float linearAccZ;
        } Data;
        
        void begin() {
            bool result = mpu.init();
            if (result) {
                Serial.println("MPU9250 init success");
            } else {
                Serial.println("MPU9250 init failed");
            }
            Serial.println("MPU9250 start calibration");
            mpu.autoOffsets();
            mpu.enableGyrDLPF();
            mpu.setGyrDLPF(MPU6500_DLPF_6);
            mpu.setSampleRateDivider(5);
            mpu.setGyrRange(MPU6500_GYRO_RANGE_250);
            mpu.setAccRange(MPU6500_ACC_RANGE_2G);
            mpu.enableAccDLPF(true);
            mpu.setAccDLPF(MPU6500_DLPF_6);

            delay(200);
        }

        void getData(Data& data) {
            xyzFloat gValue = mpu.getGValues();
            xyzFloat gyr = mpu.getGyrValues();
            float resultantG = mpu.getResultantG(gValue);
            data.yaw = gyr.x;
            data.pitch = gyr.y;
            data.roll = gyr.z;
            data.linearAccX = gValue.x;
            data.linearAccY = gValue.y;
            data.linearAccZ = gValue.z;
        }

        inline void fillTelemetryEntry(TelemetryInfo* entry) {
            xyzFloat gValue = mpu.getGValues();
            entry->linearAccX = gValue.x;
            entry->linearAccY = gValue.y;
            entry->linearAccZ = gValue.z;

            xyzFloat gyr = mpu.getGyrValues();
            entry->yaw = gyr.x;
            entry->pitch = gyr.y;
            entry->roll = gyr.z;

            entry->resultantG = mpu.getResultantG(gValue);
        }
};

#endif