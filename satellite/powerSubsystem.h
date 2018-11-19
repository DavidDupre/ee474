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

#define MEASURE_TEMP_PIN_1 A14
#define MEASURE_TEMP_PIN_2 A15
#define MEAUSURE_INTERRUPT_PIN 20
#define MEASURE_DELAY_MS 1

#define TEMP_PERCENTAGE_CHANGE_WARNING .2
#define RAW_TEMP_MILLIVOLTS_MAX 325
#define MEASUREMENT_MILLIVOLTS_MAX 1023

#define NORMALIZATION_MULTIPLIER 10
#define CELSIUS_ADD_AMOUNT 33
#define CELSIUS_MULTIPLY_AMOUNT 32

#include "powerSubsystem.h"


typedef struct {
    SolarPanelState *solarPanelState;
    bool *solarPanelDeploy;
    bool *solarPanelRetract;
    volatile unsigned int *batteryLevelPtr;
    volatile unsigned int *batteryTempPtr;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;
    bool *batteryTempHigh;
} PowerSubsystemData;


extern TCB powerSubsystemTCB;

void powerSubsystemInit();

void measurementExternalInterruptISR();

void powerSubsystem(void* powerSubsystemData);

unsigned int celsiusTemperature(volatile unsigned int* batteryTempPtr);

#endif  /* _POWER_SUBSYSTEM_H_ */