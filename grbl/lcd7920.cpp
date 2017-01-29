// Driver for 128x634 graphical LCD with ST7920 controller
// D Crocker, Escher Technologies Ltd.

#include "lcd7920.h"
#include "nuts_bolts.h"
#include "SdFatConfig.h"
#include "Sd2PinMap.h"
#include <avr/interrupt.h>

// LCD basic instructions. These all take 72us to execute except LcdDisplayClear, which takes 1.6ms
const uint8_t LcdDisplayClear = 0x01;
const uint8_t LcdHome = 0x02;
const uint8_t LcdEntryModeSet = 0x06;       // move cursor right and indcement address when writing data
const uint8_t LcdDisplayOff = 0x08;
const uint8_t LcdDisplayOn = 0x0C;          // add 0x02 for cursor on and/or 0x01 for cursor blink on
const uint8_t LcdFunctionSetBasicAlpha = 0x20;
const uint8_t LcdFunctionSetBasicGraphic = 0x22;
const uint8_t LcdFunctionSetExtendedAlpha = 0x24;
const uint8_t LcdFunctionSetExtendedGraphic = 0x26;
const uint8_t LcdSetDdramAddress = 0x80;    // add the address we want to set

// LCD extended instructions
const uint8_t LcdSetGdramAddress = 0x80;

const unsigned int LcdCommandDelayMicros = 72 - 24; // 72us required, less 24us time to send the command @ 1MHz
const unsigned int LcdDataDelayMicros = 10;         // Delay between sending data bytes

const unsigned int numRows = 64;
const unsigned int numCols = 128;

static Lcd7920 *this_lcd = 0;

static inline __attribute__((always_inline))
void sendTimerLcd(uint8_t data1, uint8_t data2)
{
    SPDR = data1;
    while ((SPSR & (1 << SPIF)) == 0) { }
    SPDR = data2 & 0xF0;
    while ((SPSR & (1 << SPIF)) == 0) { }
    SPDR = data2 << 4;
    while ((SPSR & (1 << SPIF)) == 0) { }
}

#define TIMER_STATE_NEW_ROW 1
#define TIMER_STATE_WAIT 2
#define TIMER_STATE_NORMAL_1 3
#define TIMER_STATE_NORMAL_2 4

static uint8_t current_row = 0;
static uint8_t current_col = 0;
static uint8_t timer_state = TIMER_STATE_NORMAL_1;
static uint8_t *ptr = 0;

Lcd7920::Lcd7920() : textInverted(false), currentFont(0)
{
	this_lcd = this;
}

size_t Lcd7920::write(uint8_t ch)
{
    if (currentFont == 0)
    { 
      return 0; 
    }
    const uint8_t startChar = pgm_read_byte_near(&(currentFont->startCharacter));
    const uint8_t endChar = pgm_read_byte_near(&(currentFont->endCharacter));
    
    if (ch < startChar || ch > endChar)
    {
      return 0;
    }
    
    const uint8_t fontWidth = pgm_read_byte_near(&(currentFont->width));
    const uint8_t fontHeight = pgm_read_byte_near(&(currentFont->height));
    const uint8_t bytesPerColumn = (fontHeight + 7)/8;
    const uint8_t bytesPerChar = (bytesPerColumn * fontWidth) + 1;
    const prog_uint8_t PROGMEM *fontPtr = (const prog_uint8_t*)pgm_read_word_near(&(currentFont->ptr)) + (bytesPerChar * (ch - startChar));
    uint16_t cmask = (1 << fontHeight) - 1;
    
    uint8_t nCols = pgm_read_byte_near(fontPtr++);

    // Decide whether to add a space column first (auto-kerning)
    // We don't add a space column before a space character.
    // We add a space column after a space character if we would have added one between the preceding and following characters.
    if (column != numCols)
    {
      uint16_t thisCharColData = pgm_read_word_near(fontPtr) & cmask;    // atmega328p is little-endian
      if (thisCharColData == 0)  // for characters with deliberate space row at the start, e.g. decimal point
      {
        thisCharColData = pgm_read_word_near(fontPtr + 2) & cmask;
      }
      bool wantSpace = ((thisCharColData | (thisCharColData << 1)) & (lastCharColData | (lastCharColData << 1))) != 0;
      if (wantSpace)
      {
        // Add space after character
        uint8_t mask = 0x80 >> (column & 7);
        uint8_t *p = image + ((row * (numCols/8)) + (column/8));
        for (uint8_t i = 0; i < fontHeight && p < (image + sizeof(image)); ++i)
        {
          if (textInverted)
          {
            *p |= mask;
          }
          else
          {
            *p &= ~mask;
          }
          p += (numCols/8);
        }
        ++column;
      }      
    }
    
    while (nCols != 0 && column < numCols)
    {
      uint16_t colData = pgm_read_word_near(fontPtr);
      fontPtr += bytesPerColumn;
      if (colData != 0)
      {
        lastCharColData = colData & cmask;
      }
      uint8_t mask1 = 0x80 >> (column & 7);
      uint8_t mask2 = ~mask1;
      uint8_t *p = image + ((row * (numCols/8)) + (column/8));
      const uint16_t setPixelVal = (textInverted) ? 0 : 1;
      for (uint8_t i = 0; i < fontHeight && p < (image + sizeof(image)); ++i)
      {
        if ((colData & 1) == setPixelVal)
        {
          *p |= mask1;      // set pixel
        }
        else
        {
          *p &= mask2;     // clear pixel
        }
        colData >>= 1;
        p += (numCols/8);
      }
      --nCols;
      ++column;
    }
    
  return 1;
}

