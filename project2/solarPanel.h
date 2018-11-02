#ifndef _SOLAR_PANEL_H_
#define _SOLAR_PANEL_H_

#define PIN_SOLAR_PANEL_STOPPED 21
#define PIN_SOLAR_PANEL_OUTPUT  44


typedef enum {
    SOLAR_PANEL_DEPLOYED = 0,
    SOLAR_PANEL_DEPLOYING,
    SOLAR_PANEL_RETRACTING,
    SOLAR_PANEL_RETRACTED
} SolarPanelState;

typedef struct {
    SolarPanelState *solarPanelState;
    bool *solarPanelDeploy;
    bool *solarPanelRetract;
    bool *driveMotorSpeedInc;
    bool *driveMotorSpeedDec;
} SolarPanelControlData;


void solarPanelControlInit();

void solarPanelControl(void *solarPanelData);

#endif /* _SOLAR_PANEL_H_ */
