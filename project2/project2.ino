#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "satelliteComs.h"
#include "warningAlarm.h"
#include "schedule.h"
#include "colors.h"
#include "tcb.h"

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library
#include <AUnit.h>         // Test framework

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4

// include this to run tests instead of running normally
// comment out to run normally
// #define RUN_TESTS

// Setup LCD display
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


#define TFT_IDENTIFIER 0x9341


unsigned int thrusterCommand;
unsigned short fuelLevel;
bool solarPanelState;
unsigned short batteryLevel;
unsigned short powerConsumption;
unsigned short powerGeneration;
bool batteryLow;
bool fuelLow;

ThrusterSubsystemData thrusterSubsystemData = {
    &thrusterCommand,
    &fuelLevel
};

PowerSubsystemData powerSubsystemData = {
    &solarPanelState,
    &batteryLevel,
    &powerConsumption,
    &powerGeneration
};

ConsoleDisplayData consoleDisplayData = {
    &solarPanelState,
    &batteryLevel,
    &fuelLevel,
    &powerConsumption,
    &powerGeneration,
    &batteryLow,
    &fuelLow
};

SatelliteComsData satelliteComsData = {
    &fuelLow,
    &batteryLow,
    &solarPanelState,
    &batteryLevel,
    &fuelLevel,
    &powerConsumption,
    &powerGeneration,
    &thrusterCommand
};

WarningAlarmData warningAlarmData = {
    &batteryLow,
    &fuelLow,
    &batteryLevel,
    &fuelLevel
};

TCB powerSubsystemTCB = {
    &powerSubsystemData,
    powerSubsystem
};

TCB thrusterSubsystemTCB = {
    &thrusterSubsystemData,
    thrusterSubsystem
};

TCB consoleDisplayTCB = {
    &consoleDisplayData,
    consoleDisplay
};

TCB satelliteComsTCB = {
    &satelliteComsData,
    satelliteComs
};

TCB warningAlarmTCB = {
    &warningAlarmData,
    warningAlarm
};

TCB *taskQueue[] = {
    &powerSubsystemTCB,
    &thrusterSubsystemTCB,
    &satelliteComsTCB,
    &consoleDisplayTCB,
    &warningAlarmTCB,
    // blink LED? Maybe not a task
};

void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;
    fuelLow = false;
    batteryLow = false;
    solarPanelState = false;
    batteryLevel = 100;
    powerConsumption = 0;
    powerGeneration = 0;


    Serial.begin(9600);
    tft.begin(TFT_IDENTIFIER);
    tft.fillScreen(BLACK);
}

void loop() {
#ifdef RUN_TESTS
    aunit::TestRunner::run();
#else
    schedule(taskQueue, sizeof(taskQueue) / sizeof(TCB*));
#endif  /* RUN_TESTS */
    return;
}
