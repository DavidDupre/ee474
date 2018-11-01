#ifndef _POWER_SUBSYSTEM_H_
#define _POWER_SUBSYSTEM_H_

#define HIGH_BATTERY 95
#define BATTERY_LEVEL_MID 50
#define BATTERY_LEVEL_LOW 45
#define POWER_CONSUMPTION_UPPER 10
#define POWER_CONSUMPTION_LOWER 5
#define SOLAR_PANEL_NOT_DEPLOYED_AMPLIFIER 3
#define EXTERNAL_MEASUREMENT_EVENT_PIN A13
#define MEAUSURE_INTERRUPT_PIN 45

#define ANALOG_MIN 0
#define ANALOG_MAX 1023
#define BATTERY_MIN 0
#define BATTERY_MAX 36


typedef struct {
    bool *solarPanelState;
    bool *solarPanelDeploy;
    bool *solarPanelRetract;
    volatile unsigned int *batteryLevelPtr;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
} PowerSubsystemData;

// struct Measurement {
//     volatile unsigned short data;
//     volatile struct Measurement* next;
// };

//extern struct Measurement* batteryHead;

void powerSubsystemInit();

void measurementExternalInterruptISR();

void powerSubsystem(void* powerSubsystemData);

#endif  /* _POWER_SUBSYSTEM_H_ */