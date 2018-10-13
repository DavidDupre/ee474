#ifndef _CONSOLE_DISPLAY_H_
#define _CONSOLE_DISPLAY_H_


typedef struct {
    bool *solarPanelState;
    unsigned short *batteryLevel;
    unsigned short *fuelLevel;
    unsigned short *powerConsumption;
    unsigned short *powerGeneration;

    bool *fuelLow;
    bool *batteryLow;
} ConsoleDisplayData;


void consoleDisplay(void *consoleDisplayData);

#endif  /* _CONSOLE_DISPLAY_H_ */