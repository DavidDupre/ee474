#include "predefinedMacros.h"
#include "schedule.h"
#include "tcb.h"
#include <Arduino.h>

// Timing test names
char *taskNames[] = {
    "Power Subsystem Task",
    "Thruster Subsystem Task",
    "Satellite Communication Task",
    "Console Dipslay Task",
    "Warning Alarm Task"
};


/*
 * Delay until an epoch has been reached, or do nothing if the current MET is
 * after the epoch. The updates the MET.
 */
void delayUntil(unsigned long epochMs);


unsigned long missionElapsedTime = 0;

void schedule(TCB **taskQueue, unsigned short size) {
    unsigned long majorStartTime = millis();

    // it's okay if integer division rounds this down because we will delay for
    // the remaining time
    unsigned int minorCyclesPerMajorCycle = MAJOR_CYCLE_DURATION_MS
                                            / MINOR_CYCLE_DURATION_MS;

    for (unsigned int i = 0; i < minorCyclesPerMajorCycle; i++) {
        // update the global time base
        missionElapsedTime = millis();

        // run through the task queue
        for (unsigned short j = 0; j < size; j++) {
            TCB *tcb = taskQueue[j];
            unsigned long taskStart = micros(), taskEnd;
            tcb->task(tcb->data);
            taskEnd = micros();
            #ifdef GET_TIMES
            if (0 == i) {
                Serial.print(taskNames[j]);
                Serial.print(" took: ");
                Serial.print(taskEnd - taskStart);
                Serial.println(" us to complete");
            }
            #endif
        }

        // delay until the minor cycle is over
        delayUntil(majorStartTime + MINOR_CYCLE_DURATION_MS * (i + 1));
    }

    // delay until the major cycle is over
    delayUntil(majorStartTime + MAJOR_CYCLE_DURATION_MS);

    return;
}

void delayUntil(unsigned long epochMs) {
    missionElapsedTime = millis();
    if (missionElapsedTime < epochMs) {
        delay(epochMs - missionElapsedTime);
    }
}

unsigned long globalTimeBase() {
    return missionElapsedTime;
}

void setGlobalTimeBase(unsigned long epoch) {
    #ifdef RUN_TESTS
    missionElapsedTime = epoch;
    #endif
    return;
}
