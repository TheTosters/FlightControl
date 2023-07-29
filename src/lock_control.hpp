#ifndef __LOCK_CTRL_GUARD__
#define __LOCK_CTRL_GUARD__

#include <Arduino.h>
#include <pico.h>
#include "RP2040_ISR_Servo.h"

/**
 * Wiring:
 * Yellow -> GPIO_28 (PIN 34)
 * Red -> VBUS / 5V
 * Brown -> GND
*/
class LockControl {
    private:
        static constexpr int MIN_MICROS = 800;
        static constexpr int MAX_MICROS = 2450;
        static constexpr int GPIO_28 = 28;

        int servoIndex;

    public:
        LockControl() {
            pinMode(GPIO_28, OUTPUT);
            digitalWrite(GPIO_28, LOW);
            servoIndex = RP2040_ISR_Servos.setupServo(GPIO_28, MIN_MICROS, MAX_MICROS);
        }

        /// @brief Put lock of doors in LOCKED position
        void lock() {
            RP2040_ISR_Servos.setPosition(servoIndex, 0);
        }

        /// @brief Put lock of doors in UNLOCKED position
        void unlock() {
            RP2040_ISR_Servos.setPosition(servoIndex, 180);
        }
};

#endif