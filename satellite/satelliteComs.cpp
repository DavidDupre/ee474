#include "satelliteComs.h"
#include <stdint.h>
#include <Arduino.h>
#include "powerSubsystem.h"
#include "thrusterSubsystem.h"
#include "solarPanel.h"
#include "schedule.h"
#include "sharedVariables.h"


void printBool(bool input);

void printSolarPanelState(SolarPanelState state);


TCB satelliteComsTCB;

SatelliteComsData satelliteComsData = {
    .fuelLow = &fuelLow,
    .batteryLow = &batteryLow,
    .solarPanelState = &solarPanelState,
    .batteryLevelPtr = batteryLevelPtr,
    .batteryTempPtr = batteryTempPtr,
    .distanceBufferPtr = distanceBufferPtr,
    .fuelLevel = &fuelLevel,
    .powerConsumption = &powerConsumption,
    .powerGeneration = &powerGeneration,
    .thrusterCommand = &thrusterCommand,
    .vehicleCommand = &vehicleCommand,
    .vehicleResponse = &vehicleResponse,
    .imageData = imageData
};

void satelliteComsInit() {
    tcbInit(
        &satelliteComsTCB,
        &satelliteComsData,
        satelliteComs,
        TASKID_SATCOMS,
        1
    );
}

/******************************************************************************
 * name : satelliteComs
 *
 * inputs: satelliteComsData (void*)
 * satelliteComsData holds the pointers to the following data:
 * fuelLow: bool representing if the fuel is too low
 * batteryLow: bool representing if the battery is too low
 * solarPanelState: bool representing if the solar panel is deploued
 * batteryLevel: unsigned short representing the battery percentage
 * fuelLevel: unsigned short representing the fuel percentage
 * powerConsumption: unsigned short representing the power consumed by the system.
 * powerGeneration : unsigned short representing the power generated by the system.
 * thrusterCommand: unsigned short 16 bit thruster command
 *
 * outputs: void
 *
 * description:
 *
 * satelliteComs manages the communication from both the Earth to the Satellite and
 * the Satellite back to Earth. The primary communication from the Earth to the Satellite
 * is the thruster command. The primary communication from the Satellite to the Earth are the status
 * variables described above.
 *
 *
 * psuedocode:
 *
 * if first bit is a 0
 *  update thruster command to random command
 * else
 *  set thruster command to "invalid command" ~0
 *
 * send status info back to earth
 *
 *
 *
 *
 *  author: Nick Orlov
*****************************************************************************/

void satelliteComs(void* satelliteComsData) {
    // return early if less than 5 seconds have passed
    static unsigned long lastRunTime;
    if (globalTimeBase() - lastRunTime < MAJOR_CYCLE_DURATION_MS) {
        return;
    }
    lastRunTime = globalTimeBase();

    SatelliteComsData* data = (SatelliteComsData*) satelliteComsData;

    // Transferring data back to earth
    Serial.print(F("Fuel Low status is: "));
    printBool(*data->fuelLow);
    Serial.println();
    Serial.print(F("Battery Low status is: "));
    printBool(*data->batteryLow);
    Serial.println();
    Serial.print(F("Solar Panel state is: "));
    printSolarPanelState(*data->solarPanelState);
    Serial.println();
    Serial.print(F("Battery Level is: "));
    Serial.print(data->batteryLevelPtr[0]);
    Serial.println(F("V"));
    Serial.print(F("Fuel Level is: "));
    Serial.println(*data->fuelLevel);
    Serial.print(F("Power Consumption is: "));
    Serial.println(*data->powerConsumption);
    Serial.print(F("Power Generation is: "));
    Serial.println(*data->powerGeneration);
    Serial.print(F("Vehicle Response: A "));
    Serial.println(*data->vehicleResponse);
    *data->vehicleResponse = '\0';

#ifndef ENABLE_BINARY_COMS
    if (Serial.available()) {
        *data->vehicleCommand = Serial.read();
    }
#endif /* ENABLE_BINARY_COMS */

    Serial.print(F("Image Capture freq: "));
    Serial.print(data->imageData[0]);
    Serial.println(F(" Hz"));

    Serial.print(F("Battery Temperature: "));
    Serial.println(powerToCelsiusTemperature(data->batteryTempPtr));

    Serial.print(F("Transport Distance: "));
    Serial.print(data->distanceBufferPtr[0]);
    Serial.println(F(" m"));
}


void printBool(bool input) {
    if (input) {
        Serial.print(F("true"));
    } else {
        Serial.print(F("false"));
    }
}

void printSolarPanelState(SolarPanelState state) {
    switch(state) {
        case SOLAR_PANEL_DEPLOYED:
            Serial.print(F("deployed"));
            break;
        case SOLAR_PANEL_DEPLOYING:
            Serial.print(F("deploying"));
            break;
        case SOLAR_PANEL_RETRACTING:
            Serial.print(F("retracting"));
            break;
        case SOLAR_PANEL_RETRACTED:
            Serial.print(F("retracted"));
            break;
    }
}
