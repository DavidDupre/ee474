#include "predefinedMacros.h"
#include "schedule.h"
#include <Arduino.h>


/*
 * Delay until an epoch has been reached, or do nothing if the current MET is
 * after the epoch. The updates the MET.
 */
void delayUntil(unsigned long epochMs);


unsigned long missionElapsedTime = 0;
TCB *taskQueueHead = NULL;
TCB *taskQueueTail = NULL;

void schedule() {
    unsigned short size = taskQueueLength();
    unsigned long majorStartTime = millis();

    // it's okay if integer division rounds this down because we will delay for
    // the remaining time
    unsigned int minorCyclesPerMajorCycle = MAJOR_CYCLE_DURATION_MS
                                            / MINOR_CYCLE_DURATION_MS;

    for (unsigned int i = 0; i < minorCyclesPerMajorCycle; i++) {
        // update the global time base
        missionElapsedTime = millis();

        // run through the task queue
        TCB *tcb = taskQueueHead;
        while (tcb != NULL) {
#ifdef GET_TIMES
            unsigned long taskStart = micros();
#endif  /* GET_TIMES */
            tcb->task(tcb->data);
#ifdef GET_TIMES
            unsigned long taskEnd = micros();
            if (0 == i) {
                Serial.print(tcb->name);
                Serial.print(F(" Task took: "));
                Serial.print(taskEnd - taskStart);
                Serial.println(F(" us to complete"));
            }
#endif  /* GET_TIMES */
            tcb = tcb->next;
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

void taskQueueInsert(TCB *node) {
    if (taskQueueHead == NULL) {
        taskQueueHead = node;
        taskQueueTail = node;
    } else {
        taskQueueTail->next = node;
        node->prev = taskQueueTail;
        taskQueueTail = node;
    }
    return;
}

void taskQueueDelete(TCB *node) {
    if (taskQueueHead == NULL) {
        // the queue is empty and there is nothing to delete
        return;
    } else if (taskQueueHead == taskQueueTail) {
        // there is only one item in the queue
        taskQueueHead = NULL;
        taskQueueTail = NULL;
    } else if (taskQueueHead == node) {
        // delete the head
        taskQueueHead = taskQueueHead->next;
    } else if (taskQueueTail == node) {
        // delete the tail
        taskQueueTail = taskQueueTail->prev;
    } else {
        // update the surrounding nodes to point to each other (if they exist)
        if (node->prev != NULL) {
            node->prev->next = node->next;
        }
        if (node->next != NULL) {
            node->next->prev = node->prev;
        }
    }
    node->prev = NULL;
    node->next = NULL;
}

unsigned short taskQueueLength() {
    unsigned short length = 0;
    TCB *node = taskQueueHead;
    while (node != NULL) {
        length++;
        node = node->next;
    }
    return length;
}
