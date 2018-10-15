#include <stdint.h>
#include "consoleDisplay.h"
#include "colors.h"
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
    tft.print("Solar panel: ");
    if (*(data->solarPanelState)) {
        tft.println("deployed");
    } else {
        tft.println("not deployed");
    }

    tft.print("Battery Level: ");
    tft.print(*(data->batteryLevel));
    tft.println("/100");

    tft.print("Fuel Level: ");
    tft.print(*(data->fuelLevel));
    tft.println("/100");

    tft.print("Power Consumption: ");
    tft.println(*(data->powerConsumption));

    tft.print("Power Generation: ");
    tft.println(*(data->powerGeneration));
}