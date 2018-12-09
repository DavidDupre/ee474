#ifndef _PIRATE_DETECTION_SUBSYSTEM_H_
#define _PIRATE_DETECTION_SUBSYSTEM_H_

#define PIRATE_DETECTION_DISTANCE 100
#define MAX_INTEGER 32767
#define PHASOR_PIN 42
#define PHOTON_PIN 41
#define PIRATE_DISTANCE_PIN 10


#include "pirateManagementSubsystem.h"
#include "schedule.h"

typedef struct {
    bool* detected;
    unsigned int* pirateProximity; 
} PirateDetectionSubsystemData;

void pirateDetectionInit();

extern TCB pirateDetectionSubsystemTCB;

void pirateDetectionSubsystem(void* pirateDectionSubsystemData);

#endif /* _PIRATE_DETECTION_SUBSYSTEM_H_ */






