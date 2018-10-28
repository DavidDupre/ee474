#include <stdint.h>
#include "consoleDisplay.h"
#include "tft.h"
#include "schedule.h"


void consoleDisplay(void *consoleDisplayData) {
    // return early if less than 5 seconds have passed
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

    // Cast from void to correct type
    ConsoleDisplayData *data = (ConsoleDisplayData *) consoleDisplayData;
    tft.setCursor(0,50);
    tft.setTextSize(2);
    tft.setTextColor(WHITE);
    // Clear bottom part of screen without warnings
    tft.fillRect(0,50,300,500,BLACK);
    tft.print(F("Solar panel: "));
    tft.setTextColor(YELLOW);
    if (*(data->solarPanelState)) {
        tft.println(F("deployed"));
    } else {
        tft.println(F("not deployed"));
    }

    tft.setTextColor(WHITE);
    tft.print(F("Battery Level: "));
    tft.setTextColor(YELLOW);
    tft.print(*(data->batteryLevel));
    tft.println(F("/100"));

    tft.setTextColor(WHITE);
    tft.print(F("Fuel Level: "));
    tft.setTextColor(YELLOW);
    tft.print(*(data->fuelLevel));
    tft.println(F("/100"));

    tft.setTextColor(WHITE);
    tft.print(F("Power Consumption: "));
    tft.setTextColor(YELLOW);
    tft.println(*(data->powerConsumption));

    tft.setTextColor(WHITE);
    tft.print(F("Power Generation: "));
    tft.setTextColor(YELLOW);
    tft.println(*(data->powerGeneration));
}