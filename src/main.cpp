#include <Arduino.h>
#include <pico.h>
#include <Wire.h>
#include "lock_control.hpp"
#include "alt_meter.hpp"
#include "gyro_and_acc.hpp"
#include "telemetry_storage.hpp"
#include "parachute_ctrl.hpp"
#include <math.h>

static constexpr int GPIO_14 = 14;
static constexpr int GPIO_15 = 15;

LockControl lockCtrl;
AltMeter altMeter;
GyroNAcc gyroNAcc;
TelemetryStorage storage;
ParachuteCtrl parachuteCtrl(&lockCtrl);
int loopDelay = 0;

void setup() {
  Wire1.setSDA(GPIO_14);
  Wire1.setSCL(GPIO_15);

  Serial.begin(115200);
  delay(2500);
  Serial.print("Starting");
  lockCtrl.lock();
  altMeter.begin();
  gyroNAcc.begin();
  loopDelay = max(altMeter.NEEDED_DELAY, gyroNAcc.NEEDED_DELAY);
}

/* Test code to tune parachute control*/
/*
float deg = 0;
TelemetryInfo tmp;
void loop() {
  delay(loopDelay);
  deg += 0.3f;
  tmp.altitude = 100 * sin(deg * DEG_TO_RAD) + random(-30, 30) / 10.0f;
  Serial.print("Alt=");
  Serial.println(tmp.altitude);
  parachuteCtrl.update(&tmp);
}
*/

TelemetryInfo telemetryEntry;
void loop() {
  delay(loopDelay);
  altMeter.fillTelemetryEntry(&telemetryEntry);
  gyroNAcc.fillTelemetryEntry(&telemetryEntry);
  parachuteCtrl.update(&telemetryEntry);
  storage.addToStorage(&telemetryEntry);
}