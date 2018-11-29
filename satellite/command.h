#pragma once

#include <stdint.h>
#include "schedule.h"


// returns true if the command was handled
typedef bool (*cmd_callback_fn)(uint8_t*);

typedef struct {
    uint8_t *numErrors;
} CmdData;


extern TCB cmdTCB;


void cmdInit();

void cmdUpdate(void *cmdData);

void cmdRegisterCallback(uint8_t cmdId, cmd_callback_fn callback);