// Select normal or inverted text (only works in graphics mode)
void Lcd7920::textInvert(bool b)
{
  if (b != textInverted)
  {
    textInverted = b;
    lastCharColData = 0xFFFF;    // always need space between inverted and non-inverted text
  }
}

// NB - if using SPI then the SS pin must be set to be an output before calling this!
void Lcd7920::begin()
{
	stopTimers();

	// Set up the SPI interface for talking to the LCD. We have to set MOSI, SCLK and SS to outputs, then enable SPI.
	fastDigitalWrite(SCK_PIN, LOW);
	fastDigitalWrite(MOSI_PIN, LOW);
	pinMode(SCK_PIN, OUTPUT);
	pinMode(MOSI_PIN, OUTPUT);

	pinMode(SS_PIN, OUTPUT);   // must do this before we use the lcd in SPI mode
	fastDigitalWrite(SS_PIN, HIGH);

	SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);  // enable SPI, master mode, clock low when idle, data sampled on rising edge, clock = f/16 (= 1MHz), send MSB first
	//SPSR = (1 << SPI2X);  // double the speed to 2MHz (optional)

	// Setup
	sendTimerLcd(0xF8, LcdFunctionSetExtendedGraphic);
	delay_us(LcdCommandDelayMicros);
	clear();  // clear gfx ram
	flush();
	setCursor(0, 0);
	sendTimerLcd(0xF8, LcdDisplayOn);
	delay_us(LcdCommandDelayMicros);
	currentFont = 0;
	textInverted = false;
}

void Lcd7920::stopTimers() {
	TCCR4A = 0;
	TCCR4B = 0;
	TCCR4C = 0;
	TCNT4 = 0;
	TIMSK4 = 0;
}

void Lcd7920::startTimers()
{
	ptr=image;
	current_row = 0;
	current_col = 0;
	timer_state = TIMER_STATE_NORMAL_1;

	/*
	TCCR4A = 0;
	TCCR4A |= (1 << WGM40);

	OCR4A = 65535;
	OCR4B = 819; // compare match register 16MHz/256/2Hz
	TCCR4B |= (1 << WGM12); // CTC mode
	TCCR4B |= (1 << CS41); // 8 prescaler
	TIMSK4 |= (1 << OCIE4B); // enable timer compare interrupt
	*/

	TCCR4A = 0;
	//TCCR4A |= (1 << WGM40);

	OCR4A = 650;
	OCR4B = 128; // compare match register 16MHz/256/2Hz
	TCCR4B |= (1 << WGM12); // CTC mode
	TCCR4B |= (1 << CS41); // 8 prescaler
	TIMSK4 |= (1 << OCIE4B); // enable timer compare interrupt
}

void Lcd7920::setFont(const PROGMEM LcdFont *newFont)
{
  currentFont = newFont;
}

void Lcd7920::clear()
{
  memset(image, 0, sizeof(image));
  setCursor(0, 0);
  textInverted = false;
}

// Draw a line using the Bresenham Algorithm (thanks Wikipedia)
void Lcd7920::line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, PixelMode mode)
{
  int dx = (x1 >= x0) ? x1 - x0 : x0 - x1;
  int dy = (y1 >= y0) ? y1 - y0 : y0 - y1;
  int sx = (x0 < x1) ? 1 : -1;
  int sy = (y0 < y1) ? 1 : -1;
  int err = dx - dy;
 
  for (;;)
  {
    setPixel(x0, y0, mode);
    if (x0 == x1 && y0 == y1) break;
    int e2 = err + err;
    if (e2 > -dy)
    { 
      err -= dy;
      x0 += sx;
    }
    if (e2 < dx)
    { 
      err += dx;
      y0 += sy;
    }
  }
}

