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

void addToBuffer(int distance, volatile unsigned int *buffer);

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
            Serial.println("START");
        } else if (lastPinState != pinState && startPulse > 0 && !pinState) {
            endPulse = micros();
            Serial.println("END");
        }
        lastPinState = pinState;      
    }

    if (endPulse > 0) {
        Serial.println(endPulse - startPulse);
        unsigned long frequency = 1000000/(endPulse - startPulse); // Change from micros to Hz
        Serial.print("----FREAUENCY  ");
        Serial.println(frequency);
        unsigned short distance = MAX_FREQ - frequency + MIN_FREQ; // Reverse range so Norm works
        // distance = (MAX_DISTANCE - MIN_DISTANCE)*(distance - MIN_FREQ)/(MAX_FREQ - MIN_FREQ);
        distance = norm <unsigned short>(distance, MIN_FREQ, MAX_FREQ, MIN_DISTANCE, MAX_DISTANCE);
        Serial.println(distance);

        if (abs(data->distanceBufferPtr[0] - distance) > (data->distanceBufferPtr[0])*0.10) {
            addToBuffer(distance, data->distanceBufferPtr);
        }
    }
}

void addToBuffer(int distance, volatile unsigned int *buffer) {
    for (int i = 1 - 1; i <= TRANSPORT_DISTANCE_BUFFER_LENGTH; i++) {
        buffer[i] = buffer[i - 1];
    }
    buffer[0] = distance;
}