#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#define MAJOR_CYCLE_DURATION_MS 5000
#define MINOR_CYCLE_DURATION_MS 1000


typedef struct _tcb {
    void *data;
    void (*task)(void *data);
    const char *name;
    struct _tcb *next;
    struct _tcb *prev;
} TCB;


/******************************************************************************
 * name: scheduleInit
 *
 * inputs: void
 *
 * outputs: void
 *
 * description:
 *  scheduleInit initializes the global time base.
 *
 * author: David Dupre
 *****************************************************************************/
void scheduleInit();

/******************************************************************************
 * name: schedule
 *
 * inputs: void
 *
 * outputs: void
 *
 * description:
 *  schedule runs every task in the taskQueue at least once. Each task is run
 *  in order. This function then repeatedly runs through the taskQueue until
 *  the major cycle duration is over.
 *
 *  Note that a "cycle" is an interval of time, *not* necessarily a run through
 *  the task queue. A major cycle is a 5 second interval, and a minor cycle is
 *  a smaller interval. A run through the task queue can be multiple minor
 *  cycles in duration.
 *
 * psuedocode:
 *  for n minor cycles per major cycle
 *      run each task in the task queue
 *      delay until the minor cycle is over
 *  delay until the major cycle is over
 *
 * author: David Dupre
 *
 *****************************************************************************/
void schedule();

/******************************************************************************
 * name: globalTimebase
 *
 * inputs: void
 *
 * outputs: the mission elapsed time in milliseconds
 *
 * description:
 *  globalTimeBase is guaranteed to yield the same result for all tasks in one
 *  iteration through a task queue. Tasks should use this function instead of
 *  millis() to get the system time.
 *
 * author: David Dupre
 *****************************************************************************/
unsigned long globalTimeBase();

/******************************************************************************
 * name: taskQueueInsert
 *
 * inputs:
 *  node: the task control block to add
 *
 * outputs: void
 *
 * description:
 *  Add a task to the end of the queue
 *
 * author: David Dupre
 *****************************************************************************/
void taskQueueInsert(TCB *node);

/******************************************************************************
 * name: taskQueueDelete
 *
 * inputs:
 *  node: the task to remove
 *
 * outputs: void
 *
 * description:
 *  Remove a task from the queue if it is in the queue.
 *
 * author: David Dupre
 *****************************************************************************/
void taskQueueDelete(TCB *node);

/******************************************************************************
 * name: taskQueueIncludes
 *
 * inputs: a node
 *
 * outputs: true iff the input node is in the task queue
 *
 * description:
 *  check if the task queue contains a specific node
 *
 * author: David Dupre
 *****************************************************************************/
bool taskQueueIncludes(TCB *node);

/******************************************************************************
 * name: taskQueueLength
 *
 * inputs: void
 *
 * outputs: the length of the task queue
 *
 * description:
 *  get the length of the task queue
 *
 * author: David Dupre
 *****************************************************************************/
unsigned short taskQueueLength();

#endif  /* _SCHEDULE_H_ */
