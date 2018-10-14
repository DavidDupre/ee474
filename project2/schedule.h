#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#include "tcb.h"

#define MAJOR_CYCLE_DURATION_MS 5000
#define MINOR_CYCLE_DURATION_MS 1000


/******************************************************************************
 *
 * name: schedule
 *
 * inputs:
 *  taskQueue: an array of TCB pointers to call
 *  size: the length of the taskQueue
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
void schedule(TCB **taskQueue, unsigned short size);

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

#endif  /* _SCHEDULE_H_ */
