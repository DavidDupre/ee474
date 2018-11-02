#ifndef _POWER_SUBSYSTEM_H_
#define _POWER_SUBSYSTEM_H_

#include "solarPanel.h"

#define ANALOG_MIN 0
#define ANALOG_MAX 1023
#define BATTERY_MIN 0
#define BATTERY_MAX 36

#define BATTERY_PERCENT_TO_VOLTS(P) (((P) * BATTERY_MAX) / 100 - BATTERY_MIN)

#define BATTERY_LEVEL_HIGH BATTERY_PERCENT_TO_VOLTS(95)
#define BATTERY_LEVEL_MID  BATTERY_PERCENT_TO_VOLTS(50)
#define BATTERY_LEVEL_LOW  BATTERY_PERCENT_TO_VOLTS(10)

#define POWER_CONSUMPTION_UPPER 10
#define POWER_CONSUMPTION_LOWER 5
#define SOLAR_PANEL_NOT_DEPLOYED_AMPLIFIER 3
#define EXTERNAL_MEASUREMENT_EVENT_PIN A13
#define MEAUSURE_INTERRUPT_PIN 20
#define MEASURE_DELAY_MS 1


typedef struct {
    SolarPanelState *solarPanelState;
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