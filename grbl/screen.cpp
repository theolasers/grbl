#include "screen.h"
#include "lcd7920.h"
#include "Sd2PinMap.h"
#include "print.h"
#include "system.h"
#include "screenmenu.h"
#include "fastio.h"
#include "cpu_map.h"
#include "nuts_bolts.h"
#include "screenutil.h"
#include "screenmenuoperations.h"
#include "protocol_parser_intf.h"
#include "ramps.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

//U8GLIB_ST7920_128X64_4X u8g(52, 51, 53); // Software
//U8GLIB_ST7920_128X64_4X u8g(HARDWARE_SCREEN_PIN); // Hardware
static Lcd7920 lcd;

#define B01010100 84
#define B00010000 16
#define B01000000 64
#define B01 1
#define B10 2

#define BLEN_A 0
#define BLEN_B 1
#define BLEN_C 2
#define EN_A (1<<BLEN_A)
#define EN_B (1<<BLEN_B)
#define EN_C (1<<BLEN_C)
#define encrot0 0
#define encrot1 2
#define encrot2 3
#define encrot3 1

static int index = 0;
static char char_buffer[10];
static unsigned int last_poller_count = 0;
static int8_t buttons_total = 0;
static int8_t button_counter = 0;
static unsigned int button_idle_time = 0;
static unsigned int screen_start_delay = 100;

static float last_x, last_y, last_t, last_p, last_f, last_f_per;
static unsigned int last_time;
static uint8_t last_state, last_alarm, last_file;
static uint8_t x_pos, y_pos;

#define SCREEN_STATE_START 0
#define SCREEN_STATE_MENU 1
#define SCREEN_STATE_DISPLAY 2
#define SCREEN_STATE_WAIT 3

static uint8_t screen_state = SCREEN_STATE_START;
static ScreenMenu *screenMenu = 0;

#define LCD_STR_CARD     "\x7F"

extern PROGMEM LcdFont font16x16;    // in glcd10x10.cpp
extern PROGMEM LcdFont font10x10;    // in glcd10x10.cpp
extern PROGMEM LcdFont font7x8fb;    // in glcd10x10.cpp
extern PROGMEM LcdFont font5x5f;    // in glcd10x10.cpp
extern LcdImage lcd_logo;

Lcd7920 *getLCDLib() {
	return &lcd;
}

void screen_init()
{
	lcd.begin();

	lcd.bitmap_h(0, 0, lcd_logo.width, lcd_logo.height, lcd_logo.ptr);

	lcd.flush();
	lcd.startTimers();

	// Port mode to try to read both encoder values at one time
	DDRC = DDRC | B01010100;
	PORTC = PORTC | B01010100;
}

static void screen_update_file() {
	uint8_t current_file = is_protocol_parsing_file_input();

	if (current_file != last_file) {
		last_file = current_file;

		if (!current_file) {
			// Clear % bar only
			for (int y=48; y<=51; y++) {
				lcd.line(18, y, 125, y, PixelClear);
			}
			last_f_per = 0.0f;
		} else {
			// Clear file
			for (int y=37; y<=52; y++) {
				lcd.line(0, y, 128, y, PixelClear);
			}

			lcd.setFont(&font16x16);

			lcd.setCursor(0, 37);
			lcd.print(LCD_STR_CARD);
			lcd.setFont(&font10x10);
			lcd.setCursor(18, 37);
			lcd.print(get_protocol_parsing_file_name());
			lcd.line(18, 48, 125, 48, PixelSet);
			lcd.line(18, 51, 125, 51, PixelSet);
			lcd.line(18, 49, 18, 50, PixelSet);
			lcd.line(125, 49, 125, 50, PixelSet);
			lcd.setFont(&font7x8fb);
		}
	}
}

static void screen_update_file_per() {
	if (last_file) {
		float current_per = get_protocol_parsing_file_percentage();
		if (last_f_per != current_per) {
			int last_width = 107 * last_f_per + 18;
			last_f_per = current_per;

			int width = 107 * current_per + 18;
			lcd.line(last_width, 49, width, 49, PixelSet);
			lcd.line(last_width, 50, width, 50, PixelSet);
		}
	}
}

