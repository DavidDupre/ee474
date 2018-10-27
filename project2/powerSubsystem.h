#ifndef _POWER_SUBSYSTEM_H_
#define _POWER_SUBSYSTEM_H_

#define HIGH_BATTERY 95
#define BATTERY_LEVEL_MID 50
#define BATTERY_LEVEL_LOW 45
#define POWER_CONSUMPTION_UPPER 10
#define POWER_CONSUMPTION_LOWER 5
#define SOLAR_PANEL_NOT_DEPLOYED_AMPLIFIER 3

#define ANALOG_MIN 0
#define ANALOG_MAX 1023
#define BATTERY_MIN 0
#define BATTERY_MAX 36

#define POWER_PIN A13

typedef struct {
    bool *solarPanelState;
    unsigned short *batteryLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
} PowerSubsystemData;

void powerSubsystem(void* powerSubsystemData);

#endif  /* _POWER_SUBSYSTEM_H_ */