/*
  report.c - reporting and messaging methods
  Part of Grbl

  Copyright (c) 2012-2015 Sungeun K. Jeon  

  Grbl is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Grbl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Grbl.  If not, see <http://www.gnu.org/licenses/>.
*/

/* 
  This file functions as the primary feedback interface for Grbl. Any outgoing data, such 
  as the protocol status messages, feedback messages, and status reports, are stored here.
  For the most part, these functions primarily are called from protocol.c methods. If a 
  different style feedback is desired (i.e. JSON), then a user can change these following 
  methods to accomodate their needs.
*/

#include "grbl.h"
#include "MemoryFree.h"

static void storeFloat(float n, uint8_t decimal_places, char *dest) {
	uint8_t decimals = decimal_places;
	while (decimals >= 2) { // Quickly convert values expected to be E0 to E-4.
		n *= 100;
		decimals -= 2;
	}
	if (decimals) {
		n *= 10;
	}
	n += 0.5; // Add rounding factor. Ensures carryover through entire value.

	char buf[10];
	uint8_t i = 0;
	if (n < 0) {
		decimal_places++;
		buf[i++] = '-';
		n = -n;
	}

	// Generate digits backwards and store in string.
	uint32_t a = (long) n;
	buf[decimal_places] = '.'; // Place decimal point, even if decimal places are zero.
	while (a > 0) {
		if (i == decimal_places) {
			i++;
		} // Skip decimal point location
		buf[i++] = (a % 10) + '0'; // Get digit
		a /= 10;
	}
	while (i < decimal_places) {
		buf[i++] = '0'; // Fill in zeros to decimal point for (n < 1)
	}
	if (i == decimal_places) { // Fill in leading zero, if needed.
		i++;
		buf[i++] = '0';
	}

	while (*dest) ++dest;

	  // Print the generated string.
	  for (; i > 0; i--) {
	    *dest++ = (buf[i-1]);
	  }
	  *dest = '\0';
}

// Handles the primary confirmation protocol response for streaming interfaces and human-feedback.
// For every incoming line, this method responds with an 'ok' for a successful command or an 
// 'error:'  to indicate some error event with the line or some critical system error during 
// operation. Errors events can originate from the g-code parser, settings module, or asynchronously
// from a critical error, such as a triggered hard limit. Interface should always monitor for these
// responses.
// NOTE: In silent mode, all error codes are greater than zero.
// TODO: Install silent mode to return only numeric values, primarily for GUIs.
void report_status_message(uint8_t status_code) 
{
  if (status_code == 0) { // STATUS_OK
    printPgmString(PSTR("ok\r\n"));
  } else if (status_code == STATUS_BINARY_OK) {
	  printPgmString(PSTR("Bok\r\n"));
  } else if (status_code == STATUS_NO_RESPONSE) {
	  // Suprress response
  } else {
    printPgmString(PSTR("Error: "));
    #ifdef REPORT_GUI_MODE
      print_uint8_base10(status_code);
    #else
      switch(status_code) {          
        case STATUS_EXPECTED_COMMAND_LETTER:
        printPgmString(PSTR("Expected command letter")); break;
        case STATUS_BAD_NUMBER_FORMAT:
        printPgmString(PSTR("Bad number format")); break;
        case STATUS_INVALID_STATEMENT:
        printPgmString(PSTR("Invalid statement")); break;
        case STATUS_NEGATIVE_VALUE:
        printPgmString(PSTR("Value < 0")); break;
        case STATUS_SETTING_DISABLED:
        printPgmString(PSTR("Setting disabled")); break;
        case STATUS_SETTING_STEP_PULSE_MIN:
        printPgmString(PSTR("Value < 3 usec")); break;
        case STATUS_SETTING_READ_FAIL:
        printPgmString(PSTR("EEPROM read fail. Using defaults")); break;
        case STATUS_IDLE_ERROR:
        printPgmString(PSTR("Not idle")); break;
        case STATUS_ALARM_LOCK:
        printPgmString(PSTR("Alarm lock")); break;
        case STATUS_SOFT_LIMIT_ERROR:
        printPgmString(PSTR("Homing not enabled")); break;
        case STATUS_OVERFLOW:
        printPgmString(PSTR("Line overflow")); break;
        case STATUS_CHECKSUM:
        printPgmString(PSTR("Checksum mismatch")); break;
        case STATUS_NO_STREAM:
        printPgmString(PSTR("No binary stream in progress")); break;
        case STATUS_STREAM_FULL:
        printPgmString(PSTR("Binary stream buffer full")); break;
        case STATUS_NO_LINE_NUMBER:
        printPgmString(PSTR("No Line Number with checksum")); break;
        #ifdef MAX_STEP_RATE_HZ
          case STATUS_MAX_STEP_RATE_EXCEEDED: 
          printPgmString(PSTR("Step rate > 30kHz")); break;
        #endif      
        // Common g-code parser errors.
        case STATUS_GCODE_MODAL_GROUP_VIOLATION:
        printPgmString(PSTR("Modal group violation")); break;
        case STATUS_GCODE_UNSUPPORTED_COMMAND:
        printPgmString(PSTR("Unsupported command")); break;
        case STATUS_GCODE_UNDEFINED_FEED_RATE:
        printPgmString(PSTR("Undefined feed rate")); break;
        default:
          // Remaining g-code parser errors with error codes
          printPgmString(PSTR("Invalid gcode ID:"));
          print_uint8_base10(status_code); // Print error code for user reference
      }
    #endif  
    printPgmString(PSTR("\r\n"));
  }
}

