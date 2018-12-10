#ifndef _SCHEDULE_H_
#define _SCHEDULE_H_

#define MAJOR_CYCLE_DURATION_MS 5000
#define MINOR_CYCLE_DURATION_MS 1000

#define MAX_NUM_TASKS 16


/*
 * Task IDs are used to identify tasks in telemetry.
 * They must be unique and in the range of 0 through 15.
 *
 * They are used for command dipatch.
 * These are NOT used as telemetry IDs.
 */
typedef enum {
    TASKID_COMSTX =   0,
    TASKID_DISPLAY =  1,
    TASKID_KEYPAD =   2,
    TASKID_POWER =    3,
    TASKID_SATCOMS =  4,
    TASKID_SCHED =    5,
    TASKID_PANEL =    6,
    TASKID_THRUST =   7,
    TASKID_VEHCOMS =  8,
    TASKID_ALARM =    9,
    TASKID_DISTANCE = 11,
    TASKID_COMSRX  =  12,
    TASKID_PIRATE_DETECTION = 13,
    TASKID_PIRATE_MANAGEMENT = 14,
} TaskId;

typedef void (*tcb_task_fn)(void *data);

typedef enum {
    TCBStatusRunning, // the task should be run
    TCBStatusDying,   // the task will be removed after this minor cycle
} TCBStatus;

typedef struct _tcb {
    void *data;
    tcb_task_fn task;
    TaskId taskId;
    struct _tcb *next;
    struct _tcb *prev;
    unsigned short priority;
    TCBStatus status;
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
 *  Add a task to the queue before the first task of lower priority, or to the
 *  end if no such task exists on the queue.
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
 * name: taskQueueDeleteLater
 *
 * inputs:
 *  node: the task to remove
 *
 * outputs: void
 *
 * description:
 *  Schedule a task for deletion at the end of the next minor cycle. This is
 *  safer than taskQueueDelete.
 *
 * author: David Dupre
 *****************************************************************************/
void taskQueueDeleteLater(TCB *node);

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

void clearAll();

/******************************************************************************
 * name: tcbInit
 *
 * inputs:
 *  tcb: the output TCB
 *  data: the shared data used by the task
 *  task: the function called when this task is run
 *  taskId: a unique ID from schedule.h to identify this task in telemetry
 *  priority: a priority from 1 to 5 (inclusive) where 1 is highest priority.
 *      Higher priority tasks run before lower priority tasks.
 *
 * outputs: void
 *
 * description:
 *  initialize a TCB
 *
 * author: David Dupre
 *****************************************************************************/
void tcbInit(TCB *tcb, void *data, tcb_task_fn task, TaskId taskId,
    unsigned short priority);

#endif  /* _SCHEDULE_H_ */
