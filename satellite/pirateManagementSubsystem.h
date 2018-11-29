#ifndef _PIRATE_MANAGEMENT_SUBSYSTEM_H_
#define _PIRATE_MANAGEMENT_SUBSYSTEM_H_

#define PHASOR_FIRE_DISTANCE 30
#define PHOTON_FIRE_DISTANCE 5

#include "schedule.h"

typedef struct {
    unsigned int* pirateProximity;
} PirateManagementSubsystemData;

extern TCB pirateManagementSubsystemData;

void pirateManagementSubsystem(void* pirateManagementSubsystemData);

void firePhasor();
void firePhoton();