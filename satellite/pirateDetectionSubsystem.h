#ifndef _PIRATE_DETECTION_SUBSYSTEM_H_
#define _PIRATE_DETECTION_SUBSYSTEM_H_

#define PIRATE_DETECTION_DISTANCE 100


#include "pirateManagementSubsystem.h"
#include "schedule.h"

typedef struct {
    bool* detected;
    unsigned int* pirateProximity; 
} PirateDetectionSubsystemData;

extern TCB pirateDetectionSubsystemTCB;

void pirateDetectionSubsystem(void* pirateDectionSubsystemData);

#endif /* _PIRATE_DETECTION_SUBSYSTEM_H_ */






