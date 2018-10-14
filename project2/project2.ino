#include "thrusterSubsystem.h"
#include "consoleDisplay.h"
#include "warningAlarm.h"
#include "colors.h"

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


#define MAJOR_CYCLE_DURATION_MS 5000
#define TFT_IDENTIFIER 0x9341

#define SHOW_TIMES true

typedef struct {
    void *data;
    void (*task)(void *data);
} TCB;

typedef enum {
    CycleModeMajor,
    CycleModeMinor
} CycleMode;


// set the mode to major or minor
void setCycleMode(CycleMode mode);

// run all the tasks in a queue
void runTasks(TCB **taskQueue, unsigned short size);

// get the mission elapsed time in milliseconds
unsigned long globalTimeBase();


// TCB shared variables
unsigned int thrusterCommand;
unsigned short fuelLevel;
bool solarPanelState;
unsigned short batteryLevel;
unsigned short powerConsumption;
unsigned short powerGeneration;
bool batteryLow;
bool fuelLow;

// "private" variables
unsigned long missionElapsedTime;

ThrusterSubsystemData thrusterSubsystemData = {
    &thrusterCommand,
    &fuelLevel
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

WarningAlarmData warningAlarmData = {
    &batteryLow,
    &fuelLow,
    &batteryLevel,
    &fuelLevel
};

TCB thrusterSubsystemTCB = {
    &thrusterSubsystemData,
    thrusterSubsystem
};

TCB consoleDisplayTCB = {
    &consoleDisplayData,
    consoleDisplay
};

TCB warningAlarmTCB = {
    &warningAlarmData,
    warningAlarm
};

TCB *majorTasks[] = {
    // power subsystem
    &thrusterSubsystemTCB,
    // satellite comms
    &consoleDisplayTCB
};

String taskNames[] = {
    //"Power Subsystem",
    "Thruster Subsystem",
    //"Satellite Comms",
    "Console Display"
};

TCB *minorTasks[] = {
    &thrusterSubsystemTCB,
    // console display
    &warningAlarmTCB
    // blink LED? Maybe not a task
};

void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;
    batteryLevel = 100;

    Serial.begin(9600);
    tft.begin(TFT_IDENTIFIER);
    tft.fillScreen(BLACK);
}

void loop() {
#ifdef RUN_TESTS
    aunit::TestRunner::run();
#else
    // TODO maybe put all this in a dedicated schedule file

    unsigned long startTimeMs = millis();
    missionElapsedTime = startTimeMs;

    // run one major cycle
    setCycleMode(CycleModeMajor);
    runTasks(majorTasks, sizeof(majorTasks) / sizeof(TCB*));

    // run minor cycles until 5 seconds have passed
    setCycleMode(CycleModeMinor);
    missionElapsedTime = millis();
    while (missionElapsedTime - startTimeMs < MAJOR_CYCLE_DURATION_MS) {
        runTasks(minorTasks, sizeof(minorTasks) / sizeof(TCB*));
        missionElapsedTime = millis();
    }
#endif  /* RUN_TESTS */
    return;
}

void setCycleMode(CycleMode mode) {
    if (mode == CycleModeMajor) {
        // TODO
        // set console display to info mode
    } else {
        // TODO
        // set console display to annunciation mode
    }
}

void runTasks(TCB **taskQueue, unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        TCB *tcb = taskQueue[i];
        unsigned long startTimeUs = micros();
        tcb->task(tcb->data);
        unsigned long endTimeUs = micros();
        if (SHOW_TIMES) {
            Serial.print(taskNames[i]);
            Serial.print(" took: ");
            Serial.print(endTimeUs - startTimeUs);
            Serial.println(" us");
        }
    }
}

unsigned long globalTimeBase() {
    return missionElapsedTime;
}
