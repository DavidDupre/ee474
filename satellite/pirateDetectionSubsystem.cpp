#include "pirateDetectionSubsystem.h"
#include "pirateManagementSubsystem.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>
#include "udools.h"

TCB pirateDetectionSubsystemTCB;

PirateDetectionSubsystemData pirateDetectionSubsystemData = {
    &detected,
    &pirateProximity
};

// Initialize the LED outputs for displaying the weapons firing 42,43
// Initialize the pirate distance to INTEGER_MAX
// Having a pirate detected to be false
void pirateDetectionInit() {
    pirateProximity = MAX_INTEGER;
    detected = false;

    tcbInit(
        &pirateDetectionSubsystemTCB,
        &pirateDetectionSubsystemData,
        pirateDetectionSubsystem,
        TASKID_PIRATE_DETECTION,
        1
    );

    pinMode(PHASOR_PIN, OUTPUT);
    pinMode(PHOTON_PIN, OUTPUT);
}

void pirateDetectionSubsystem(void* pirateDetectionSubsystemData) {
    PirateDetectionSubsystemData *data = (PirateDetectionSubsystemData *) pirateDetectionSubsystemData;
    unsigned int distance = analogRead(PIRATE_DISTANCE_PIN);
    Serial.println(distance);
    // distance = norm<unsigned int>(distance, 0, 1023, 0, 400);
    // Serial.println(distance);
    *data->pirateProximity = distance;
    // *data->pirateProximity = analogRead(PIRATE_DISTANCE_PIN);
    if(*data->pirateProximity <= 100) {
        *data->detected = true;
        taskQueueInsert(&pirateManagementSubsystemTCB);
    } else {
        *data->detected = false;
        taskQueueDeleteLater(&pirateManagementSubsystemTCB);
    }
}