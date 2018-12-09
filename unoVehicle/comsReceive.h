#pragma once

#include <stdint.h>


// returns true if the command was handled
typedef bool (*cmd_callback_fn)(char opcode, uint8_t*);


void comsRxInit();

void comsRxUpdate(void *cmdData);

void comsRxRegisterCallback(uint8_t cmdId, cmd_callback_fn callback);