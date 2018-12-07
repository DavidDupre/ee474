#include "pirateDetectionSubsystem.h"
#include "pirateManagementSubsystem.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>

TCB pirateManagementSubsystemTCB;

PirateManagementSubsystemData pirateManagementSubsystemData = {
    &pirateProximity
};

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

void fireWeapon(unsigned int pin) {
    digitalWrite(pin, HIGH);
    delay(100);
    digitalWrite(pin, LOW);
    delay(100);
}
