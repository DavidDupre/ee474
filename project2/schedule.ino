#include "schedule.h"
#include "tcb.h"


// run all the tasks in a queue
void runTasks(TCB **taskQueue, unsigned short size);


unsigned long missionElapsedTime = 0;

void schedule(TCB **taskQueue, unsigned short size) {
    missionElapsedTime = millis();
    unsigned long majorStartTimeMs = missionElapsedTime;

    // do the first run through the queue
    runTasks(taskQueue, size);

    /*
     * Repeatedly through the task queue.
     * This dynamically calculates a "minor cycle" to be the duration of
     * another run through the task queue. This stops when there is less time
     * remaining than twice the length of the longest minor cycle.
     */
    unsigned long maxMinorCycle = 0;
    missionElapsedTime = millis();
    while (missionElapsedTime - majorStartTimeMs
        < MAJOR_CYCLE_DURATION_MS - maxMinorCycle * 2) {
        unsigned long minorCycleStartTime = missionElapsedTime;

        // run through the task queue
        runTasks(taskQueue, size);

        // update the global time base
        missionElapsedTime = millis();

        // update the max minor cycle time
        unsigned long minorCycleMs = missionElapsedTime - minorCycleStartTime;
        if (minorCycleMs > maxMinorCycle) {
            maxMinorCycle = minorCycleMs;
        }
    }

    // delay up to the major cycle duration
    missionElapsedTime = millis();
    if (missionElapsedTime < majorStartTimeMs + MAJOR_CYCLE_DURATION_MS) {
        delay(MAJOR_CYCLE_DURATION_MS - (missionElapsedTime - majorStartTimeMs));
    }

    return;
}

void runTasks(TCB **taskQueue, unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        TCB *tcb = taskQueue[i];
        tcb->task(tcb->data);
    }
}

unsigned long globalTimeBase() {
    return missionElapsedTime;
}