// Prints alarm messages.
void report_alarm_message(int8_t alarm_code)
{
  printPgmString(PSTR("ALARM: "));
  #ifdef REPORT_GUI_MODE
    print_uint8_base10(alarm_code);
  #else
    switch (alarm_code) {
      case ALARM_HARD_LIMIT_ERROR: 
      printPgmString(PSTR("Hard limit")); break;
      case ALARM_SOFT_LIMIT_ERROR:
      printPgmString(PSTR("Soft limit")); break;
      case ALARM_ABORT_CYCLE: 
      printPgmString(PSTR("Abort during cycle")); break;
      case ALARM_PROBE_FAIL:
      printPgmString(PSTR("Probe fail")); break;
      case ALARM_HOMING_FAIL:
      printPgmString(PSTR("Homing fail")); break;
      case ALARM_TEMP_ERROR:
      printPgmString(PSTR("Temperature Limit")); break;
    }
  #endif
  printPgmString(PSTR("\r\n"));
  delay_ms(500); // Force delay to ensure message clears serial write buffer.
}

// Prints feedback messages. This serves as a centralized method to provide additional
// user feedback for things that are not of the status/alarm message protocol. These are
// messages such as setup warnings, switch toggling, and how to exit alarms.
// NOTE: For interfaces, messages are always placed within brackets. And if silent mode
// is installed, the message number codes are less than zero.
// TODO: Install silence feedback messages option in settings
void report_feedback_message(uint8_t message_code)
{
  printPgmString(PSTR("["));
  switch(message_code) {
    case MESSAGE_CRITICAL_EVENT:
    printPgmString(PSTR("Reset to continue")); break;
    case MESSAGE_ALARM_LOCK:
    printPgmString(PSTR("'$H'|'$X' to unlock")); break;
    case MESSAGE_ALARM_UNLOCK:
    printPgmString(PSTR("Caution: Unlocked")); break;
    case MESSAGE_ENABLED:
    printPgmString(PSTR("Enabled")); break;
    case MESSAGE_DISABLED:
    printPgmString(PSTR("Disabled")); break; 
    case MESSAGE_SAFETY_DOOR_AJAR:
    printPgmString(PSTR("Check Door")); break;
    case MESSAGE_PROGRAM_END:
    printPgmString(PSTR("Pgm End")); break;
    case MESSAGE_RESTORE_DEFAULTS:
    printPgmString(PSTR("Restoring defaults")); break;
  }
  printPgmString(PSTR("]\r\n"));
}


// Welcome message
void report_init_message()
{
  printPgmString(PSTR("\r\nGrbl RAMPS " GRBL_VERSION " ['$' for help]\r\n"));
}

