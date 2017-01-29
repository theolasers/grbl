#ifdef GRBL_PLATFORM
#error "cpu_map already defined: GRBL_PLATFORM=" GRBL_PLATFORM
#endif

#define GRBL_PLATFORM "Atmega2560Ramps"

// Serial port pins
#define SERIAL_RX USART0_RX_vect
#define SERIAL_UDRE USART0_UDRE_vect

#define SERIAL_RX_1 USART1_RX_vect
#define SERIAL_UDRE_1 USART1_UDRE_vect

// Increase Buffers to make use of extra SRAM
//#define RX_BUFFER_SIZE		256
//#define TX_BUFFER_SIZE		128
//#define BLOCK_BUFFER_SIZE	36
//#define SEGMENT_BUFFER_SIZE 12
//#define LINE_BUFFER_SIZE	256

// Define step pulse output pins. Changed for Ramps
#define STEP_DDR      DDRA
#define STEP_PORT     PORTA
#define STEP_PIN      PINA
#define X_STEP_BIT        2 // Not realy used
#define Y_STEP_BIT        3 // Not realy used
#define Z_STEP_BIT        4 // Not realy used
#define STEP_MASK ((1<<X_STEP_BIT)|(1<<Y_STEP_BIT)) // All step bits

// Define step direction output pins. Changed for Ramps
#define DIRECTION_DDR      DDRA
#define DIRECTION_PORT     PORTA
#define DIRECTION_PIN      PINA
#define X_DIRECTION_BIT   5 // Not realy used
#define Y_DIRECTION_BIT   6 // Not realy used
#define Z_DIRECTION_BIT   7 // Not realy used
#define DIRECTION_MASK ((1<<X_DIRECTION_BIT)|(1<<Y_DIRECTION_BIT)) // All direction bits

// Define stepper driver enable/disable output pin. Changed for Ramps
#define STEPPERS_DISABLE_DDR   DDRB
#define STEPPERS_DISABLE_PORT  PORTB
#define STEPPERS_DISABLE_BIT   7 // Not realy used
#define STEPPERS_DISABLE_MASK (1<<STEPPERS_DISABLE_BIT)

// NOTE: All limit bit pins must be on the same port
#define LIMIT_DDR       DDRB
#define LIMIT_PORT      PORTB
#define LIMIT_PIN       PINB
#define X_LIMIT_BIT     3 // Not realy used
#define Y_LIMIT_BIT     2 // Not realy used
#define Z_LIMIT_BIT     1 // Not realy used
#define LIMIT_INT       PCIE0  // Pin change interrupt enable pin
#define LIMIT_INT_vect  PCINT0_vect
#define LIMIT_PCMSK     PCMSK0 // Pin change interrupt register
#define LIMIT_MASK ((1<<X_LIMIT_BIT)|(1<<Y_LIMIT_BIT)) // All limit bits

// Define spindle enable and spindle direction output pins.
#define SPINDLE_ENABLE_DDR   DDRH
#define SPINDLE_ENABLE_PORT  PORTH
#define SPINDLE_ENABLE_BIT   3 // MEGA2560 Digital Pin 6
#define SPINDLE_DIRECTION_DDR   DDRE
#define SPINDLE_DIRECTION_PORT  PORTE
#define SPINDLE_DIRECTION_BIT   3 // MEGA2560 Digital Pin 5
#define PWM_MAX_VALUE    1023.0 // 10 bit

// Define flood and mist coolant enable output pins.
// NOTE: Uno analog pins 4 and 5 are reserved for an i2c interface, and may be installed at
// a later date if flash and memory space allows.
#define COOLANT_FLOOD_DDR   DDRH
#define COOLANT_FLOOD_PORT  PORTH
#define COOLANT_FLOOD_BIT   5 // MEGA2560 Digital Pin 8
#ifdef ENABLE_M7 // Mist coolant disabled by default. See config.h to enable/disable.
#define COOLANT_MIST_DDR   DDRH
#define COOLANT_MIST_PORT  PORTH
#define COOLANT_MIST_BIT   6 // MEGA2560 Digital Pin 9
#endif

// Define user-control pinouts (cycle start, reset, feed hold) input pins.
// NOTE: All pinouts pins must be on the same port and not on a port with other input pins (limits).
#define PINOUT_DDR       DDRK
#define PINOUT_PIN       PINK
#define PINOUT_PORT      PORTK
#define PIN_RESET        0  // MEGA2560 Analog Pin 8
#define PIN_FEED_HOLD    1  // MEGA2560 Analog Pin 9
#define PIN_CYCLE_START  2  // MEGA2560 Analog Pin 10
#define PINOUT_INT       PCIE2  // Pin change interrupt enable pin
#define PINOUT_INT_vect  PCINT2_vect
#define PINOUT_PCMSK     PCMSK2 // Pin change interrupt register
#define PINOUT_MASK ((1<<PIN_RESET)|(1<<PIN_FEED_HOLD)|(1<<PIN_CYCLE_START))

#define SDSUPPORT

// Ramps PB3 ( MISO/PCINT3 ) Digital pin 50 (MISO) ->Ramps AUX-3 pin 3
// Ramps PB2 ( MOSI/PCINT2 ) Digital pin 51 (MOSI) ->Ramps AUX-3 pin 4
// Ramps PB1 ( SCK/PCINT1 ) Digital pin 52 (SCK) -> Ramps AUX-3 pin 5
// Motor ON PB4 - 10
// Motor Dir PH6 - 9
// Coolant PH6 - 8

#define ENDSTOPPULLUPS
#ifdef ENDSTOPPULLUPS
  #define ENDSTOPPULLUP_XMAX
  #define ENDSTOPPULLUP_YMAX
  //#define ENDSTOPPULLUP_ZMAX
  #define ENDSTOPPULLUP_XMIN
  #define ENDSTOPPULLUP_YMIN
  //#define ENDSTOPPULLUP_ZMIN
#endif

#define X_STEP_PIN         54 //PF0 ( ADC0 )
#define X_DIR_PIN          55 //PF1 ( ADC1 )
#define X_ENABLE_PIN       38 //PD7 ( T0 )
#define X_MIN_PIN           3
#define X_MAX_PIN           2

#define Y_STEP_PIN         60 //PF6 ( ADC6 )
#define Y_DIR_PIN          61 //PF7 ( ADC7 )
#define Y_ENABLE_PIN       56 //PF2 ( ADC2 )
#define Y_MIN_PIN          14
#define Y_MAX_PIN          15

//#define Z_STEP_PIN         46 //PL3 ( OC5A )
//#define Z_DIR_PIN          48 //PL1 ( ICP5 )
//#define Z_ENABLE_PIN       62 //PK0 ( ADC8/PCINT16 )
//#define Z_MIN_PIN          18
//#define Z_MAX_PIN          19

#define BTN_EN1  33             // Define encoder pin A
#define BTN_EN2  31              // Define encoder pin B
#define BTN_ENC  35

#define PAUSE_PIN	42
#define START_PIN	64
#define DOOR_PIN	40

#define HARDWARE_SCREEN_PIN 53

#define TEMP_0_PIN 13
#define TEMP_SENSOR_0 5

#define SPINDLE_ENABLE_PIN 44
//#define SPINDLE_DIRECTION_PIN 9
//#define COOLANT_FLOOD_PIN 5

#define X_MOTION_STATUS_LED 6
#define Y_MOTION_STATUS_LED 5
#define SPINDLE_STATUS_LED 4
#define DOOR_STATUS_LED 11

#define BUZZER_PIN 37