static void screen_update_file_time() {
	if (last_file) {
		unsigned int current_time = get_protocol_parsing_time();
		if (last_time != current_time) {
			last_time = current_time;

			lcd.setFont(&font5x5f);
			unsigned int current_time_mins = current_time / 60;
			unsigned int current_time_secs = current_time % 60;

			uint8_t c = 103;
			if (current_time_mins > 10) c -= 5;
			if (current_time_mins > 100) c -= 5;
			lcd.setCursor(c, 40);
			lcd.print(" ");
			lcd.print(current_time_mins, 10);
			lcd.print(":");
			if (current_time_secs < 10) {
				lcd.print("0");
			}
			lcd.print(current_time_secs, 10);
			lcd.print(" ");
			lcd.setFont(&font7x8fb);
		}
	}
}

static void screen_update_status()
{
	uint8_t current_state = sys.state;
	uint8_t current_alarm = sys_rt_exec_alarm;

	if ((last_state != current_state) ||
		(current_state == STATE_ALARM && last_alarm != current_alarm)) {

		last_state = current_state;
		last_alarm = current_alarm;

		lcd.setFont(&font10x10);
		lcd.setCursor(2, 54);

		switch (current_state) {
			case STATE_IDLE: lcd.printPgmString(PSTR("Idle")); break;
			case STATE_TEMPERATURE: lcd.printPgmString(PSTR("Temperature Wait")); break;
			case STATE_CYCLE: lcd.printPgmString(PSTR("Running")); break;
			case STATE_HOLD: lcd.printPgmString(PSTR("Hold")); break;
			case STATE_HOMING: lcd.printPgmString(PSTR("Homing Cycle")); break;
			case STATE_ALARM:
			{
				if (current_alarm & EXEC_ALARM_HARD_LIMIT) {
					lcd.printPgmString(PSTR("Alarm - Hard Limit"));
				} else if (current_alarm & EXEC_ALARM_SOFT_LIMIT) {
					lcd.printPgmString(PSTR("Alarm - Soft Limit"));
				} else if (current_alarm & EXEC_ALARM_ABORT_CYCLE) {
					lcd.printPgmString(PSTR("Alarm - Abort Cycle"));
				} else if (current_alarm & EXEC_ALARM_PROBE_FAIL) {
					lcd.printPgmString(PSTR("Alarm - Probe Fail"));
				} else if (current_alarm & EXEC_ALARM_HOMING_FAIL) {
					lcd.printPgmString(PSTR("Alarm - Homing Fail"));
				} else if (current_alarm & EXEC_ALARM_TEMP_ERROR) {
					lcd.printPgmString(PSTR("Alarm - Temperature"));
				} else {
					lcd.printPgmString(PSTR("Alarm"));
				}
			}
			break;
			case STATE_CHECK_MODE: lcd.printPgmString(PSTR("Check")); break;
			case STATE_SAFETY_DOOR: lcd.printPgmString(PSTR("Door Open")); break;
			default: break;
		}

		lcd.setFont(&font7x8fb);

		uint8_t c, r;
		lcd.getCursor(c, r);
		for (int y=54; y<=64; y++) {
			lcd.line(c, y, 128, y, PixelClear);
		}
	}
}

void screen_stage_update_x() {
	float x = sys.position[X_AXIS]/settings.steps_per_mm[X_AXIS];
	if (x != last_x) {
		last_x = x;

		ftostr31(last_x, char_buffer);
		char_buffer[6]=' ';
		char_buffer[7]='\0';

		lcd.textInvert(true);
		lcd.setCursor(x_pos, 13);
		lcd.print(char_buffer);
		lcd.textInvert(false);
	}
}

void screen_stage_update_y() {
	float y = sys.position[Y_AXIS]/settings.steps_per_mm[Y_AXIS];
	if (y != last_y) {
		last_y = y;

		ftostr31(last_y, char_buffer);
		char_buffer[6]=' ';
		char_buffer[7]='\0';

		lcd.textInvert(true);
		lcd.setCursor(y_pos, 13);
		lcd.print(char_buffer);
		lcd.textInvert(false);
	}
}

