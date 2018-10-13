#include <stdint.h>
#include "consoleDisplay.h"
#include "colors.h"

void consoleDisplay(void *consoleDisplayData) {

    // Cast from void to correct type
    ConsoleDisplayData *data = (ConsoleDisplayData *) consoleDisplayData;
    tft.setCursor(0,30);
    // Clear bottom part of screen without warnings
    tft.fillRect(0,30,300,500,BLACK);
    tft.print("Solar panel is ");
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
    tft.print(*(data->powerConsumption));
}