// Grbl help message
void report_grbl_help() {
  #ifndef REPORT_GUI_MODE
    printPgmString(PSTR("$$ (view Grbl settings)\r\n"
                        "$# (view # parameters)\r\n"
                        "$G (view parser state)\r\n"
                        "$I (view build info)\r\n"
                        "$N (view startup blocks)\r\n"
                        "$x=value (save Grbl setting)\r\n"
                        "$Nx=line (save startup block)\r\n"
                        "$C (check gcode mode)\r\n"
                        "$X (kill alarm lock)\r\n"
                        "$H (run homing cycle)\r\n"
                        "~ (cycle start)\r\n"
                        "! (feed hold)\r\n"
                        "? (current status)\r\n"
                        "ctrl-x (reset Grbl)\r\n"));
  #endif
}


// Grbl global settings print out.
// NOTE: The numbering scheme here must correlate to storing in settings.c
void report_grbl_settings() {
	uint8_t i;
	for (i=0; i<get_settings_info_count(); i++) {
		settings_info_t settings_info;
		get_settings_info(i, &settings_info);
		uint8_t idx = settings_info.index;
		printPgmString(PSTR("$"));
		print_uint8_base10(idx);
		printPgmString(PSTR("="));
		float value = settings_fetch_global_setting(idx);
		uint8_t intVal = trunc(value);
		switch (settings_info.type) {
		case SETTINGS_TYPE_INT_BASE10:
			print_uint8_base10(intVal);
			break;
		case SETTINGS_TYPE_INT_BASE2:
			print_uint8_base10(intVal);
			break;
		case SETTINGS_TYPE_BOOL:
			print_uint8_base10(intVal);
			break;
		case SETTINGS_TYPE_FLOAT:
			printFloat_SettingValue(value);
			break;
		}

#ifndef REPORT_GUI_MODE
		printPgmString(PSTR(" ("));
		printPgmString(settings_info.description);

		switch (settings_info.type) {
		case SETTINGS_TYPE_INT_BASE10:
			break;
		case SETTINGS_TYPE_INT_BASE2:
			printPgmString(PSTR(" mask:"));
			print_uint8_base2(intVal);
			break;
		case SETTINGS_TYPE_BOOL:
			printPgmString(PSTR(", bool"));
			break;
		case SETTINGS_TYPE_FLOAT:
			break;
		}
		printPgmString(PSTR(")"));
#endif

		printPgmString(PSTR("\r\n"));
	}
}

void report_grbl_settings_readable() {
	uint8_t i;
	for (i=0; i<get_settings_info_count(); i++) {
		settings_info_t settings_info;
		get_settings_info(i, &settings_info);
		uint8_t idx = settings_info.index;

		char buffer[256];

		strcpy_P(buffer, PSTR("$"));
		storeFloat(idx, 1, buffer);
		strcat_P(buffer, PSTR("="));
		float value = settings_fetch_global_setting(idx);
		storeFloat(value, N_DECIMAL_SETTINGVALUE, buffer);

		strcat_P(buffer, PSTR(" ("));
		strcat_P(buffer, settings_info.description);
		strcat_P(buffer, PSTR(")"));

		strcat_P(buffer, PSTR(" ("));
		switch (settings_info.type) {
		case SETTINGS_TYPE_INT_BASE10:
			strcat_P(buffer, PSTR("int,"));
			break;
		case SETTINGS_TYPE_INT_BASE2:
			strcat_P(buffer, PSTR("mask,"));
			break;
		case SETTINGS_TYPE_BOOL:
			strcat_P(buffer, PSTR("bool,"));
			break;
		case SETTINGS_TYPE_FLOAT:
			strcat_P(buffer, PSTR("float,"));
			break;
		}
		storeFloat(settings_info.min, N_DECIMAL_SETTINGVALUE, buffer);
		strcat_P(buffer, PSTR(","));
		storeFloat(settings_info.max, N_DECIMAL_SETTINGVALUE, buffer);
		strcat_P(buffer, PSTR(","));
		storeFloat(settings_info.step, N_DECIMAL_SETTINGVALUE, buffer);
		strcat_P(buffer, PSTR(")"));

		printString(buffer);

		char *b;
		uint8_t chkSum = 0;
		for (b=buffer; *b; b++) {
			chkSum ^= *b;
		}

		printPgmString(PSTR(" ("));
		print_uint8_base10(chkSum);
		printPgmString(PSTR(")"));
		printPgmString(PSTR("\r\n"));
	}
}

