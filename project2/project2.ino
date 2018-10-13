#include "thrusterSubsystem.h"
#include "consoleDisplay.h"

#include <Elegoo_GFX.h>    // Core graphics library
#include <Elegoo_TFTLCD.h> // Hardware-specific library

#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4

// Setup LCD display
Elegoo_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);


#define MAJOR_CYCLE_DURATION_MS 5000
#define TFT_IDENTIFIER 0x9341


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

ConsoleDisplayData consoleDisplayData = {
    &solarPanelState,
    &batteryLevel,
    &fuelLevel,
    &powerConsumption,
    &powerGeneration,
    &batteryLow,
    &fuelLow
};

TCB thrusterSubsystemTCB = {
    &thrusterSubsystemData,
    thrusterSubsystem
};

TCB consoleDisplayTCB = {
    &consoleDisplayData,
    consoleDisplay
};

TCB *majorTasks[] = {
    // power subsystem
    &thrusterSubsystemTCB,
    // satellite comms
    &consoleDisplayTCB
};

TCB *minorTasks[] = {
    &thrusterSubsystemTCB,
    // console display
    // warning alarm
    // blink LED? Maybe not a task
};

void setup() {
    thrusterCommand = 0;
    fuelLevel = 100;

    Serial.begin(9600);
    tft.begin(TFT_IDENTIFIER);
}

void loop() {
    // TODO maybe put all this in a dedicated schedule file

    unsigned long startTimeMs = millis();

    // run one major cycle
    setCycleMode(CycleModeMajor);
    runTasks(majorTasks, sizeof(majorTasks) / sizeof(TCB*));

    // run minor cycles until 5 seconds have passed
    setCycleMode(CycleModeMinor);
    while (millis() - startTimeMs < MAJOR_CYCLE_DURATION_MS) {
        runTasks(minorTasks, sizeof(minorTasks) / sizeof(TCB*));
    }
}

void setCycleMode(CycleMode mode) {
    if (mode == CycleModeMajor) {
        // TODO
        // set console display to info mode
        thrusterSubsystemData.thrusterCommand = &thrusterCommand;
    } else {
        // TODO
        // set console display to annunciation mode
        thrusterSubsystemData.thrusterCommand = NULL;
    }
}

void runTasks(TCB **taskQueue, unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        TCB *tcb = taskQueue[i];
        tcb->task(tcb->data);
    }
}