// Draw a circle using the Bresenham Algorithm (thanks Wikipedia)
void Lcd7920::circle(uint8_t x0, uint8_t y0, uint8_t radius, PixelMode mode)
{
  int f = 1 - (int)radius;
  int ddF_x = 1;
  int ddF_y = -2 * (int)radius;
  int x = 0;
  int y = radius;
 
  setPixel(x0, y0 + radius, mode);
  setPixel(x0, y0 - radius, mode);
  setPixel(x0 + radius, y0, mode);
  setPixel(x0 - radius, y0, mode);
 
  while(x < y)
  {
    // keep ddF_x == 2 * x + 1;
    // keep ddF_y == -2 * y;
    // keep f == x*x + y*y - radius*radius + 2*x - y + 1;
    if(f >= 0) 
    {
      y--;
      ddF_y += 2;
      f += ddF_y;
    }
    x++;
    ddF_x += 2;
    f += ddF_x;    
    setPixel(x0 + x, y0 + y, mode);
    setPixel(x0 - x, y0 + y, mode);
    setPixel(x0 + x, y0 - y, mode);
    setPixel(x0 - x, y0 - y, mode);
    setPixel(x0 + y, y0 + x, mode);
    setPixel(x0 - y, y0 + x, mode);
    setPixel(x0 + y, y0 - x, mode);
    setPixel(x0 - y, y0 - x, mode);
  }
}

// Draw a bitmap
void Lcd7920::bitmap(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, const prog_uint8_t data[])
{
  for (uint8_t r = 0; r < height && r + y0 < numRows; ++r)
  {
    uint8_t *p = image + (((r + y0) * (numCols/8)) + (x0/8));
    uint16_t bitMapOffset = r * (width/8);
    for (uint8_t c = 0; c < (width/8) && c + (x0/8) < numCols/8; ++c)
    {
      *p++ = pgm_read_byte_near(bitMapOffset++);
    }
  }
}

void Lcd7920::bitmap_h(uint8_t x0, uint8_t y0, uint8_t width, uint8_t height, const prog_uint8_t data[])
{
	for (uint8_t y=0; y<height; ++y) {
		for (uint8_t x=0; x<width/8; ++x, ++data) {
			uint8_t val = pgm_read_byte_near(data);
			for (uint8_t a=0; a<8; a++) {
				if (val & (1 << a)) {
					setPixel(x0 + x*8 + (7-a), y0 + y, PixelSet);
				}
			}
		}
	}
}

ISR(TIMER4_COMPB_vect)
{
	switch (timer_state) {
	case TIMER_STATE_NEW_ROW:
		sei();
		sendTimerLcd(0xF8, (LcdSetGdramAddress | 0 | ((current_row & 32) >> 2)));
		timer_state = TIMER_STATE_WAIT;
		return;
	case TIMER_STATE_WAIT:
		timer_state = TIMER_STATE_NORMAL_1;
		return;
	case TIMER_STATE_NORMAL_1:
		sei();
		if (current_col >= 8) {
			current_col = 0;
			++current_row;
			if (current_row >= numRows) {
				current_row = 0;
				ptr = this_lcd->image;
			}
			sendTimerLcd(0xF8, (LcdSetGdramAddress | (current_row & 31)));
			timer_state = TIMER_STATE_NEW_ROW;
			return;
		}

		sendTimerLcd(0xFA, *ptr++);
		timer_state = TIMER_STATE_NORMAL_2;
		return;
	case TIMER_STATE_NORMAL_2:
		sei();
		sendTimerLcd(0xFA, *ptr++);
		++current_col;
		timer_state = TIMER_STATE_NORMAL_1;
		return;
	}
}

void Lcd7920::setGraphicsAddress(unsigned int r, unsigned int c)
{
	sendTimerLcd(0xF8, LcdSetGdramAddress | (r & 31));
	sendTimerLcd(0xF8, LcdSetGdramAddress | c | ((r & 32) >> 2));
	delay_us(LcdCommandDelayMicros);    // we definitely need this one
}

void Lcd7920::flush()
{
	uint8_t i;
    uint8_t *ptr = image;
    for (uint8_t r = 0; r < numRows; ++r)
    {
      setGraphicsAddress(r, 0);
      for (i = 0; i < 8; ++i)
      {
    	  sendTimerLcd(0xFA, *ptr++);
    	  sendTimerLcd(0xFA, *ptr++);
    	  delay_us(LcdDataDelayMicros);
      }
    }
}

// Set the cursor position. We can only set alternate columns. The row addressing is rather odd.
void Lcd7920::setCursor(uint8_t c, uint8_t r)
{
    row = r % numRows;
    column = c % numCols;
    lastCharColData = 0u;    // flag that we just set the cursor position, so no space before next character
}

void Lcd7920::getCursor(uint8_t &c, uint8_t &r)
{
	c = column;
	r = row;
}

void Lcd7920::setPixel(uint8_t x, uint8_t y, PixelMode mode)
{
  if (y < numRows && x < numCols)
  {
    uint8_t *p = image + ((y * (numCols/8)) + (x/8));
    uint8_t mask = 0x80u >> (x%8);
    switch(mode)
    {
      case PixelClear:
        *p &= ~mask;
        break;
      case PixelSet:
        *p |= mask;
        break;
      case PixelFlip:
        *p ^= mask;
        break;
    }
  }
}

bool Lcd7920::readPixel(uint8_t x, uint8_t y)
{
  if (y < numRows && x < numCols)
  {
    uint8_t *p = image + ((y * (numCols/8)) + (x/8));
    return (*p & (0x80u >> (x%8))) != 0;
  }
  return false;
}

