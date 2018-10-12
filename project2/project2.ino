#define MAJOR_CYCLE_DURATION_MS 5000


typedef struct {
    void *data;
    void (*task)(void *data);
} TCB;

typedef enum {
    CycleModeMajor,
    CycleModeMinor
} CycleMode;


// set the mode to major or minor
void setCycleMode(CycleMode mode);

// run all the tasks in a queue
void runTasks(TCB **taskQueue, unsigned short size);


TCB *majorTasks[] = {
    // power subsystem
    // thruster task
    // satellite comms
    // console display
};

TCB *minorTasks[] = {
    // thruster task
    // console display
    // warning alarm
    // blink LED? Maybe not a task
};

void setup() {

}

void loop() {
    // TODO maybe put all this in a dedicated schedule file

    unsigned long startTimeMs = millis();

    // run one major cycle
    setCycleMode(CycleModeMajor);
    runTasks(majorTasks, sizeof(majorTasks) / sizeof(TCB*));

    // run minor cycles until 5 seconds have passed
    setCycleMode(CycleModeMinor);
    while (millis() - startTimeMs < MAJOR_CYCLE_DURATION_MS) {
        runTasks(minorTasks, sizeof(minorTasks) / sizeof(TCB*));
    }
}

void setCycleMode(CycleMode mode) {
    if (mode == CycleModeMajor) {
        // TODO
        // set console display to info mode
        // set thruster TCB's command to point to the global command varaible
    } else {
        // TODO
        // set console display to annunciation mode
        // set thruster TCB's command to NULL
    }
}

void runTasks(TCB **taskQueue, unsigned short size) {
    for (unsigned short i = 0; i < size; i++) {
        TCB *tcb = taskQueue[i];
        tcb->task(tcb->data);
    }
}
