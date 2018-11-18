#include <stdint.h>
#include "consoleDisplay.h"
#include "tft.h"
#include "solarPanel.h"
#include "schedule.h"

#define TEXT_SIZE         2
#define TEXT_HEIGHT       8  // in multiples of TEXT_SIZE
#define TEXT_WIDTH        6  // in multiples of TEXT_SIZE
#define HEIGHT            50 // where the display starts
#define MAX_VALUE_STR_LEN 20


const char *labels[] = {
    "Solar panel: ",
    "Battery Level: ",
    "Fuel Level: ",
    "Power Consumption: ",
    "Power Generation: ",
    "Transport Distance"
};

void consoleDisplayInit() {
    tft.setCursor(0, HEIGHT);
    tft.setTextSize(TEXT_SIZE);
    tft.setTextColor(WHITE);
    for (unsigned short i = 0; i < sizeof(labels) / sizeof(const char *); i++) {
        tft.println(labels[i]);
    }
}

void printLabel(const char *label, uint8_t lineNum) {
    int16_t x = strlen(label) * TEXT_WIDTH * TEXT_SIZE;
    int16_t y = HEIGHT + lineNum * TEXT_HEIGHT * TEXT_SIZE;

    // clear the line
    tft.fillRect(x, y, TEXT_WIDTH * TEXT_SIZE * MAX_VALUE_STR_LEN,
            TEXT_HEIGHT * TEXT_SIZE, BLACK);

    // move to the part after the label
    tft.setCursor(x, y);
}

void consoleDisplay(void *consoleDisplayData) {
    // return early if less than 5 seconds have passed
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

    // Cast from void to correct type
    ConsoleDisplayData *data = (ConsoleDisplayData *) consoleDisplayData;
    tft.setTextSize(TEXT_SIZE);
    tft.setTextColor(YELLOW);

    uint8_t lineNum = 0;

    printLabel(labels[lineNum], lineNum);
    lineNum++;
    switch(*(data->solarPanelState)) {
        case SOLAR_PANEL_DEPLOYED:
            tft.print(F("deployed"));
            break;
        case SOLAR_PANEL_DEPLOYING:
            tft.print(F("deploying"));
            break;
        case SOLAR_PANEL_RETRACTING:
            tft.print(F("retracting"));
            break;
        case SOLAR_PANEL_RETRACTED:
            tft.print(F("retracted"));
            break;
    }

    printLabel(labels[lineNum], lineNum);
    lineNum++;
    tft.print(data->batteryLevelPtr[0]);
    tft.print(F("/36"));

    printLabel(labels[lineNum], lineNum);
    lineNum++;
    tft.print(*(data->fuelLevel));
    tft.print(F("/100"));

    printLabel(labels[lineNum], lineNum);
    lineNum++;
    tft.print(*(data->powerConsumption));

    printLabel(labels[lineNum], lineNum);
    lineNum++;
    tft.print(*(data->powerGeneration));

    printLabel(labels[lineNum], lineNum);
    lineNum++;
    tft.print(*(data->powerGeneration));
}