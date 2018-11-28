#include "predefinedMacros.h"
#include "schedule.h"
#include "binarySatelliteComs.h"
#include <Arduino.h>

// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_TIMES 5


TLM_PACKET {
    // the execution time of each task in microseconds
    // indexed by task ID
    uint32_t execTimeMicros[MAX_NUM_TASKS];
} TimePacket;


/*
 * Delay until an epoch has been reached, or do nothing if the current MET is
 * after the epoch. The updates the MET.
 */
void delayUntil(unsigned long epochMs);

/*
 * Delete all dying tasks from the task queue.
 */
void purgeTaskQueue();


unsigned long missionElapsedTime = 0;
TCB *taskQueueHead = NULL;
TCB *taskQueueTail = NULL;

TimePacket timePacket;


void scheduleInit() {
    missionElapsedTime = millis();

    bcRegisterTlmSender(TLMID_TIMES, sizeof(timePacket), &timePacket);
}

void schedule() {
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
            timePacket.execTimeMicros[tcb->taskId] = micros() - taskStart;
#endif  /* GET_TIMES */
            tcb = tcb->next;
        }

#ifdef GET_TIMES
        // send the execution times packet
        bcSend(TLMID_TIMES);
#endif /* GET_TIMES */

        // remove tasks marked for deletion
        purgeTaskQueue();

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

void purgeTaskQueue() {
    TCB *tcb = taskQueueHead;
    while (tcb != NULL) {
        TCB *next = tcb->next;
        if (tcb->status == TCBStatusDying) {
            taskQueueDelete(tcb);
        }
        tcb = next;
    }
}

unsigned long globalTimeBase() {
    return missionElapsedTime;
}

void taskQueueInsert(TCB *node) {
    // don't add a node that's already on the queue
    if (taskQueueIncludes(node)) {
        node->status = TCBStatusRunning;
        return;
    }

    if (taskQueueHead == NULL) {
        taskQueueHead = node;
        taskQueueTail = node;
    } else {
        // find the task to insert after (node before last node with lower
        // priority)
        TCB *other = taskQueueHead;
        while (other->next != NULL
                && other->next->priority <= node->priority) {
            other = other->next;
        }

        // insert after "other"
        if (other->next) {
            other->next->prev = node;
        } else {
            taskQueueTail = node;
        }
        node->next = other->next;
        node->prev = other;
        other->next = node;
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

void taskQueueDeleteLater(TCB *node) {
    node->status = TCBStatusDying;
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

void tcbInit(TCB *tcb, void *data, tcb_task_fn task, TaskId taskId,
       unsigned short priority) {
    tcb->data = data;
    tcb->task = task;
    tcb->taskId = taskId;
    tcb->priority = priority;
    tcb->next = NULL;
    tcb->prev = NULL;
    tcb->status = TCBStatusRunning;
}
