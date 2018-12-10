#include "comsTransmit.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>

#define MAX_TLM_SENDERS      64
#define TLM_SYNC_PATTERN     0xFC

// Telemetry IDs unique to the entire satellite
// Keep this in sync with COSMOS
#define TLMID_TLM_STATUS 0


typedef struct {
    serial_bus *bus;
    uint8_t tlmId;
    uint8_t length;
    uint8_t *data;
    bool ready;
} TelemetrySender;

TLM_PACKET {
    uint8_t numTlmSenders;
} MetaPacket;


void sendTelemetryPacket(serial_bus *bus, uint8_t tlmId, uint8_t *data,
        uint8_t size);


TCB comsTxTCB;
ComsTxData comsTxData;

TelemetrySender tlmSenders[MAX_TLM_SENDERS];
uint8_t numTlmSenders = 0;

MetaPacket metaPacket;


void comsTxInit() {
    tcbInit(
        &comsTxTCB,
        &comsTxData,
        comsTx,
        TASKID_COMSTX,
        1
    );

    memset(tlmSenders, 0, sizeof(tlmSenders));
    comsTxRegisterSender(BUS_GROUND, TLMID_TLM_STATUS, sizeof(metaPacket),
            &metaPacket);
}

void comsTx(void *comsTxData) {
    ComsTxData *data = (ComsTxData *) comsTxData;

    // send telemetry packets
    for (uint8_t i = 0; i < numTlmSenders; i++) {
        if (tlmSenders[i].ready) {
            sendTelemetryPacket(tlmSenders[i].bus, tlmSenders[i].tlmId,
                    tlmSenders[i].data, tlmSenders[i].length);
            tlmSenders[i].ready = false;
        }
    }

    // update meta packet
    metaPacket.numTlmSenders = numTlmSenders;
    comsTxSend(TLMID_TLM_STATUS);
}

void comsTxRegisterSender(serial_bus *bus, uint8_t tlmId, uint8_t length,
        void *data) {
    if (numTlmSenders >= MAX_TLM_SENDERS) {
        Serial.println(F("ERROR! Too many telementry senders!"));
        return;
    }
    tlmSenders[numTlmSenders++] = {
        bus,
        tlmId,
        length,
        (uint8_t *) data,
        false
    };
}

void comsTxSend(uint8_t tlmId) {
    for (uint8_t i = 0; i < numTlmSenders; i++) {
        if (tlmSenders[i].tlmId == tlmId) {
            tlmSenders[i].ready = true;
            return;
        }
    }
    Serial.print("unknown tlm id! ");
    Serial.println(tlmId);
}

void sendTelemetryPacket(serial_bus *bus, uint8_t tlmId, uint8_t *data,
        uint8_t size) {
    // write the header (sync byte, full length, and ID)
    bus->write(TLM_SYNC_PATTERN);
    bus->write(size + 3); // add 3 for these header bytes
    bus->write(tlmId);

    // write the data
    bus->write(data, size);
}
