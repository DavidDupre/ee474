#include "schedule.h"
#include "tcb.h"


unsigned long missionElapsedTime = 0;

void schedule(TCB **taskQueue, unsigned short size) {
    unsigned long majorStartTimeMs = millis();

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
        if (millis() - missionElapsedTime < MINOR_CYCLE_DURATION_MS) {
            delay(MINOR_CYCLE_DURATION_MS - (millis() - missionElapsedTime));
        }
    }

    // delay until the major cycle is over
    if (millis() - majorStartTimeMs < MAJOR_CYCLE_DURATION_MS) {
        delay(MAJOR_CYCLE_DURATION_MS - (millis() - majorStartTimeMs));
    }

    return;
}

unsigned long globalTimeBase() {
    return missionElapsedTime;
}
