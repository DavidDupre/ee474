#ifndef _WARNING_ALARM_H_
#define _WARNING_ALARM_H_

#include "schedule.h"
#include "consoleDisplay.h"


typedef struct {
    bool *batteryLow;
    bool *fuelLow;
    volatile unsigned int *batteryLevelPtr;
    unsigned short *fuelLevel;
    bool *batteryTempHigh;
    bool *temperatureAlarmAcked;
    ConsoleStatus *cStatus;
} WarningAlarmData;


extern TCB warningAlarmTCB;

void warningAlarmInit();

void warningAlarm(void *warningAlarmData);

#endif  /* _WARNING_ALARM_H_ */