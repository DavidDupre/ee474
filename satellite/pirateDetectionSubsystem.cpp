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

/******************************************************************************
 * name: pirateDetectionSubsystem
 *
 * inputs:
 *
 * pirateDetectionSubsystemData (void*): must be of Type pirateDetectionSubsystemData*.
 * pirateDetectionSubsystemData holds pointers to the following variables:
 *
 * pirateProximity: unsigned int representing the distance of the nearest pirate in meters
 * detected: bool representing if a pirate has been detected within 100 meters
 *
 * outputs: void
 *
 * description:
 *  pirateDetectionSubsystem handles the detection of nearby pirates attempting to infiltrate
 *  the satellite.
 *
 * pseudocode:
 *
 * init:
 *  pirateProximity = max_integer;
 *  detected = false
 *  configure tcb
 *  configure pins
 *
 * pirate detection:
 *  if pirate less than or equal to 100
 *   set detected to true
 *   schedule pirate management
 *  else
 *   set detected to false
 *   unschedule pirate management
 *
 * author: Nick Orlov
*****************************************************************************/

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
    // distance = norm<unsigned int>(distance, 0, 1023, 0, 400);
    // Serial.println(distance);
    *data->pirateProximity = distance;
    if(*data->pirateProximity <= 100) {
        *data->detected = true;
        taskQueueInsert(&pirateManagementSubsystemTCB);
    } else {
        *data->detected = false;
        taskQueueDeleteLater(&pirateManagementSubsystemTCB);
    }
}