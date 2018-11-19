#include "solarPanel.h"
#include "consoleKeypad.h"
#include "schedule.h"
#include "sharedVariables.h"
#include "binarySatelliteComs.h"
#include <Arduino.h>

// the max value of the solar panel speed
#define SOLAR_PANEL_MAX_SPEED          100

// the amount to add or substract to speed on each button press
#define SOLAR_PANEL_SPEED_INCREMENT    5

// the duty cycle that will stop the motor
#define SOLAR_PANEL_DUTY_CYCLE_STOPPED 127

// the value added to the stopped duty cycle to achieve max speed
#define SOLAR_PANEL_DUTY_CYCLE_RANGE   100

// opcodes for commands over Serial
#define SOLAR_PANEL_OPCODE_INC 0
#define SOLAR_PANEL_OPCODE_DEC 1


TLM_PACKET {
    uint16_t speed;
} SolarPanelTlmPacket;


void solarPanelStop();

bool solarPanelProcessCommand(uint8_t opcode, uint8_t *data);


TCB solarPanelControlTCB;

SolarPanelControlData solarPanelControlData = {
    &solarPanelState,
    &solarPanelDeploy,
    &solarPanelRetract,
    &driveMotorSpeedInc,
    &driveMotorSpeedDec
};

unsigned short solarPanelSpeed;

static SolarPanelTlmPacket tlmPacket;


void solarPanelControlInit() {
    tcbInit(
        &solarPanelControlTCB,
        &solarPanelControlData,
        solarPanelControl,
        TASKID_PANEL,
        1
    );

    solarPanelSpeed = 0;
    pinMode(PIN_SOLAR_PANEL_OUTPUT, OUTPUT);
    pinMode(PIN_SOLAR_PANEL_STOPPED, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_SOLAR_PANEL_STOPPED),
        solarPanelStop, RISING);

    bcRegisterTlmSender(TLMID_SOLAR_PANEL, sizeof(tlmPacket), &tlmPacket);
    bcRegisterCmdHandler(TASKID_PANEL, solarPanelProcessCommand);
}

void solarPanelControl(void *solarPanelControlData) {
    SolarPanelControlData *data
        = (SolarPanelControlData *) solarPanelControlData;

    // update speeds
    if (*data->driveMotorSpeedInc && solarPanelSpeed <= SOLAR_PANEL_MAX_SPEED
            - SOLAR_PANEL_SPEED_INCREMENT) {
        solarPanelSpeed += SOLAR_PANEL_SPEED_INCREMENT;
    }
    if (*data->driveMotorSpeedDec
            && solarPanelSpeed >= SOLAR_PANEL_SPEED_INCREMENT) {
        solarPanelSpeed -= SOLAR_PANEL_SPEED_INCREMENT;
    }
    *data->driveMotorSpeedInc = false;
    *data->driveMotorSpeedDec = false;

    // handle commands based on current state
    if (*data->solarPanelDeploy
            && *data->solarPanelState != SOLAR_PANEL_DEPLOYED
            && *data->solarPanelState != SOLAR_PANEL_DEPLOYING) {
        // start deploying
        *data->solarPanelState = SOLAR_PANEL_DEPLOYING;
        taskQueueInsert(&consoleKeypadTCB);
        solarPanelSpeed = SOLAR_PANEL_MAX_SPEED / 2;
    } else if (*data->solarPanelRetract
            && *data->solarPanelState != SOLAR_PANEL_RETRACTED
            && *data->solarPanelState != SOLAR_PANEL_RETRACTING) {
        // start retract
        *data->solarPanelState = SOLAR_PANEL_RETRACTING;
        taskQueueInsert(&consoleKeypadTCB);
        solarPanelSpeed = SOLAR_PANEL_MAX_SPEED / 2;
    }

    // after handling commands above, we can clear the flags again
    *data->solarPanelDeploy = false;
    *data->solarPanelRetract = false;

    // set PWM based on speed
    // this doesn't use a 500 ms period
    unsigned short dutyCycle;
    unsigned short dutyCycleOffset = solarPanelSpeed
        * SOLAR_PANEL_DUTY_CYCLE_RANGE / SOLAR_PANEL_MAX_SPEED;
    if (*data->solarPanelState == SOLAR_PANEL_RETRACTING) {
        // retracting
        dutyCycle = SOLAR_PANEL_DUTY_CYCLE_STOPPED - dutyCycleOffset;
    } else if (*data->solarPanelState == SOLAR_PANEL_DEPLOYING) {
        // deploying
        dutyCycle = SOLAR_PANEL_DUTY_CYCLE_STOPPED + dutyCycleOffset;
    } else {
        // stopped
        dutyCycle = SOLAR_PANEL_DUTY_CYCLE_STOPPED;
    }
    analogWrite(PIN_SOLAR_PANEL_OUTPUT, dutyCycle);

    // send telemetry
    tlmPacket.speed = solarPanelSpeed;
    bcSend(TLMID_SOLAR_PANEL);
}

bool solarPanelProcessCommand(uint8_t opcode, uint8_t *data) {
    switch(opcode) {
        case SOLAR_PANEL_OPCODE_INC:
            driveMotorSpeedInc = true;
            return true;
        case SOLAR_PANEL_OPCODE_DEC:
            driveMotorSpeedDec = true;
            return true;
        default:
            return false;
    }
}

void solarPanelStop() {
    solarPanelSpeed = 0;
    if (solarPanelState == SOLAR_PANEL_DEPLOYING) {
        solarPanelState = SOLAR_PANEL_DEPLOYED;
    } else if (solarPanelState == SOLAR_PANEL_RETRACTING) {
        solarPanelState = SOLAR_PANEL_RETRACTED;
    }
    taskQueueDeleteLater(&consoleKeypadTCB);
    taskQueueDeleteLater(&solarPanelControlTCB);
}
