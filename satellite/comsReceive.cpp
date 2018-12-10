#include "schedule.h"
#include "comsReceive.h"
#include "comsTransmit.h"
#include "sharedVariables.h"
#include <Arduino.h>

#define MAX_COMMAND_HANDLERS 32
#define CMD_SYNC_PATTERN     0xFC
#define SERIAL_TIMEOUT_MS    100

// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_CMD_STATUS 9
#define TLMID_CMD_ACK    245

// special code to represent an unknown command ID
#define CMDID_ERR 255


typedef struct {
    uint8_t cmdId;
    cmd_callback_fn handle;
} CommandHandler;

TLM_PACKET {
    uint8_t numErrors;
    uint8_t numCmdHandlers;
} CmdTlmPacket;

TLM_PACKET {
    uint8_t cmdId;
} AckTlmPacket;


// reads an incoming command from Serial and dispatches it to a command handler
void processCommand(serial_bus *bus, CmdData *data);

// returns true if it found the sync byte
bool sync(serial_bus *bus);


CommandHandler commandHandlers[MAX_COMMAND_HANDLERS];
uint8_t numCommandHandlers = 0;

TCB comsRxTCB;
CmdData cmdData = {
    &numCmdErrors
};

CmdTlmPacket cmdTlmPacket;
AckTlmPacket ackTlmPacket;

serial_bus *serialBuses[] = {
    BUS_GROUND,
    BUS_VEHICLE
};


void comsRxInit() {
    tcbInit(
        &comsRxTCB,
        &cmdData,
        comsRxUpdate,
        TASKID_COMSRX,
        1
    );

    numCmdErrors = 0;
    memset(commandHandlers, 0, sizeof(commandHandlers));
    comsTxRegisterSender(BUS_GROUND, TLMID_CMD_STATUS, sizeof(cmdTlmPacket),
        &cmdTlmPacket);
    comsTxRegisterSender(BUS_GROUND, TLMID_CMD_ACK, sizeof(ackTlmPacket),
        &ackTlmPacket);
}

void comsRxUpdate(void *cmdData) {
    CmdData *data = (CmdData *) cmdData;

    // process commands from each serial bus
    for (uint8_t i = 0; i < sizeof(serialBuses) / sizeof(serial_bus *); i++) {
        serial_bus *bus = serialBuses[i];
        while (sync(bus)) {
            processCommand(bus, data);
        }
    }

    // send metadata telementry
    cmdTlmPacket.numErrors = *data->numErrors;
    cmdTlmPacket.numCmdHandlers = numCommandHandlers;
    comsTxSend(TLMID_CMD_STATUS);
}

void comsRxRegisterCallback(uint8_t cmdId, cmd_callback_fn callback) {
    if (numCommandHandlers >= MAX_COMMAND_HANDLERS) {
        Serial.println(F("ERROR! Too many command handlers!"));
        return;
    }
    commandHandlers[numCommandHandlers++] = {
        .cmdId = cmdId,
        .handle = callback
    };
}

bool sync(serial_bus *bus) {
    while (bus->available()) {
        if (bus->read() == CMD_SYNC_PATTERN) {
            return true;
        }
    }
    return false;
}

void processCommand(serial_bus *bus, CmdData *cmdData) {
    // read the header
    uint8_t length;
    uint8_t cmdId;
    bus->readBytes(&length, 1);
    bus->readBytes(&cmdId, 1);

    // read the body of the command
    uint8_t data[256];
    bus->readBytes(data, length - 3); // subtract 3 for the header bytes

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
        (*(cmdData->numErrors))++;
    }

    // if this was from the ground, update the ack packet and send it
    if (bus == BUS_GROUND) {
        ackTlmPacket.cmdId = handled ? cmdId : CMDID_ERR;
        comsTxSend(TLMID_CMD_ACK);
    }
}
