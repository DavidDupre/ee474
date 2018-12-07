#ifndef _PIRATE_MANAGEMENT_SUBSYSTEM_H_
#define _PIRATE_MANAGEMENT_SUBSYSTEM_H_

#define PHASOR_FIRE_DISTANCE 30
#define PHOTON_FIRE_DISTANCE 5
#define PIRATE_PROXIMITY_INITIAL 65535

#include "schedule.h"

typedef struct {
    unsigned int* pirateProximity;
} PirateManagementSubsystemData;

extern TCB pirateManagementSubsystemTCB;

void pirateManagementInit();

void pirateManagementSubsystem(void* pirateManagementSubsystemData);

void fireWeapon(unsigned int pin);

#endif /* _PIRATE_MANAGEMENT_SUBSYSTEM_H_ */