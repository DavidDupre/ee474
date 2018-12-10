#include "pirateDetectionSubsystem.h"
#include "pirateManagementSubsystem.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>
#include "comsReceive.h"

#define CMDID_FIRE_PHOTON 28
#define CMDID_FIRE_PHASER 29

static bool firePhoton;
static bool firePhasor;

TCB pirateManagementSubsystemTCB;

PirateManagementSubsystemData pirateManagementSubsystemData = {
    &pirateProximity
};

void handleFirePhoton(uint8_t *data);
void handleFirePhasor(uint8_t *data);

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

    comsRxRegisterCallback(CMDID_FIRE_PHOTON, handleFirePhoton);
    comsRxRegisterCallback(CMDID_FIRE_PHASER, handleFirePhasor);
    firePhasor = false;
    firePhoton = false;
}

void pirateManagementSubsystem(void* pirateManagementSubsystemData) {
    PirateManagementSubsystemData *data = (PirateManagementSubsystemData *) pirateManagementSubsystemData;
    if(*data->pirateProximity <= PHASOR_FIRE_DISTANCE || firePhasor) {
        fireWeapon(PHASOR_PIN);
        firePhasor = false;
    }
    if(*data->pirateProximity <= PHOTON_FIRE_DISTANCE || firePhoton) {
        fireWeapon(PHOTON_PIN);
        firePhoton = false;
    }
}

// Fires the weapon assosciated with the input pin
void fireWeapon(unsigned int pin) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
}

void handleFirePhasor(uint8_t *data) {
    firePhasor = true;
    return true;
}

void handleFirePhoton(uint8_t *data) {
    firePhoton = true;
    return true;
}