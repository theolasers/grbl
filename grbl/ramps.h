/* 
 * File:   ramps.h
 * Author: arsi
 *
 * Created on September 1, 2014, 4:44 PM
 */
#ifndef RAMPS_H
#define	RAMPS_H

#include"cpu_map.h"
#include<stdint.h>
#include"fastio.h"

#define CHECK(var,pos) ((var) & (1<<(pos)))

#ifdef __cplusplus
extern "C" {
#endif

    void rampsInitCoolant();
    void rampsInitSpindle();
    void rampsInitSteppers();
    void rampsWriteDisable(uint8_t value);
    void rampsWriteSteps(uint8_t value);
    void rampsWriteDirections(uint8_t value);
    void rampsStartSpindle(uint16_t speed);
    void rampsStopSpindle();
    void rampsLeftSpindle();
    void rampsRightSpindle();
    void rampsCoolantOn();
    void rampsCoolantOff();
    void rampsInitLimits();

    uint16_t getSpindleSpeed();
    bool getSpindleStarted();
    bool get_x_motion();
    bool get_y_motion();
    void reset_x_motion();
    void reset_y_motion();

#ifdef __cplusplus
} // extern "C"
#endif

#endif	/* RAMPS_H */

