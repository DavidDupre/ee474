#include "predefinedMacros.h"
#include "sharedVariables.h"
#include "thrusterSubsystem.h"
#include "powerSubsystem.h"
#include "consoleDisplay.h"
#include "satelliteComs.h"
#include "vehicleComms.h"
#include "warningAlarm.h"
#include "solarPanel.h"
#include "schedule.h"
#include "tft.h"

#include <AUnit.h>  // Test framework


unsigned int thrusterCommand;
unsigned short fuelLevel;
bool solarPanelState;
bool solarPanelDeploy;
bool solarPanelRetract;
unsigned short batteryLevel;
unsigned short powerConsumption;
unsigned short powerGeneration;
bool batteryLow;
bool fuelLow;
volatile unsigned int batteryLevelPtr[BATTERY_LEVEL_BUFFER_LENGTH];
bool solarPanelDeploy;
bool solarPanelRetract;

bool driveMotorSpeedInc;
bool driveMotorSpeedDec;
char vehicleCommand;
char vehicleResponse;

ThrusterSubsystemData thrusterSubsystemData = {
    &thrusterCommand,
    &fuelLevel
};

PowerSubsystemData powerSubsystemData = {
    &solarPanelState,
    &solarPanelDeploy,
    &solarPanelRetract,
    batteryLevelPtr,
    &powerConsumption,
    &powerGeneration
};

ConsoleDisplayData consoleDisplayData = {
    &solarPanelState,
    batteryLevelPtr,
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
    batteryLevelPtr,
    &fuelLevel,
    &powerConsumption,
    &powerGeneration,
    &thrusterCommand,
    &vehicleResponse
};

VehicleCommsData vehicleCommsData = {
    &vehicleCommand,
    &vehicleResponse
};

WarningAlarmData warningAlarmData = {
    &batteryLow,
    &fuelLow,
    batteryLevelPtr,
    &fuelLevel
};

SolarPanelControlData solarPanelControlData = {
    &solarPanelState,
    &solarPanelDeploy,
    &solarPanelRetract,
    &driveMotorSpeedInc,
    &driveMotorSpeedDec
};

TCB powerSubsystemTCB = {
    &powerSubsystemData,
    powerSubsystem,
    "Power Subsystem",
    NULL, NULL
};

TCB thrusterSubsystemTCB = {
    &thrusterSubsystemData,
    thrusterSubsystem,
    "Thruster Subsystem",
    NULL, NULL
};

TCB consoleDisplayTCB = {
    &consoleDisplayData,
    consoleDisplay,
    "Console Display",
    NULL, NULL
};

TCB satelliteComsTCB = {
    &satelliteComsData,
    satelliteComs,
    "Satellite Communications",
    NULL, NULL
};

TCB vehicleCommsTCB = {
    &vehicleCommsData,
    vehicleComms,
    "Vehicle Communications",
    NULL, NULL
};

TCB warningAlarmTCB = {
    &warningAlarmData,
    warningAlarm,
    "Warning/Alarm",
    NULL, NULL
};

TCB solarPanelControlTCB = {
    &solarPanelControlData,
    solarPanelControl,
    "Solar Panel Control",
    NULL, NULL
};

void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;
    solarPanelState = false;
    memset((unsigned int *) batteryLevelPtr, 0, BATTERY_LEVEL_BUFFER_LENGTH);
    solarPanelDeploy = false;
    solarPanelRetract = false;
    batteryLevel = 100;
    powerConsumption = 0;
    powerGeneration = 0;
    batteryLow = false;
    fuelLow = false;
    vehicleCommand = '\0';
    vehicleResponse = '\0';

    Serial.begin(250000);
    Serial1.begin(9600);
    tft.begin(tft.readID());
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    consoleDisplayInit();
    solarPanelControlInit();

    // initialize the task queue
#ifndef RUN_TESTS
    taskQueueInsert(&powerSubsystemTCB);
    taskQueueInsert(&thrusterSubsystemTCB);
    taskQueueInsert(&consoleDisplayTCB);
    taskQueueInsert(&satelliteComsTCB);
    taskQueueInsert(&warningAlarmTCB);
    taskQueueInsert(&vehicleCommsTCB);
#endif

    Serial.begin(9600);
    Serial1.begin(9600);
    tft.begin(TFT_IDENTIFIER);
    tft.setRotation(1);
    tft.fillScreen(BLACK);

    noInterrupts();           // disable all interrupts

    TCCR5A = 0;               // Timer/Counter Control Register 1 output compare register A

    TCCR5B = 0;               // Timer/Counter Control Register 1 output compare register B

    TCNT5  = 0;               // Timer/Counter Register 1


    OCR5A = 1040;            // compare match register 16MHz/256/60Hz  Output Compare Register

    TCCR5B |= (1 << WGM12);   // CTC mode

    TCCR5B |= (1 << CS12);    // 256 prescaler 

    TIMSK5 |= (1 << OCIE5A);  // enable timer compare interrupt   Timer/Counter Interrupt Mask Register

    interrupts();
}

void loop() {
#ifdef RUN_TESTS
    aunit::TestRunner::run();
#else
    schedule();
#endif  /* RUN_TESTS */
    return;
}
