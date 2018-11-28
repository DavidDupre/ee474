#include "command.h"
#include "schedule.h"
#include "binarySatelliteComs.h"
#include "sharedVariables.h"
#include <Arduino.h>

#define MAX_COMMAND_HANDLERS 16
#define CMD_SYNC_PATTERN     0xFC
#define SERIAL_TIMEOUT_MS    100

// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_COMMAND 9


typedef struct {
    uint8_t cmdId;
    cmd_callback_fn handle;
} CommandHandler;

TLM_PACKET {
    uint8_t numErrors;
    uint8_t numCmdHandlers;
} CmdTlmPacket;


// reads an incoming command from Serial and dispatches it to a command handler
void processCommand(CmdData *data);

// returns true if it found the sync byte
bool sync();


CommandHandler commandHandlers[MAX_COMMAND_HANDLERS];
uint8_t numCommandHandlers = 0;

TCB cmdTCB;
CmdData cmdData = {
    &numCmdErrors
};

CmdTlmPacket cmdTlmPacket;


void cmdInit() {
    tcbInit(
        &cmdTCB,
        &cmdData,
        cmdUpdate,
        TASKID_COMMAND,
        1
    );

    Serial.setTimeout(SERIAL_TIMEOUT_MS);
    numCmdErrors = 0;
    memset(commandHandlers, 0, sizeof(commandHandlers));
}

void cmdUpdate(void *cmdData) {
    CmdData *data = (CmdData *) cmdData;

    // process commands
    while (sync()) {
        processCommand(data);
    }

    // send metadata telementry
    cmdTlmPacket.numErrors = *data->numErrors;
    bcSend(TLMID_COMMAND);
}

void cmdRegisterCallback(uint8_t cmdId, cmd_callback_fn callback) {
    if (numCommandHandlers >= MAX_COMMAND_HANDLERS) {
        Serial.println(F("ERROR! Too many command handlers!"));
        return;
    }
    commandHandlers[numCommandHandlers++] = {
        cmdId,
        callback
    };
}

bool sync() {
    while (Serial.available()) {
        if (Serial.read() == CMD_SYNC_PATTERN) {
            return true;
        }
    }
    return false;
}

void processCommand(CmdData *cmdData) {
    // read the header
    uint8_t length;
    uint8_t cmdId;
    Serial.readBytes(&length, 1);
    Serial.readBytes(&cmdId, 1);

    // read the body of the command
    uint8_t data[256];
    Serial.readBytes(data, length);

    // dispatch to different entities
    bool handled = false;
    for (uint8_t i = 0; i < numCommandHandlers; i++) {
        CommandHandler *h = &commandHandlers[i];
        if (cmdId == h->cmdId) {
            handled = h->handle(data);
            break;
        }
    }

    // report unhandled commands
    if (!handled) {
        cmdData->numErrors++;
    }
}
