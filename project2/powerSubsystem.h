#ifndef _POWER_SUBSYSTEM_H_
#define _POWER_SUBSYSTEM_H_

typedef struct {
    bool *solarPanelState;
    unsigned short *batteryLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
} PowerSubsystemData;

void powerSubsystem(void* powerSubsystemData);

#endif  /* _POWER_SUBSYSTEM_H_ */