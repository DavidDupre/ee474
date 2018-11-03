#ifndef _WARNING_ALARM_H_
#define _WARNING_ALARM_H_


typedef struct {
    bool *batteryLow;
    bool *fuelLow;
    volatile unsigned int *batteryLevelPtr;
    unsigned short *fuelLevel;
} WarningAlarmData;


void warningAlarm(void *warningAlarmData);

#endif  /* _WARNING_ALARM_H_ */