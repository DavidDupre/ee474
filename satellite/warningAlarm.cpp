#include <stdint.h>
#include <Arduino.h>
#include "warningAlarm.h"
#include "schedule.h"
#include "sharedVariables.h"
#include "binarySatelliteComs.h"
#include "tft.h"

#define HALF_FUEL       50
#define LOW_FUEL        10
#define LOW_BATTERY     10
#define HALF_BATTERY    18
#define BATTERY_RATE    1000
#define FUEL_RATE       2000
#define ACK_PERIOD      15000

#define ACK_PIN         35


static bool handleCommand(uint8_t opcode, uint8_t *data);


TCB warningAlarmTCB;

WarningAlarmData warningAlarmData = {
    &batteryLow,
    &fuelLow,
    batteryLevelPtr,
    &fuelLevel,
    &batteryTempHigh,
    &temperatureAlarmAcked
};

void warningAlarmInit() {
    tcbInit(
        &warningAlarmTCB,
        &warningAlarmData,
        warningAlarm,
        TASKID_ALARM,
        1
    );

    temperatureAlarmAcked = false;

    pinMode(ACK_PIN, INPUT);

    // optionally acknowledge temperature through serial
    bcRegisterCmdHandler(TASKID_ALARM, handleCommand);
}

void warningAlarm(void *warningAlarmData) {

    // Cast from void to correct type
    WarningAlarmData *data = (WarningAlarmData *) warningAlarmData;

    static bool fuelFlashed = false;
    static bool batteryFlashed = false;
    static bool tempFlashed = false;
    static unsigned long lastBatteryUpdateEpochMs = globalTimeBase();
    static unsigned long lastFuelUpdateEpochMs = globalTimeBase();
    static unsigned long tempWarningStart = 0;

    unsigned long currentEpochMs = globalTimeBase();
    unsigned long timePassedMsBattery = currentEpochMs - lastBatteryUpdateEpochMs;
    unsigned long timePassedMsFuel = currentEpochMs - lastFuelUpdateEpochMs;

    // Update battery status
    tft.setTextSize(4);
    tft.setCursor(0,0);
    if (data->batteryLevelPtr[0] <= HALF_BATTERY) {
        // Only update flash if the last update time was over a second ago
        if (timePassedMsBattery >= BATTERY_RATE) {
            // Draw black over batter portion
            if (data->batteryLevelPtr[0] <= LOW_BATTERY) {
                tft.setTextColor(RED);
                *(data->batteryLow) = true;
            } else {
                tft.setTextColor(ORANGE);
                *(data->batteryLow) = false;
            }
            if (batteryFlashed) {
                tft.fillRect(0,0,300,30,BLACK);
                batteryFlashed = false;
            } else {
                tft.print(F("BATTERY"));
                batteryFlashed = true;
            }
            // Update last time
            lastBatteryUpdateEpochMs = currentEpochMs;
        }

    } else {
        tft.setTextColor(GREEN);
        tft.print(F("BATTERY"));
        batteryFlashed = false;
        *(data->batteryLow) = false;
        // Last update time doesn't change if status is GREEN
        // This makes sure that the flashing starts immediately rather than
        // a second late
    }

    // Update fuel status
    tft.setCursor(200,0);
    if (*(data->fuelLevel) <= HALF_FUEL) {
        // Only update flash if the last update time was over two seconds ago
        if (timePassedMsFuel >= FUEL_RATE) {
            // Draw black over batter portion
            if (*(data->fuelLevel) <= LOW_FUEL) {
                tft.setTextColor(RED);
                *(data->fuelLow) = true;
            } else {
                tft.setTextColor(ORANGE);
                *(data->fuelLow) = false;
            }
            if (fuelFlashed) {
                tft.fillRect(200,0,300,30,BLACK);
                fuelFlashed = false;
            } else {
                tft.print(F("FUEL"));
                fuelFlashed = true;
            }
            // Update last time
            lastFuelUpdateEpochMs = currentEpochMs;
        }

    } else {
        tft.setTextColor(GREEN);
        tft.print(F("FUEL"));
        fuelFlashed = false;
        *(data->fuelLow) = false;
        // Last update time doesn't change if status is GREEN
        // This makes sure that the flashing starts immediately rather than
        // a second late
    }

    *data->temperatureAlarmAcked |= digitalRead(ACK_PIN);

    if(*data->batteryTempHigh) {
        if (*data->temperatureAlarmAcked) {
            *data->temperatureAlarmAcked = false;
            Serial.println("Entered into flipping of batterytemphigh");
            *data->batteryTempHigh = false;
            tempWarningStart = 0;
            tft.fillRect(0, 150, 270, 60, BLACK);
        } else if(tempWarningStart == 0) {
            tempWarningStart = globalTimeBase();
            tft.setTextColor(RED);
            tft.setCursor(0, 150);
            tft.print(F("TEMPERATURE"));
        } else if (globalTimeBase() > tempWarningStart + ACK_PERIOD) {
            if (tempFlashed) {
                tft.fillRect(0, 150, 270, 60, BLACK);
            } else {
                tft.setTextColor(RED);
                tft.setCursor(0, 150);
                tft.print(F("TEMPERATURE"));
            }
            tempFlashed = !tempFlashed;
        }
    }

    return;
}

// handle command from serial
static bool handleCommand(uint8_t opcode, uint8_t *data) {
    temperatureAlarmAcked = true;
    return true;
}