// Prints current probe parameters. Upon a probe command, these parameters are updated upon a
// successful probe or upon a failed probe with the G38.3 without errors command (if supported). 
// These values are retained until Grbl is power-cycled, whereby they will be re-zeroed.
void report_probe_parameters()
{
  uint8_t i;
  float print_position[N_AXIS];
 
  // Report in terms of machine position.
  printPgmString(PSTR("[PRB:"));
  for (i=0; i< N_AXIS; i++) {
    print_position[i] = system_convert_axis_steps_to_mpos(sys.probe_position,i);
    printFloat_CoordValue(print_position[i]);
    if (i < (N_AXIS-1)) { printPgmString(PSTR(",")); }
  }
  printPgmString(PSTR(":"));
  print_uint8_base10(sys.probe_succeeded);
  printPgmString(PSTR("]\r\n"));
}


// Prints Grbl NGC parameters (coordinate offsets, probing)
void report_ngc_parameters()
{
  float coord_data[N_AXIS];
  uint8_t coord_select, i;
  for (coord_select = 0; coord_select <= SETTING_INDEX_NCOORD; coord_select++) { 
    if (!(settings_read_coord_data(coord_select,coord_data))) { 
      report_status_message(STATUS_SETTING_READ_FAIL); 
      return;
    } 
    printPgmString(PSTR("[G"));
    switch (coord_select) {
      case 6: printPgmString(PSTR("28")); break;
      case 7: printPgmString(PSTR("30")); break;
      default: print_uint8_base10(coord_select+54); break; // G54-G59
    }  
    printPgmString(PSTR(":"));         
    for (i=0; i<N_AXIS; i++) {
      printFloat_CoordValue(coord_data[i]);
      if (i < (N_AXIS-1)) { printPgmString(PSTR(",")); }
      else { printPgmString(PSTR("]\r\n")); }
    } 
  }
  printPgmString(PSTR("[G92:")); // Print G92,G92.1 which are not persistent in memory
  for (i=0; i<N_AXIS; i++) {
    printFloat_CoordValue(gc_state.coord_offset[i]);
    if (i < (N_AXIS-1)) { printPgmString(PSTR(",")); }
    else { printPgmString(PSTR("]\r\n")); }
  } 
  printPgmString(PSTR("[TLO:")); // Print tool length offset value
  printFloat_CoordValue(gc_state.tool_length_offset);
  printPgmString(PSTR("]\r\n"));
  report_probe_parameters(); // Print probe parameters. Not persistent in memory.
}


