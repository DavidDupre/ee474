#ifndef _SOLAR_PANEL_H_
#define _SOLAR_PANEL_H_

#define PIN_SOLAR_PANEL_DEPLOYED  20
#define PIN_SOLAR_PANEL_RETRACTED 21
#define PIN_SOLAR_PANEL_OUTPUT    44


typedef struct {
    bool *solarPanelState;
    bool *solarPanelDeploy;
    bool *solarPanelRetract;
    bool *driveMotorSpeedInc;
    bool *driveMotorSpeedDec;
} SolarPanelControlData;


void solarPanelControlInit();

void solarPanelControl(void *solarPanelData);

#endif /* _SOLAR_PANEL_H_ */
