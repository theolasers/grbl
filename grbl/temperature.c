/*
  temperature.c - temperature control
  Part of Marlin
  
 Copyright (C) 2011 Camiel Gubbels / Erik van der Zalm
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "temperature.h"

#include "thermistortables.h"
#include "ramps.h"
#include "fastio.h"

volatile unsigned int poller_count = 0;
volatile unsigned int poller_seconds = 0;
static volatile unsigned long current_temperature_raw_0 = 0;
static unsigned int last_poller_count = 0;
static unsigned int last_temp_check = 0;

static unsigned long raw_temp_0_value = 0;
static unsigned char temp_count = 0;
static float current_temperature = 0;

void temperature_init() {

	// Setup button pins
	SET_INPUT(PAUSE_PIN);
	SET_INPUT(START_PIN);
	SET_INPUT(DOOR_PIN);

	WRITE(PAUSE_PIN,HIGH);
	WRITE(START_PIN,HIGH);
	WRITE(DOOR_PIN,HIGH);

	SET_INPUT(BTN_EN1);
	SET_INPUT(BTN_EN2);
	SET_INPUT(BTN_ENC);

	WRITE(BTN_EN1,HIGH);  // setup the pins as inputs
	WRITE(BTN_EN2,HIGH);
	WRITE(BTN_ENC,HIGH);

	// Setup status LEDs
#ifdef X_MOTION_STATUS_LED
	SET_OUTPUT(X_MOTION_STATUS_LED);
#endif
#ifdef Y_MOTION_STATUS_LED
	SET_OUTPUT(Y_MOTION_STATUS_LED);
#endif
#ifdef SPINDLE_STATUS_LED
	SET_OUTPUT(SPINDLE_STATUS_LED);
#endif
#ifdef DOOR_STATUS_LED
	SET_OUTPUT(DOOR_STATUS_LED);
#endif

	// Temperature Set analog inputs
	ADCSRA = 1<<ADEN | 1<<ADSC | 1<<ADIF | 0x07;
	DIDR0 = 0;
	#ifdef DIDR2
		DIDR2 = 0;
	#endif
	#if defined(TEMP_0_PIN) && (TEMP_0_PIN > -1)
		#if TEMP_0_PIN < 8
		   DIDR0 |= 1 << TEMP_0_PIN;
		#else
		   DIDR2 |= 1<<(TEMP_0_PIN - 8);
		#endif
	#endif

    // Timer interupt
	TCCR3A = 0;
	TCCR3B = 0;
	TCNT3 = 0;

	OCR3A = 311; // 50 hz
	TCCR3B |= (1 << WGM32); // CTC mode
	TCCR3B |= (1 << CS30) | (1 << CS32);  // Set CS10 and CS12 bits for 1024 prescaler:
	TIMSK3 |= (1 << OCIE3B); // enable timer compare interrupt
}

#define PGM_RD_W(x)   (short)pgm_read_word(&x)
// Derived from RepRap FiveD extruder::getTemperature()
// For hot end temperature measurement.
static float analog2temp(unsigned long raw)
{
	float celsius = 0;
	uint8_t i;
	short (*tt)[][2] = (short (*)[][2])(HEATER_0_TEMPTABLE);

	for (i=1; i<HEATER_0_TEMPTABLE_LEN; i++)
	{
	  if (PGM_RD_W((*tt)[i][0]) > raw)
	  {
		celsius = PGM_RD_W((*tt)[i-1][1]) +
		  (raw - PGM_RD_W((*tt)[i-1][0])) *
		  (float)(PGM_RD_W((*tt)[i][1]) - PGM_RD_W((*tt)[i-1][1])) /
		  (float)(PGM_RD_W((*tt)[i][0]) - PGM_RD_W((*tt)[i-1][0]));
		break;
	  }
	}

	// Overflow: Set to last value in the table
	if (i == HEATER_0_TEMPTABLE_LEN) celsius = PGM_RD_W((*tt)[i-1][1]);

	return celsius;
}

void temperature_check()
{
	unsigned int poller_seconds_copy = poller_seconds;
	if (last_temp_check == poller_seconds_copy) {
		return;
	}
	last_temp_check = poller_seconds_copy;

	CRITICAL_SECTION_START
	unsigned int long current_temperature_raw_copy_0 = current_temperature_raw_0;
	CRITICAL_SECTION_END

	current_temperature = analog2temp(current_temperature_raw_copy_0);
	if (settings.temperature_soft_limit == 0.0f) return;

	if (getSpindleStarted()) {
		if (current_temperature > settings.temperature_hard_limit) {
			spindle_stop(); // Make sure spindle is stopped
			bit_true_atomic(sys_rt_exec_alarm, (EXEC_ALARM_TEMP_ERROR|EXEC_CRITICAL_EVENT)); // Indicate hard limit critical event
		} else if (sys.state != STATE_TEMPERATURE && current_temperature > settings.temperature_soft_limit) {
			printPgmString(PSTR("[Caution: Temperature Soft Limit Reached "));
			printFloat(current_temperature, 2);
			printPgmString(PSTR(" / "));
			printFloat(settings.temperature_soft_limit, 2);
			printPgmString(PSTR("]\r\n"));

			bit_true(sys_rt_exec_state, EXEC_TEMPERATURE);
		}
	} else if (settings.temperature_restart_limit != 0.0f) {
		if (sys.state == STATE_TEMPERATURE && current_temperature < settings.temperature_restart_limit) {
			bit_true(sys_rt_exec_state, EXEC_CYCLE_START);
		}
	}
}

float get_current_temperature()
{
	return current_temperature;
}

ISR(TIMER3_COMPB_vect)
{
#if defined(TEMP_0_PIN) && (TEMP_0_PIN > -1)
  #if TEMP_0_PIN > 7
    ADCSRB = 1<<MUX5;
  #else
    ADCSRB = 0;
  #endif
  ADMUX = ((1 << REFS0) | (TEMP_0_PIN & 0x07));
  ADCSRA |= 1<<ADSC; // Start conversion
#endif

#if defined(TEMP_0_PIN) && (TEMP_0_PIN > -1)
  raw_temp_0_value += ADC;
#endif

	++poller_count;
	if (poller_count - last_poller_count >= 50) {
		last_poller_count = poller_count;
		poller_seconds++;
	}

	if (++temp_count >= OVERSAMPLENR) {
		current_temperature_raw_0 = raw_temp_0_value;

		raw_temp_0_value = 0;
		temp_count = 0;
	}

	if (!READ(PAUSE_PIN)) {
		bit_true(sys_rt_exec_state, EXEC_FEED_HOLD);
	}
	if ((sys.state & STATE_HOLD) || (sys.state == STATE_IDLE) || (sys.state & STATE_TEMPERATURE)) {
		if (!READ(START_PIN)) {
			bit_true(sys_rt_exec_state, EXEC_CYCLE_START);
		}
	}
	if (!(sys.state & STATE_SAFETY_DOOR)) {
		if (!READ(DOOR_PIN)) {
			bit_true(sys_rt_exec_state, EXEC_SAFETY_DOOR);
		}
	}
	if (screen_button_pushed()) {
		bit_true(sys_rt_exec_state, EXEC_MENU);
	}

	// Update LEDs
#ifdef X_MOTION_STATUS_LED
	static bool x_led_state = false;
	if (get_x_motion()) {
		if (!x_led_state) {
			WRITE(X_MOTION_STATUS_LED, 255);
			x_led_state = true;
		}
		reset_x_motion();
	} else {
		if (x_led_state) {
			WRITE(X_MOTION_STATUS_LED, 0);
			x_led_state = false;
		}
	}
#endif
#ifdef Y_MOTION_STATUS_LED
	static bool y_led_state = false;
	if (get_y_motion()) {
		if (!y_led_state) {
			WRITE(Y_MOTION_STATUS_LED, 255);
			y_led_state = true;
		}
		reset_y_motion();
	} else {
		if (y_led_state) {
			WRITE(Y_MOTION_STATUS_LED, 0);
			y_led_state = false;
		}
	}
#endif
#ifdef SPINDLE_STATUS_LED
	static bool spindle_led_state = false;
	if (getSpindleStarted()) {
		if (!spindle_led_state) {
			WRITE(SPINDLE_STATUS_LED, 255);
			spindle_led_state = true;
		}
	} else {
		if (spindle_led_state) {
			WRITE(SPINDLE_STATUS_LED, 0);
			spindle_led_state = false;
		}
	}
#endif
#ifdef DOOR_STATUS_LED

#endif
}

unsigned int millis() {
	return poller_count * 20;
}
