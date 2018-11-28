#include "binarySatelliteComs.h"
#include "schedule.h"
#include "sharedVariables.h"
#include <Arduino.h>

#define MAX_TLM_SENDERS      16
#define TLM_SYNC_PATTERN     0xFC


typedef struct {
    TlmId tlmId;
    uint8_t length;
    uint8_t *data;
    bool ready;
} TelemetrySender;

TLM_PACKET {
    uint8_t numTlmSenders;
} MetaPacket;


void sendTelemetryPacket(TlmId tlmId, uint8_t *data, uint8_t size);


TCB bcTCB;
BCData bcData;

TelemetrySender tlmSenders[MAX_TLM_SENDERS];
uint8_t numTlmSenders = 0;

MetaPacket metaPacket;


void bcInit() {
    tcbInit(
        &bcTCB,
        &bcData,
        binarySatelliteComs,
        TASKID_BINCOMS,
        1
    );

    memset(tlmSenders, 0, sizeof(tlmSenders));
    bcRegisterTlmSender(TLMID_META, sizeof(metaPacket), &metaPacket);
}

void binarySatelliteComs(void *bcData) {
    BCData *data = (BCData *) bcData;

    // send telemetry packets
    for (uint8_t i = 0; i < numTlmSenders; i++) {
        if (tlmSenders[i].ready) {
            sendTelemetryPacket(tlmSenders[i].tlmId, tlmSenders[i].data,
                    tlmSenders[i].length);
            tlmSenders[i].ready = false;
        }
    }

    // update meta packet
    metaPacket.numTlmSenders = numTlmSenders;
    bcSend(TLMID_META);
}

void bcRegisterTlmSender(TlmId tlmId, uint8_t length, void *data) {
    if (numTlmSenders >= MAX_TLM_SENDERS) {
        Serial.println(F("ERROR! Too many telementry senders!"));
        return;
    }
    tlmSenders[numTlmSenders++] = {
        tlmId,
        length,
        (uint8_t *) data,
        false
    };
}

void bcSend(TlmId tlmId) {
    for (uint8_t i = 0; i < numTlmSenders; i++) {
        if (tlmSenders[i].tlmId == tlmId) {
            tlmSenders[i].ready = true;
            break;
        }
    }
}

void sendTelemetryPacket(TlmId tlmId, uint8_t *data, uint8_t size) {
    // write the header (sync byte, full length, and ID)
    Serial.write(TLM_SYNC_PATTERN);
    Serial.write(size + 3); // add 3 for these header bytes
    Serial.write(tlmId);

    // write the data
    Serial.write(data, size);
}