// Print current gcode parser mode state
void report_gcode_modes()
{
  printPgmString(PSTR("["));
  
  switch (gc_state.modal.motion) {
    case MOTION_MODE_SEEK : printPgmString(PSTR("G0")); break;
    case MOTION_MODE_LINEAR : printPgmString(PSTR("G1")); break;
    case MOTION_MODE_CW_ARC : printPgmString(PSTR("G2")); break;
    case MOTION_MODE_CCW_ARC : printPgmString(PSTR("G3")); break;
    case MOTION_MODE_NONE : printPgmString(PSTR("G80")); break;
    default: 
      printPgmString(PSTR("G38."));
      print_uint8_base10(gc_state.modal.motion - (MOTION_MODE_PROBE_TOWARD-2));
  }

  printPgmString(PSTR(" G"));
  print_uint8_base10(gc_state.modal.coord_select+54);
  
  switch (gc_state.modal.plane_select) {
    case PLANE_SELECT_XY : printPgmString(PSTR(" G17")); break;
    case PLANE_SELECT_ZX : printPgmString(PSTR(" G18")); break;
    case PLANE_SELECT_YZ : printPgmString(PSTR(" G19")); break;
  }
  
  if (gc_state.modal.units == UNITS_MODE_MM) { printPgmString(PSTR(" G21")); }
  else { printPgmString(PSTR(" G20")); }
  
  if (gc_state.modal.distance == DISTANCE_MODE_ABSOLUTE) { printPgmString(PSTR(" G90")); }
  else { printPgmString(PSTR(" G91")); }
  
  if (gc_state.modal.feed_rate == FEED_RATE_MODE_INVERSE_TIME) { printPgmString(PSTR(" G93")); }
  else { printPgmString(PSTR(" G94")); }
    
  switch (gc_state.modal.program_flow) {
    case PROGRAM_FLOW_RUNNING : printPgmString(PSTR(" M0")); break;
    case PROGRAM_FLOW_PAUSED : printPgmString(PSTR(" M1")); break;
    case PROGRAM_FLOW_COMPLETED : printPgmString(PSTR(" M2")); break;
  }

  switch (gc_state.modal.spindle) {
    case SPINDLE_ENABLE_CW : printPgmString(PSTR(" M3")); break;
    case SPINDLE_ENABLE_CCW : printPgmString(PSTR(" M4")); break;
    case SPINDLE_DISABLE : printPgmString(PSTR(" M5")); break;
  }
  
  switch (gc_state.modal.coolant) {
    case COOLANT_DISABLE : printPgmString(PSTR(" M9")); break;
    case COOLANT_FLOOD_ENABLE : printPgmString(PSTR(" M8")); break;
    #ifdef ENABLE_M7
      case COOLANT_MIST_ENABLE : printPgmString(PSTR(" M7")); break;
    #endif
  }
  
  printPgmString(PSTR(" T"));
  print_uint8_base10(gc_state.tool);
  
  printPgmString(PSTR(" F"));
  printFloat_RateValue(gc_state.feed_rate);
  
  #ifdef VARIABLE_SPINDLE
    printPgmString(PSTR(" S"));
    printFloat_RateValue(gc_state.spindle_speed);
  #endif

  printPgmString(PSTR("]\r\n"));
}

// Prints specified startup line
void report_startup_line(uint8_t n, char *line)
{
  printPgmString(PSTR("$N")); print_uint8_base10(n);
  printPgmString(PSTR("=")); printString(line);
  printPgmString(PSTR("\r\n"));
}


// Prints build info line
void report_build_info(char *line)
{
  printPgmString(PSTR("[" GRBL_VERSION "." GRBL_VERSION_BUILD ":"));
  printString(line);
  printPgmString(PSTR("]\r\n"));
}


// Prints the character string line Grbl has received from the user, which has been pre-parsed,
// and has been sent into protocol_execute_line() routine to be executed by Grbl.
void report_echo_line_received(char *line)
{
  printPgmString(PSTR("[echo: ")); printString(line);
  printPgmString(PSTR("]\r\n"));
}


 // Prints real-time data. This function grabs a real-time snapshot of the stepper subprogram 
 // and the actual location of the CNC machine. Users may change the following function to their
 // specific needs, but the desired real-time data report must be as short as possible. This is
 // requires as it minimizes the computational overhead and allows grbl to keep running smoothly, 
 // especially during g-code programs with fast, short line segments and high frequency reports (5-20Hz).
