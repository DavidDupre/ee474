#include <stdint.h>
#include "warningAlarm.h"
#include "colors.h"

void warningAlarm(void *warningAlarmData) {

    // Cast from void to correct type
    WarningAlarmData *data = (WarningAlarmData *) warningAlarmData;

    static bool fuelFlashed = false;
    static bool batteryFlashed = false;
    static unsigned long lastBatteryUpdateEpochMs = millis();
    static unsigned long lastFuelUpdateEpochMs = millis();

    unsigned long currentEpochMs = millis();
    unsigned long timePassedMsBattery = currentEpochMs - lastBatteryUpdateEpochMs;
    unsigned long timePassedMsFuel = currentEpochMs - lastFuelUpdateEpochMs;

    // Update battery status
    tft.setCursor(0,0);
    if (*(data->batteryLevel) <= 50) {
        // Only update flash if the last update time was over a second ago
        if (timePassedMsBattery >= 1000) {
            // Draw black over batter portion
            if (batteryFlashed) {
                tft.fillRect(0,0,300,15,BLACK);
                batteryFlashed = false;
            } else {
                if (*(data->batteryLevel) <= 10) {
                    tft.setTextColor(RED);
                } else {
                    tft.setTextColor(ORANGE);
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
        // Last update time doesn't change if status is GREEN
        // This makes sure that the flashing starts immediately rather than
        // a second late
    }

    // Update fuel status
    tft.setCursor(0,15);
    if (*(data->fuelLevel) <= 50) {
        // Only update flash if the last update time was over two seconds ago
        if (timePassedMsFuel >= 2000) {
            // Draw black over batter portion
            if (fuelFlashed) {
                tft.fillRect(0,15,300,16,BLACK);
                fuelFlashed = false;
            } else {
                if (*(data->fuelLevel) <= 10) {
                    tft.setTextColor(RED);
                } else {
                    tft.setTextColor(ORANGE);
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
        // Last update time doesn't change if status is GREEN
        // This makes sure that the flashing starts immediately rather than
        // a second late
    }

    return;
}