#include "pirateDetectionSubsystem.h"
#include "pirateManagementSubsystem.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>

TCB pirateManagementSubsystemTCB;

PirateManagementSubsystemData pirateManagementSubsystemData = {
    &pirateProximity
};

/******************************************************************************
 * name: pirateManagementSubsystem
 *
 * inputs:
 * 
 * pirateManagementSubsystemData (void*): must be of Type pirateManagementSubsystemData*.
 * pirateManagementSubsystemData holds pointers to the following variables:
 * 
 * pirateProximity: unsigned int representing the distance of the nearest pirate in meters
 * 
 * outputs: void
 *
 * description:
 *  pirateManagementsSubsystem handles the management of nearby pirates attempting to infiltrate
 *  the satellite.
 *
 * pseudocode:
 * 
 * init:
 *  detected = false
 *  configure tcb
 * 
 * pirate management:
 *  if pirate less than phasor distance
 *   fire phasor
 *  if pirate less than photon distance
 *   fire photon
 *
 * author: Nick Orlov
*****************************************************************************/

void pirateManagementInit() {
    detected = false;

    tcbInit(
        &pirateManagementSubsystemTCB,
        &pirateManagementSubsystemData,
        pirateManagementSubsystem,
        TASKID_PIRATE_MANAGEMENT,
        1
    );
}

void pirateManagementSubsystem(void* pirateManagementSubsystemData) {
    PirateManagementSubsystemData *data = (PirateManagementSubsystemData *) pirateManagementSubsystemData;
    if(*data->pirateProximity <= PHASOR_FIRE_DISTANCE) {
        fireWeapon(PHASOR_PIN);
        if(*data->pirateProximity <= PHOTON_FIRE_DISTANCE) {
            fireWeapon(PHOTON_PIN);
        }
    }
}

// Fires the weapon assosciated with the input pin
void fireWeapon(unsigned int pin) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
}