void report_realtime_status()
{
  // **Under construction** Bare-bones status report. Provides real-time machine position relative to 
  // the system power on location (0,0,0) and work coordinate position (G54 and G92 applied). Eventually
  // to be added are distance to go on block, processed block id, and feed rate. Also a settings bitmask
  // for a user to select the desired real-time data.
  uint8_t idx;
  int32_t current_position[N_AXIS]; // Copy current state of the system position variable
  memcpy(current_position,sys.position,sizeof(sys.position));
  float print_position[N_AXIS];
 
  // Report current machine state
  switch (sys.state) {
    case STATE_IDLE: printPgmString(PSTR("<Idle")); break;
    case STATE_TEMPERATURE: printPgmString(PSTR("<Temp")); break;
    case STATE_CYCLE: printPgmString(PSTR("<Run")); break;
    case STATE_HOLD: printPgmString(PSTR("<Hold")); break;
    case STATE_HOMING: printPgmString(PSTR("<Home")); break;
    case STATE_ALARM: printPgmString(PSTR("<Alarm")); break;
    case STATE_CHECK_MODE: printPgmString(PSTR("<Check")); break;
    case STATE_SAFETY_DOOR: printPgmString(PSTR("<Door")); break;
  }
 
  // If reporting a position, convert the current step count (current_position) to millimeters.
  if (bit_istrue(settings.status_report_mask,(BITFLAG_RT_STATUS_MACHINE_POSITION | BITFLAG_RT_STATUS_WORK_POSITION | BITFLAG_RT_STATUS_TEMPERATURE))) {
    system_convert_array_steps_to_mpos(print_position,current_position);
  }
  
  // Report machine position
  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_MACHINE_POSITION)) {
    printPgmString(PSTR(",MPos:")); 
    for (idx=0; idx< N_AXIS; idx++) {
      printFloat_CoordValue(print_position[idx]);
      if (idx < (N_AXIS-1)) { printPgmString(PSTR(",")); }
    }
  }
  
  // Report work position
  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_WORK_POSITION)) {
    printPgmString(PSTR(",WPos:")); 
    for (idx=0; idx< N_AXIS; idx++) {
      // Apply work coordinate offsets and tool length offset to current position.
      print_position[idx] -= gc_state.coord_system[idx]+gc_state.coord_offset[idx];
      if (idx == TOOL_LENGTH_OFFSET_AXIS) { print_position[idx] -= gc_state.tool_length_offset; }    
      printFloat_CoordValue(print_position[idx]);
      if (idx < (N_AXIS-1)) { printPgmString(PSTR(",")); }
    }
  }
        
  // Returns the number of active blocks are in the planner buffer.
  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_PLANNER_BUFFER)) {
    printPgmString(PSTR(",Buf:"));
    print_uint8_base10(plan_get_block_buffer_count());
  }

  // Report serial read buffer status
  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_SERIAL_RX)) {
    printPgmString(PSTR(",RX:"));
    print_uint8_base10(serial_get_rx_buffer_count());
  }
    
  #ifdef USE_LINE_NUMBERS
    // Report current line number
    printPgmString(PSTR(",Ln:")); 
    int32_t ln=0;
    plan_block_t * pb = plan_get_current_block();
    if(pb != NULL) {
      ln = pb->line_number;
    } 
    printInteger(ln);
  #endif
    
  #ifdef REPORT_REALTIME_RATE
    // Report realtime rate 
    printPgmString(PSTR(",F:")); 
    printFloat_RateValue(st_get_realtime_rate());
  #endif    
  
  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_LIMIT_PINS)) {
    printPgmString(PSTR(",Lim:"));
    print_unsigned_int8(limits_get_state(),2,N_AXIS);
  }
  
  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_FEED_POWER)) {
    printPgmString(PSTR(",P:"));
    printInteger((int) gc_state.spindle_speed);
    printPgmString(PSTR(",F:"));
    printInteger((int) gc_state.feed_rate);
  }

  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_TEMPERATURE)) {
    printPgmString(PSTR(",Temp:"));
    printFloat(get_current_temperature(), 1);
    printPgmString(PSTR("/"));
    printFloat(settings.temperature_soft_limit, 1);
  }

  if (bit_istrue(settings.status_report_mask,BITFLAG_RT_STATUS_MEMORY)) {
    printPgmString(PSTR(",FreeMemory:"));
    printInteger(freeMemory());
  }

  #ifdef REPORT_CONTROL_PIN_STATE 
    printPgmString(PSTR(",Ctl:"));
    print_uint8_base2(CONTROL_PIN & CONTROL_MASK);
  #endif
  
  printPgmString(PSTR(">\r\n"));
}

static uint8_t report_automatic_status_flag = false;

void report_automatic_status(uint8_t on) {
	report_automatic_status_flag = on;
}

uint8_t get_report_automatic_status() {
	return report_automatic_status_flag;
}