void screen_stage_update_p() {
	if (gc_state.spindle_speed != last_p) {
		last_p = gc_state.spindle_speed;

		ftostr4(last_p, char_buffer);
		char_buffer[4]=' ';
		char_buffer[5]='\0';

		lcd.textInvert(true);
		lcd.setCursor(x_pos, 25);
		lcd.print(char_buffer);
		lcd.textInvert(false);
	}
}

void screen_stage_update_t() {
	float current_temp = get_current_temperature();
	if (current_temp != last_t) {
		last_t = current_temp;

		ftostr31ns(last_t, &char_buffer[0]);
		char_buffer[5] = 0x10;
		char_buffer[6] = ' ';
		char_buffer[7] = '\0';

		lcd.setFont(&font10x10);
		lcd.setCursor(93, 1);
		lcd.print(char_buffer);
		lcd.setFont(&font7x8fb);
	}
}

void screen_stage_update_f() {
	if (gc_state.feed_rate != last_f) {
		last_f = gc_state.feed_rate;

		ftostr4(last_f, char_buffer);
		char_buffer[4]=' ';
		char_buffer[5]='\0';

		lcd.textInvert(true);
		lcd.setCursor(y_pos, 25);
		lcd.print(char_buffer);
		lcd.textInvert(false);
	}
}

void screen_drawmain() {
	screen_state = SCREEN_STATE_DISPLAY;

	lcd.clear();
	lcd.setFont(&font7x8fb);

	uint8_t tmp, col_pos;
	uint8_t xc_pos, yc_pos;

	for (int y=11; y<35; y++) {
		lcd.line(0, y, 128, y, PixelSet);
	}
	lcd.textInvert(true);
	lcd.setCursor(2, 13);
	lcd.printPgmString(PSTR("X"));
	lcd.getCursor(xc_pos, tmp);
	lcd.printPgmString(PSTR(": "));
	lcd.getCursor(x_pos, tmp);
	lcd.print("           ");
	lcd.getCursor(col_pos, tmp);
	lcd.printPgmString(PSTR("Y"));
	lcd.getCursor(yc_pos, tmp);
	lcd.printPgmString(PSTR(": "));
	lcd.getCursor(y_pos, tmp);

	lcd.setCursor(2, 25);
	lcd.printPgmString(PSTR("P"));
	lcd.setCursor(xc_pos, 25);
	lcd.printPgmString(PSTR(":"));
	lcd.setCursor(col_pos, 25);
	lcd.printPgmString(PSTR("F"));
	lcd.setCursor(yc_pos, 25);
	lcd.printPgmString(PSTR(":"));

	lcd.textInvert(false);

	last_x = -1.1f;
	last_y = -1.1f;
	last_p = -1.1f;
	last_f = -1.1f;
	last_t = -1.1f;
	last_file = 0;
	last_state = 255;
	last_alarm = 255;
	last_f_per = 0.0f;
	last_time = -1;
}

typedef void (*SCREEN_PART)();

#define SCREEN_PARTS_SIZE 9
static SCREEN_PART SCREEN_PARTS[SCREEN_PARTS_SIZE] = {
		screen_stage_update_x,
		screen_stage_update_y,
		screen_stage_update_p,
		screen_update_status,
		screen_stage_update_t,
		screen_stage_update_f,
		screen_update_file,
		screen_update_file_per,
		screen_update_file_time
};

static bool screen_button_pushed_state = false;
static unsigned int button_poller_count = 0;

bool screen_button_pushed() {
	if (button_poller_count) {
		if (poller_count - button_poller_count < 25) {
			return false;
		}
		button_poller_count = 0;
	}

	if (!READ(BTN_ENC)) {
		if (!screen_button_pushed_state) {
			screen_button_pushed_state = true;
			button_poller_count = poller_count;
			return true;
		}
	} else {
		screen_button_pushed_state = false;
	}
	return false;
}

