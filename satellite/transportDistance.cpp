#include <stdint.h>
#include <Arduino.h>
#include "transportDistance.h"
#include "schedule.h"
#include "sharedVariables.h"
#include "vehicleComms.h"
#include "udools.h"

/******************************************************************************
 * name : transportDistance
 *
 * inputs: transportDistanceData (void*)
 * transportDistanceData holds the pointers to the following data:
 *
 * outputs: void
 *
 * description:
 *
 * transportDistance measures the frequency of an input signal to determine 
 * the distance of transport vehicles
 *
 *
 * psuedocode:
 *
 * Wait for duration longer than lowest frequency
 * get time difference between two low input signals
 * convert to frequency
 * frequency convert to distance
 * if distance is more than 10% different from last reading in buffer, store value
 * 
 *
 *  author: Philip White
*****************************************************************************/

void addToBuffer(float distance, volatile float *buffer);

// volatile float distanceBufferPtr[TRANSPORT_DISTANCE_BUFFER_LENGTH];

TransportDistanceData transportDistanceData = {
    distanceBufferPtr
};

TCB transportDistanceTCB;

const char* const taskName = "Transport Distance";

void transportDistanceInit() {
    pinMode(SENSOR_PIN, INPUT);
    tcbInit(
        &transportDistanceTCB,
        &transportDistanceData,
        transportDistance,
        taskName,
        3
    );
}

void transportDistance(void *transportDistanceData) {
    TransportDistanceData *data = (TransportDistanceData *) transportDistanceData;

    unsigned long startPulse = 0;
    unsigned long endPulse = 0;
    bool firstDown = false;
    bool lastPinState = digitalRead(SENSOR_PIN);
    bool pinState;
    unsigned long taskStartTime = micros();
    while (micros() - taskStartTime < LONGEST_PERIOD && endPulse == 0) {
        pinState = digitalRead(SENSOR_PIN);
        if (startPulse == 0 && !pinState && pinState != lastPinState) {
            startPulse = micros();
        } else if (lastPinState != pinState && startPulse > 0 && !pinState) {
            endPulse = micros();
        }
        lastPinState = pinState;      
    }

    if (endPulse > 0) {
        unsigned long frequency = 1000000/(endPulse - startPulse); // Change from micros to Hz
        float distance = MAX_FREQ - frequency + MIN_FREQ; // Reverse range so Norm works
        if (distance > MAX_FREQ)
            distance = MAX_FREQ;

        if (distance < MIN_FREQ)
            distance = MIN_FREQ;

        distance = norm <float>(distance, MIN_FREQ, MAX_FREQ, MIN_DISTANCE, MAX_DISTANCE);

        if (abs(data->distanceBufferPtr[0] - distance) > (data->distanceBufferPtr[0])*0.10) {
            addToBuffer(distance, distanceBufferPtr, TRANSPORT_DISTANCE_BUFFER_LENGTH);
        }
    }
}