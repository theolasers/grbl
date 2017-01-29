#include "grbl.h"
#include "ramps.h"

#include"fastio.h"

#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

static bool x_motion = false, y_motion = false;
static uint16_t spindleSpeed = 0;

void rampsCoolantOn() {
#ifdef COOLANT_FLOOD_PIN
	WRITE(COOLANT_FLOOD_PIN, 255);
#endif
}

void rampsCoolantOff() {
#ifdef COOLANT_FLOOD_PIN
	WRITE(COOLANT_FLOOD_PIN, 0);
#endif
}

void rampsLeftSpindle() {
#ifdef SPINDLE_DIRECTION_PIN
	WRITE(SPINDLE_DIRECTION_PIN, 1);
#endif
}

void rampsRightSpindle() {
#ifdef SPINDLE_DIRECTION_PIN
	WRITE(SPINDLE_DIRECTION_PIN, 0);
#endif
}

bool getSpindleStarted() {
	return (spindleSpeed != 0);
}

uint16_t getSpindleSpeed() {
	return spindleSpeed;
}

bool get_x_motion() {
	return x_motion;
}

void reset_x_motion() {
	x_motion = false;
}

bool get_y_motion() {
	return y_motion;
}

void reset_y_motion() {
	y_motion = false;
}

void rampsStartSpindle(uint16_t speed) {
	spindleSpeed = speed;

	sbi(TCCR5A, COM5C1);
	OCR5C = speed; // set pwm duty
}

void rampsStopSpindle() {
	spindleSpeed = 0;
	sbi(TCCR5A, COM5C1);
	OCR5C = 0; // set pwm duty
}

void rampsInitCoolant() {
#ifdef COOLANT_FLOOD_PIN
	SET_OUTPUT(COOLANT_FLOOD_PIN);
#endif
}

void rampsInitSpindle() {
	spindleSpeed = 0;

	// Taken from init() method in ardunio/core/main.cpp
#if defined(TCCR5B) && defined(CS51) && defined(WGM50)
	sbi(TCCR5B, CS51);		// set timer 5 prescale factor to 64
	sbi(TCCR5B, CS50);
	sbi(TCCR5A, WGM50);
	sbi(TCCR5A, WGM51);		// put timer 5 in 10-bit phase correct pwm mode
#endif

	SET_OUTPUT(SPINDLE_ENABLE_PIN);
#ifdef SPINDLE_DIRECTION_PIN
	SET_OUTPUT(SPINDLE_DIRECTION_PIN);
#endif
}

/**
 * Perform port direction init for ramps for steppers
 */
void rampsInitSteppers() {
	SET_OUTPUT(X_STEP_PIN);
	SET_OUTPUT(Y_STEP_PIN);
	SET_OUTPUT(X_DIR_PIN);
	SET_OUTPUT(Y_DIR_PIN);
	SET_OUTPUT(X_ENABLE_PIN);
	SET_OUTPUT(Y_ENABLE_PIN);

#if defined(Z_STEP_PIN) && Z_STEP_PIN > -1
	SET_OUTPUT(Z_STEP_PIN);
	SET_OUTPUT(Z_DIR_PIN);
	SET_OUTPUT(Z_ENABLE_PIN);
#endif
}

void rampsInitLimits() {
#if defined(X_MIN_PIN) && X_MIN_PIN > -1
  SET_INPUT(X_MIN_PIN);
  #ifdef ENDSTOPPULLUP_XMIN
    WRITE(X_MIN_PIN,HIGH);
  #endif
#endif

#if defined(Y_MIN_PIN) && Y_MIN_PIN > -1
  SET_INPUT(Y_MIN_PIN);
  #ifdef ENDSTOPPULLUP_YMIN
    WRITE(Y_MIN_PIN,HIGH);
  #endif
#endif

#if defined(Z_MIN_PIN) && Z_MIN_PIN > -1
  SET_INPUT(Z_MIN_PIN);
  #ifdef ENDSTOPPULLUP_ZMIN
    WRITE(Z_MIN_PIN,HIGH);
  #endif
#endif

#if defined(X_MAX_PIN) && X_MAX_PIN > -1
  SET_INPUT(X_MAX_PIN);
  #ifdef ENDSTOPPULLUP_XMAX
    WRITE(X_MAX_PIN,HIGH);
  #endif
#endif

#if defined(Y_MAX_PIN) && Y_MAX_PIN > -1
  SET_INPUT(Y_MAX_PIN);
  #ifdef ENDSTOPPULLUP_YMAX
    WRITE(Y_MAX_PIN,HIGH);
  #endif
#endif

#if defined(Z_MAX_PIN) && Z_MAX_PIN > -1
  SET_INPUT(Z_MAX_PIN);
  #ifdef ENDSTOPPULLUP_ZMAX
    WRITE(Z_MAX_PIN,HIGH);
  #endif
#endif
}

/**
 * Perform set of EN driver signal
 * @param value
 */
void rampsWriteDisable(uint8_t value) {
	if (CHECK(value, STEPPERS_DISABLE_BIT)) {
		WRITE(X_ENABLE_PIN, 1);
		WRITE(Y_ENABLE_PIN, 1);
	} else {
		WRITE(X_ENABLE_PIN, 0);
		WRITE(Y_ENABLE_PIN, 0);
	}
}

/**
 * write stepper pulse
 * @param value
 */
void rampsWriteSteps(uint8_t value) {
	if (CHECK(value, X_STEP_BIT)) {
		x_motion = true;
		WRITE(X_STEP_PIN, 1);
	} else {
		WRITE(X_STEP_PIN, 0);
	}
	if (CHECK(value, Y_STEP_BIT)) {
		y_motion = true;
		WRITE(Y_STEP_PIN, 1);
	} else {
		WRITE(Y_STEP_PIN, 0);
	}
#if defined(Z_STEP_PIN) && Z_STEP_PIN > -1
	if (CHECK(value, Z_STEP_BIT)) {
		WRITE(Z_STEP_PIN, 1);
	} else {
		WRITE(Z_STEP_PIN, 0);
	}
#endif
}

/**
 * set stepper direction
 * @param value
 */
void rampsWriteDirections(uint8_t value) {
	if (CHECK(value, X_DIRECTION_BIT)) {
		WRITE(X_DIR_PIN, 1);
	} else {
		WRITE(X_DIR_PIN, 0);
	}
	if (CHECK(value, Y_DIRECTION_BIT)) {
		WRITE(Y_DIR_PIN, 1);
	} else {
		WRITE(Y_DIR_PIN, 0);
	}

#if defined(Z_STEP_PIN) && Z_STEP_PIN > -1
	if (CHECK(value, Z_DIRECTION_BIT)) {
		WRITE(Z_DIR_PIN, 0);
	} else {
		WRITE(Z_DIR_PIN, 1);
	}
#endif
}
