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