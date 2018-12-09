#ifndef _CONSOLE_DISPLAY_H_
#define _CONSOLE_DISPLAY_H_

#include "solarPanel.h"
#include "schedule.h"

typedef enum {
    consoleOn,
    consoleOff,
    consoleTurningOff,
} ConsoleStatus;


typedef struct {
    SolarPanelState *solarPanelState;
    volatile unsigned int *batteryLevelPtr;
    volatile float *distanceBufferPtr;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;

    bool *batteryLow;
    bool *fuelLow;

    ConsoleStatus *cStatus;
} ConsoleDisplayData;


extern TCB consoleDisplayTCB;

void consoleDisplayInit();

void consoleDisplay(void *consoleDisplayData);

#endif  /* _CONSOLE_DISPLAY_H_ */