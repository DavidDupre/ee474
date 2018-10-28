#include "schedule.h"
#include <AUnit.h>  // Test framework

#define SCHEDULE_TEST_MAX_CALLS 5


typedef struct {
    unsigned short callCount;
    unsigned long timeCalled[SCHEDULE_TEST_MAX_CALLS];
    bool errored;
} ScheduleTestData;


void scheduleTestTask(void *scheduleTestData);


// test the schedule function
test(schedule) {
    unsigned short numTasks = 3;
    unsigned long errorThresholdMs = 10;

    // build a task queue for the test functions
    ScheduleTestData data[numTasks];
    TCB tcbs[numTasks];
    for (unsigned short i = 0; i < numTasks; i++) {
        // initialize the task data
        data[i].callCount = 0;
        for (unsigned short j = 0; j < SCHEDULE_TEST_MAX_CALLS; j++) {
            data[i].timeCalled[j] = 0;
        }
        data[i].errored = false;

        // initialize the TCBs
        tcbs[i].data = &data[i];
        tcbs[i].task = scheduleTestTask;
        tcbs[i].name = "";
        tcbs[i].next = NULL;
        tcbs[i].prev = NULL;

        // build the task queue
        taskQueueInsert(&tcbs[i]);
    }

    // record the start time
    unsigned long startTime = millis();

    // call schedule to do a major cycle
    schedule();

    // test that 5 seconds passed (give or take 10 ms)
    assertTrue(millis() > startTime);
    assertNear((unsigned long) (millis() - startTime),
            (unsigned long) MAJOR_CYCLE_DURATION_MS, errorThresholdMs);

    for (unsigned short i = 0; i < numTasks; i++) {
        assertFalse(data[i].errored);
        unsigned short expectedNumCalls = MAJOR_CYCLE_DURATION_MS
                / MINOR_CYCLE_DURATION_MS;
        assertEqual(data[i].callCount, expectedNumCalls);

        // test that the task was called once per minor cycle
        for (unsigned short j = 0; j < data[i].callCount; j++) {
            unsigned long expectedTimeCalled = startTime
                    + MINOR_CYCLE_DURATION_MS * j;
            unsigned long actualTimeCalled = data[i].timeCalled[j];
            assertNear(expectedTimeCalled, actualTimeCalled, errorThresholdMs);
        }
    }
}

void scheduleTestTask(void *scheduleTestData) {
    ScheduleTestData *data = (ScheduleTestData *) scheduleTestData;
    data->callCount++;
    if (data->callCount > SCHEDULE_TEST_MAX_CALLS) {
        data->errored = true;
    } else {
        data->timeCalled[data->callCount - 1] = globalTimeBase();
        delay(10);  // simulate a longer function taking some time
    }
}
