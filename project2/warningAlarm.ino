#include <stdint.h>
#include "warningAlarm.h"
#include "colors.h"

#define HALF_DEPLETED   50
#define LOW_LEVEL       10
#define BATTERY_RATE    1000
#define FUEL_RATE       2000

void warningAlarm(void *warningAlarmData) {

    // Cast from void to correct type
    WarningAlarmData *data = (WarningAlarmData *) warningAlarmData;

    static bool fuelFlashed = false;
    static bool batteryFlashed = false;
    static unsigned long lastBatteryUpdateEpochMs = globalTimeBase();
    static unsigned long lastFuelUpdateEpochMs = globalTimeBase();

    unsigned long currentEpochMs = globalTimeBase();
    unsigned long timePassedMsBattery = currentEpochMs - lastBatteryUpdateEpochMs;
    unsigned long timePassedMsFuel = currentEpochMs - lastFuelUpdateEpochMs;

    // Update battery status
    tft.setTextSize(4);
    tft.setCursor(0,0);
    if (*(data->batteryLevel) <= HALF_DEPLETED) {
        // Only update flash if the last update time was over a second ago
        if (timePassedMsBattery >= BATTERY_RATE) {
            // Draw black over batter portion
            if (batteryFlashed) {
                tft.fillRect(0,0,300,30,BLACK);
                batteryFlashed = false;
            } else {
                if (*(data->batteryLevel) <= LOW_LEVEL) {
                    tft.setTextColor(RED);
                    *(data->batteryLow) = true;
                } else {
                    tft.setTextColor(ORANGE);
                    *(data->batteryLow) = false;
                }
                tft.print("BATTERY");
                batteryFlashed = true;
            }
            // Update last time
            lastBatteryUpdateEpochMs = currentEpochMs;
        }

    } else {
        tft.setTextColor(GREEN);
        tft.print("BATTERY");
        batteryFlashed = false;
        *(data->batteryLow) = false;
        // Last update time doesn't change if status is GREEN
        // This makes sure that the flashing starts immediately rather than
        // a second late
    }

    // Update fuel status
    tft.setCursor(200,0);
    if (*(data->fuelLevel) <= HALF_DEPLETED) {
        // Only update flash if the last update time was over two seconds ago
        if (timePassedMsFuel >= FUEL_RATE) {
            // Draw black over batter portion
            if (fuelFlashed) {
                tft.fillRect(200,0,300,30,BLACK);
                fuelFlashed = false;
            } else {
                if (*(data->fuelLevel) <= LOW_LEVEL) {
                    tft.setTextColor(RED);
                    *(data->fuelLow) = true;
                } else {
                    tft.setTextColor(ORANGE);
                    *(data->fuelLow) = false;
                }
                tft.print("FUEL");
                fuelFlashed = true;
            }
            // Update last time
            lastFuelUpdateEpochMs = currentEpochMs;
        }

    } else {
        tft.setTextColor(GREEN);
        tft.print("FUEL");
        fuelFlashed = false;
        *(data->fuelLow) = true;
        // Last update time doesn't change if status is GREEN
        // This makes sure that the flashing starts immediately rather than
        // a second late
    }

    return;
}