/* returns change in encoder state (-1,0,1) */
static int8_t screen_button_rotate()
{
	// Read from the port
	uint8_t enc=0;
	uint8_t val = PINC;
	if (val & B00010000) {
		enc |= B01;
	}
	if (val & B01000000) {
		enc |= B10;
	}
	// Same but not using ports
	//if(READ(BTN_EN1)==0)  enc |= B01;
	//if(READ(BTN_EN2)==0)  enc |= B10;

	static int8_t enc_states[] = {0,-1,1,0,1,0,0,-1,-1,0,0,1,0,1,-1,0};
	static uint8_t old_AB = 0;
	/**/
	old_AB <<= 2;                   //remember previous state
	old_AB |= (enc  & 0x03 );  //add current state
	return ( enc_states[( old_AB & 0x0f )]);
}

void screen_update() {
	// Only update every poller tick, to slow down this poll rate
	unsigned int current_poller_count = poller_count;
	if (screen_state == SCREEN_STATE_START) {
		if (last_poller_count == current_poller_count) return;
		last_poller_count = current_poller_count;

		if (screen_start_delay > 0) {
			screen_start_delay --;
			if (screen_start_delay == 0) {
				screen_drawmain();
			}
		}
		return;
	} else if (screen_state == SCREEN_STATE_MENU) {
		if (screenMenu->getExited()) {
			delete screenMenu;
			screenMenu = 0;
			screen_drawmain();
			lcd.startTimers();
		} else {
			int rotationCount = screen_button_rotate();
			if (rotationCount) {
				button_idle_time = 0;
				buttons_total += rotationCount;
				if (button_counter++ >= 4) {
					if (buttons_total > 0) {
						screenMenu->cursorUp();
					} else if (buttons_total < 0) {
						screenMenu->cursorDown();
					}

					buttons_total = 0;
					button_counter = 0;
				}
			} else {
				// Clear the buffer if idle so errors don't accumulate
				if (button_idle_time == 0) {
					button_idle_time = poller_count;
				} else if (poller_count - button_idle_time > 10) {
					buttons_total = 0;
					button_counter = 0;
					button_idle_time = 0;
				}
			}

			if (screen_button_pushed()) {
				screenMenu->cursorSelected();
			}
		}
		return;
	} else if (screen_state == SCREEN_STATE_DISPLAY) {
		if (last_poller_count == current_poller_count) return;
		last_poller_count = current_poller_count;

		SCREEN_PART part = SCREEN_PARTS[index];
		part();

		index++;
		if (index >= SCREEN_PARTS_SIZE) index = 0;
	}
}

void screen_drawmenu() {
	screen_state = SCREEN_STATE_MENU;

	screen_button_pushed(); // Just in case someone is holding the button down

	lcd.stopTimers();
	screenMenu = new ScreenMenu();
	ScreenMenuItemDisplayerFactoryRoot *rootFactory =
			new ScreenMenuItemDisplayerFactoryRoot();
	screenMenu->pushScreenMenuItemDisplayerFactory(rootFactory);
}

void screen_enterwait(const char *string) {
	if (screen_state != SCREEN_STATE_WAIT) {
		screen_state = SCREEN_STATE_WAIT;

		lcd.stopTimers();
		lcd.clear();

		int pos = 2;
		for (int y=pos-1; y<pos+9; y++) {
			lcd.line(0, y, 128, y, PixelSet);
		}

		lcd.setFont(&font7x8fb);
		lcd.textInvert(true);
		lcd.setCursor(10, pos);
		lcd.printPgmString(string);
		lcd.textInvert(false);

		lcd.flush();
	}
}

void screen_leavewait() {
	if (screen_state == SCREEN_STATE_WAIT) {
		screen_state = SCREEN_STATE_MENU;
		screenMenu->drawScreenMenu();
	}
}

void screen_entermenu() {
	if (screen_state != SCREEN_STATE_MENU && screen_state != SCREEN_STATE_WAIT) {
		screen_drawmenu();
	}
}

