#ifndef _POWER_SUBSYSTEM_H_
#define _POWER_SUBSYSTEM_H_

typedef struct {
    unsigned short *batteryLevel;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    unsigned int *timeElapsed;
} PowerSubsystemData;

void powerSubsystem(void* powerSubsystemData);

#endif  /* _POWER_SUBSYSTEM_H_ */