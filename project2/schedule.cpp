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

bool taskQueueIncludes(TCB *node) {
    TCB *i = taskQueueHead;
    while (i) {
        if (i == node) {
            return true;
        }
        i = i->next;
    }
    return false;
}

void taskQueueDelete(TCB *node) {
    // don't delete it if it's not in the queue
    if (!taskQueueIncludes(node)) {
        return;
    }

    // update surrounding nodes' links
    if (node->prev) {
        node->prev->next = node->next;
    }
    if (node->next) {
        node->next->prev = node->prev;
    }

    if (node == taskQueueHead) {
        if (node == taskQueueTail) {
            // if this is the only node, clear head and tail
            taskQueueHead = NULL;
            taskQueueTail = NULL;
        } else {
            // if this is just the head, move the head to the next item
            taskQueueHead = node->next;
        }
    } else if (node == taskQueueTail) {
        // if this is just the tail, move the tail to the previous item
        taskQueueTail = node->prev;
    }

    // clear this node's links
    node->next = NULL;
    node->prev = NULL;
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
