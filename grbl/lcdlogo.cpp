#include "lcd7920.h"

// http://www.digole.com/tools/PicturetoC_Hex_converter.php

static const prog_uint8_t lcd_logo_data[] PROGMEM  = {
		0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xfc,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xfc,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x1f,0xff,0xfc,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xff,0xff,0xfc,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xff,0xff,0xff,0xfe,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xff,0xff,0xfe,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xfc,0x3e,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0x00,0x3e,0x00,0x00,0x00
		,0x00,0x00,0x01,0xc0,0x00,0xf0,0x00,0x03,0xff,0xff,0x80,0x00,0x3e,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x03,0xff,0xe0,0x00,0x00,0x3e,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x03,0xf8,0x00,0x00,0x00,0x1c,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x03,0xf0,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x01,0xe0,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x1f,0xff,0x81,0xf9,0xff,0x00,0x03,0xfe,0x00,0x03,0xff,0x00,0x00,0x00
		,0x00,0x00,0x3f,0xff,0xc1,0xfb,0xff,0x80,0x0f,0xff,0x80,0x0f,0xff,0xc0,0x00,0x00
		,0x00,0x00,0x3f,0xff,0xc1,0xff,0xff,0xc0,0x1f,0xff,0xc0,0x1f,0xff,0xe0,0x00,0x00
		,0x00,0x00,0x3f,0xff,0xc1,0xff,0xff,0xe0,0x3f,0xff,0xe0,0x3f,0xff,0xf0,0x00,0x00
		,0x00,0x00,0x1f,0xff,0x81,0xff,0xff,0xf0,0x3f,0xff,0xf0,0x7f,0xff,0xf0,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xfe,0x07,0xf0,0x7f,0x07,0xf0,0x7f,0x03,0xf8,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xfc,0x03,0xf0,0x7e,0x01,0xf8,0xfe,0x01,0xf8,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x03,0xf0,0xfc,0x01,0xf8,0xfc,0x79,0xf8,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf8,0xff,0x80,0xf8,0xf8,0xfc,0xfc,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf8,0xff,0xf8,0xf8,0xf8,0xfc,0xfc,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf9,0xff,0xff,0xf8,0xf8,0x1c,0xfc,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf9,0xff,0xff,0xf8,0xf8,0x18,0x7e,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf9,0xff,0xff,0xf8,0xfc,0x00,0x7e,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf9,0xf8,0x7f,0xf8,0xfc,0x00,0x7e,0x00,0x00
		,0x00,0x00,0x03,0xe0,0x01,0xf8,0x01,0xf9,0xf8,0x0f,0xf8,0xfc,0x00,0x7c,0x00,0x00
		,0x00,0x00,0x03,0xf0,0x01,0xf8,0x01,0xf8,0xf8,0x00,0xf0,0x7e,0x00,0xfc,0x00,0x00
		,0x00,0x00,0x03,0xf0,0x01,0xf8,0x01,0xf8,0xfc,0x00,0x00,0x7e,0x01,0xfc,0x00,0x00
		,0x00,0x00,0x03,0xf8,0x01,0xf8,0x01,0xf8,0xff,0x00,0x00,0x7f,0x83,0xf8,0x00,0x00
		,0x00,0x00,0x03,0xff,0xc1,0xf8,0x01,0xf8,0x7f,0xff,0x80,0x3f,0xff,0xf8,0x00,0x00
		,0x00,0x00,0x01,0xff,0xe1,0xf8,0x01,0xf8,0x3f,0xff,0xc0,0x1f,0xff,0xf0,0x00,0x00
		,0x00,0x00,0x00,0xff,0xe1,0xf8,0x01,0xf8,0x1f,0xff,0xc0,0x0f,0xff,0xe0,0x00,0x00
		,0x00,0x00,0x00,0x7f,0xe1,0xf8,0x01,0xf8,0x0f,0xff,0x80,0x07,0xff,0x80,0x00,0x00
		,0x00,0x00,0x00,0x1f,0xc0,0xf0,0x00,0xf0,0x03,0xff,0x00,0x03,0xff,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xe0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xf0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xf0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0x00,0x00,0x00,0x07,0xf0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0x80,0x00,0x00,0x0f,0xf0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xc0,0x00,0x00,0x3f,0xe0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x0f,0xc0,0x00,0x03,0xff,0xc0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xf0,0x00,0x7f,0xff,0xc0,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xff,0xff,0xff,0xff,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xfc,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0xff,0xff,0xff,0xf0,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0xff,0xff,0x80,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x7f,0xff,0xf8,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x07,0xfe,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0xc0,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0xc0,0x00,0x30,0x00,0x00,0x00,0x00,0x00,0x00
		,0x00,0x00,0x00,0x00,0x00,0x00,0x1e,0xf1,0xc7,0x37,0x8f,0x38,0xcf,0x07,0x1c,0xd2
		,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0xdb,0x6f,0xb0,0xdc,0x6d,0xec,0x0f,0xbe,0xff
		,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0xdb,0xed,0xb3,0xce,0x7d,0x8e,0x0c,0x36,0xdb
		,0x00,0x00,0x00,0x00,0x00,0x00,0x0c,0xdb,0x0f,0xb6,0xc7,0x61,0x87,0x6f,0xbe,0xdb
		,0x00,0x00,0x00,0x00,0x00,0x00,0x0e,0xd9,0xc7,0x37,0x9e,0x39,0x9e,0x67,0x1c,0xdb
};

extern const LcdImage lcd_logo =
{
	lcd_logo_data,   // data
	64,          // height in pixels
	128           // width in pixels
};
