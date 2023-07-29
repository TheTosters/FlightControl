#ifndef __PARACHUTE_CTRL_GUARD__
#define __PARACHUTE_CTRL_GUARD__

#include "lock_control.hpp"
#include "telemetry_storage.hpp"

class ParachuteCtrl {
    private:
        static constexpr int LIST_LEN = 4;
        static constexpr int SAMPLES_PER_AVG = 6;
        static constexpr float FALL_PRC = 0.01f;

        LockControl* lockCtrl;
        float avgList[LIST_LEN] = {0};
        float samples[SAMPLES_PER_AVG] = {0};
        int sampleIndex = 0;
        int avgListIndex = 0;
        bool parachuteIsOpen = false;
    public:
        ParachuteCtrl(LockControl* aLockCtrl) : lockCtrl(aLockCtrl) {};
        void update(TelemetryInfo* info) {
            if (!parachuteIsOpen) {
                if (collectSample(info)) {
                    //full sample collected, analyze what to do?
                    if (avgListIndex >= LIST_LEN) {
                        //Decide what to do, if not here not whole AvgList is filled
                        parachuteDecision();
                    }
                }
            }
        }

    private:
        void parachuteDecision() {
            int fallCount = 0;
            float data = avgList[LIST_LEN - 1];
            for(int t = LIST_LEN - 2; t >= 0; t--) {
                float tmp = data == 0 ? 1 : (avgList[t] - data) / data;
                if (tmp > FALL_PRC) {
                    fallCount++;
                }
                data = avgList[t];
            }
            Serial.print("Fall count:");
            Serial.println(fallCount);
            if (fallCount >= LIST_LEN - 1) {
                //Open Parachute
                lockCtrl->unlock();
                parachuteIsOpen = true;
                Serial.println("Open parachute");
            }
        }

        bool collectSample(TelemetryInfo* info) {
            samples[sampleIndex] = info->altitude;
            sampleIndex++;
            bool result = sampleIndex == SAMPLES_PER_AVG;
            if (result) {
                addNewAvg();
                sampleIndex = 0;
            }
            return result;
        }

        void addNewAvg() {
            float sum = 0;
            for(int t = 0; t < SAMPLES_PER_AVG; t++) {
                sum += samples[t];
            }
            sum /= SAMPLES_PER_AVG;
            if (avgListIndex >= LIST_LEN) {
                for(int t = 0; t < LIST_LEN - 1; t++) {
                    avgList[t] = avgList[t + 1];
                }
                avgList[LIST_LEN - 1] = sum;
            } else {
                avgList[avgListIndex] = sum;
                avgListIndex ++;
            }
            Serial.print("AvgList: ");
            for(int t = 0; t < LIST_LEN; t++) {
                Serial.print(avgList[t]);
                Serial.print(", ");
            }
            Serial.println();
        }
};

#endif