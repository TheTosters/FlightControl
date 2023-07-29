#ifndef __TELEMETRY_GUARD__
#define __TELEMETRY_GUARD__

#include <Arduino.h>

typedef struct {
    float yaw;
    float pitch;
    float roll;
    float linearAccX;
    float linearAccY;
    float linearAccZ;
    float resultantG;
    float altitude;
    uint32_t pressure;
} __attribute__((__packed__)) TelemetryInfo;

class TelemetryStorage {
    private:
        static constexpr int recordsPerBank = 4096;
        TelemetryInfo data[recordsPerBank];
        int entityIndex;

    public:
        void addToStorage(TelemetryInfo* info) {
            if (entityIndex < recordsPerBank) {
                data[entityIndex] = *info;
                entityIndex++;
            }
        }

        void writeToFlash() {

        }

        void readFromFlash() {

        }

        /// @brief returns pointer to whole data buffer
        /// @param count returns info about how many entries in buffer is filled
        /// @return pointer to telemetry buffer
        TelemetryInfo* getBuffer(int* count) {
            *count = entityIndex;
            return &(data[0]);
        } 
};

#endif