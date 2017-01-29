#include "protocol_parser.h"
#include "spindle_control.h"
#include "gcode.h"
#include "stepper.h"

// Workaround for http://gcc.gnu.org/bugzilla/show_bug.cgi?id=34734
#ifdef PROGMEM
#undef PROGMEM
#define PROGMEM __attribute__((section(".progmem.data")))
#endif

ProtocolParser::ProtocolParser() {
	reset();
}

ProtocolParser::~ProtocolParser() {

}

void ProtocolParser::reset() {
    state = PARSER_STATE_NONE;
    char_counter = 0;
	checksum = 0;
	line_checksum_c = 0;
	line_number_c = 0;
}

bool ProtocolParser::parse_input(uint8_t c) {
    if ((c == '\n') || (c == '\r')) { // End of line reached
      line[char_counter] = 0; // Set string termination character.
      if (line_checksum_c > 0) {
    	  line_checksum[line_checksum_c] = '\0';
    	  uint8_t wanted_checksum = atoi(line_checksum);
    	  if (wanted_checksum != checksum) {
    		  report_status_message(STATUS_CHECKSUM);
    		  if (line_number_c > 0) {
    			  line_number[line_number_c] = '\0';
    			  failed_line = atol(line_number);
    		  } else {
    			  failed_line = -1;
    		  }
			  reset();
			  return false;
    	  }
      }

      if (settings.extraflags & BITFLAG_GC_ECHO_ENABLE) { // Echo GC to serial
		  serial_write('(');
		  printString(line);
		  serial_write(')');
		  serial_write('\r');
		  serial_write('\n');
      }


    protocol_execute_line(line); // Line is complete. Execute it!
      reset();
    } else {
      if (state == PARSER_STATE_CHECKSUM) {
    	  if (c >= '0' && c <= '9') {
    		  line_checksum[line_checksum_c++] = c;
    		  return true;
    	  } else {
    		  state = PARSER_STATE_NONE;
    	  }
      } else if (state == PARSER_STATE_LINE_NUMBER) {
    	  if ((c >= '0' && c <= '9') || (c == '.')) {
    		  line_number[line_number_c++] = c;
    		  checksum ^= c;
    		  line[char_counter++] = c;
    		  return true;
    	  } else {
    		  state = PARSER_STATE_NONE;
    	  }
      }

      if (state == PARSER_STATE_COMMENT_TYPE_PARENTHESES || state == PARSER_STATE_COMMENT_TYPE_SEMICOLON) {
        // Throw away all comment characters
        if (c == ')') {
          // End of comment. Resume line. But, not if semicolon type comment.
          if (state == PARSER_STATE_COMMENT_TYPE_PARENTHESES) { state = PARSER_STATE_NONE; }
        }
      } else {
    	if (c == '*') {
          state = PARSER_STATE_CHECKSUM;
        } else if (c <= ' ') {
          // Throw away whitepace and control characters
          checksum ^= c;
        } else if (c == '(') {
          // Enable comments flag and ignore all characters until ')' or EOL.
          // NOTE: This doesn't follow the NIST definition exactly, but is good enough for now.
          // In the future, we could simply remove the items within the comments, but retain the
          // comment control characters, so that the g-code parser can error-check it.
          state = PARSER_STATE_COMMENT_TYPE_PARENTHESES;
        } else if (c == ';') {
          // NOTE: ';' comment to EOL is a LinuxCNC definition. Not NIST.
          state = PARSER_STATE_COMMENT_TYPE_SEMICOLON;

        // TODO: Install '%' feature
        // } else if (c == '%') {
          // Program start-end percent sign NOT SUPPORTED.
          // NOTE: This maybe installed to tell Grbl when a program is running vs manual input,
          // where, during a program, the system auto-cycle start will continue to execute
          // everything until the next '%' sign. This will help fix resuming issues with certain
          // functions that empty the planner buffer to execute its task on-time.

        } else if (char_counter >= (LINE_BUFFER_SIZE-1)) {
          // Detect line buffer overflow. Report error and reset line buffer.
          report_status_message(STATUS_OVERFLOW);
          reset();
        } else if (c >= 'a' && c <= 'z') { // Upcase lowercase
            if (char_counter == 0 && (c == 'N' || c == 'n')) {
              state = PARSER_STATE_LINE_NUMBER;
            }

          checksum ^= c;
          if (char_counter > 0 && line[0] == 'B') {
        	  // Leave binary data unchanged
        	  line[char_counter++] = c;
          } else {
        	  line[char_counter++] = c-'a'+'A';
          }
        } else {
            if (char_counter == 0 && (c == 'N' || c == 'n')) {
              state = PARSER_STATE_LINE_NUMBER;
            }

          checksum ^= c;
          line[char_counter++] = c;
        }
      }
    }
    return true;
}

size_t ProtocolParser::write(uint8_t c) {
	parse_input(c);
	return 1;
}

ProtocolParserFile::ProtocolParserFile(CancelNotifier *cancelNotifier) : cancelNotifier_(cancelNotifier) {

}

ProtocolParserFile::~ProtocolParserFile() {
	delete cancelNotifier_;
}

void ProtocolParserFile::setFileName(const char *fileName) {
	const char *smallFileName = strrchr(fileName, '/');
	if (smallFileName) smallFileName++;
	else smallFileName = fileName;
	fileName_ = ScreenMenuString(smallFileName, ScreenMenuStringBuffer::CopyString);
}

ProtocolParserGCFile::ProtocolParserGCFile(CancelNotifier *cancelNotifier, ProtocolParser *protocolParser) :
	ProtocolParserFile(cancelNotifier),
	protocolParser_(protocolParser), fileSize_(0), readSize_(0), startTime_(0) {

}

ProtocolParserGCFile::~ProtocolParserGCFile() {
}

bool ProtocolParserGCFile::init_file(const char *fileName) {
	setFileName(fileName);
	if (!cardReader_.openFile(fileName)) return false;

	fileSize_ = cardReader_.getCurrentFile().fileSize();
	startTime_ = poller_seconds;
	return true;
}

bool ProtocolParserGCFile::parse_file_input() {
	int buffLen = cardReader_.getCurrentFile().read(buff_, LINE_BUFFER_SIZE);
	if (buffLen <= 0) {
		if (protocolParser_->get_char_counter() > 0) {
			// Send any remaining input
			protocolParser_->parse_input('\n');
		}
		return false;
	}
	readSize_+= buffLen;

	for (int i=0; i<buffLen; i++) {
		protocolParser_->parse_input(buff_[i]);
	}
	return true;
}

unsigned int ProtocolParserGCFile::get_parsing_time() {
	return poller_seconds - startTime_;
}

float ProtocolParserGCFile::get_read_percent() {
	return ((float) readSize_) / ((float) fileSize_);
}

typedef struct {
    unsigned int bfType;                 /* Magic identifier            */
    unsigned long size;                       /* File size in bytes          */
    unsigned long reserved;
    unsigned long bfOffBits;                     /* Offset to image data, bytes */
} BITMAPFILEHEADER;

typedef struct {
    unsigned long size;               /* Header size in bytes      */
    long width,height;                /* Width and height of image */
    unsigned int planes;       /* Number of colour planes   */
    unsigned int bits;         /* Bits per pixel            */
    unsigned long compression;        /* Compression type          */
    unsigned long imagesize;          /* Image size in bytes       */
    long xresolution,yresolution;     /* Pixels per meter          */
    unsigned long ncolours;           /* Number of colours         */
    unsigned long importantcolours;   /* Important colours         */
} BITMAPINFOHEADER;

void ImageParser::wait_for_completion() {
	while (sys.state != STATE_IDLE) {
		protocol_execute_realtime();
	}
	delay_ms(100);
}

bool ImageParser::supported_format(const char *fileName) {
	if (strstr(fileName, "__SCALE__") ||
		strstr(fileName, "__SSCALE__")) {
		return true;
	}
	if (strstr(fileName, ".BMP")) {
		return true;
	}
	return false;
}

ImageParser *ImageParser::parser_factory(const char *fileName, CancelNotifier *cancelNotifier) {
	if (strstr(fileName, "__SCALE__")) {
		return new ImageParserStripe(cancelNotifier, false);
	}
	if (strstr(fileName, "__SSCALE__")) {
		return new ImageParserStripe(cancelNotifier, true);
	}
	if (strstr(fileName, ".BMP")) {
		return new ImageParserBMP(cancelNotifier);
	}
	return 0;
}

ImageParserBMP::ImageParserBMP(CancelNotifier *cancelNotifier) :
	ImageParser(cancelNotifier),
	fileSize_(0), readSize_(0),
	width_(0), height_(0), buff_(0), rgb_(0) {

}

ImageParserBMP::~ImageParserBMP() {
	delete buff_;
}

bool ImageParserBMP::read_header() {
	BITMAPFILEHEADER bitmapFileHeader;
	if (cardReader_.getCurrentFile().read(&bitmapFileHeader, sizeof(BITMAPFILEHEADER)) != sizeof(BITMAPFILEHEADER)) {
		printPgmString(PSTR("Failed to read bitmap file header.\r\n"));
		return false;
	}
	if (bitmapFileHeader.bfType !=0x4D42) {
		printPgmString(PSTR("File is not a valid bitmap.\r\n"));
		return false;
	}

	BITMAPINFOHEADER bitmapInfoHeader;
	if (cardReader_.getCurrentFile().read(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER)) != sizeof(BITMAPINFOHEADER)) {
		printPgmString(PSTR("Failed to read bitmap info header.\r\n"));
		return false;
	}

	if (bitmapInfoHeader.bits != 24 || bitmapInfoHeader.compression != 0) {
		printPgmString(PSTR("Invalid bitmap format, bitmaps must be uncompressed 24 bit color.\r\n"));
		return false;
	}

	if (!cardReader_.getCurrentFile().seekSet(bitmapFileHeader.bfOffBits)) {
		printPgmString(PSTR("Failed seeking bitmap header.\r\n"));
		return false;
	}

	width_ = bitmapInfoHeader.width;
	height_ = bitmapInfoHeader.height;
	return true;
}

bool ImageParserBMP::loadFile(const char *fileName) {
	if (!cardReader_.openFile(fileName)) return false;

	fileSize_ = cardReader_.getCurrentFile().fileSize();
	if  (!read_header()) return false;

	bufferWidth_ = 3 * 32;
	bytesWidth_ = width_ * 3;
	if (bytesWidth_ % 4 != 0) bytesWidth_ += 4 - (bytesWidth_ % 4);
	buff_ = new uint8_t[bufferWidth_];
	rgb_ = buff_;

	return true;
}

bool ImageParserBMP::startRow() {
	bytesLeftInBuffer_ = 0;
	bytesLeftOnRow_ = bytesWidth_;
	return true;
}

uint16_t ImageParserBMP::nextColumn() {
	if (bytesLeftInBuffer_ <= 0) {
		int readAmount = bytesLeftOnRow_;
		if (readAmount > bufferWidth_) readAmount = bufferWidth_;
		bytesLeftOnRow_ -= readAmount;
		bytesLeftInBuffer_ += readAmount;
		readSize_ += readAmount;
		rgb_ = buff_;

		if (cardReader_.getCurrentFile().read(buff_, readAmount) != readAmount) {
			return 0;
		}
	} else {
		rgb_ += 3;
	}
	uint16_t result = (rgb_[0] + rgb_[1] + rgb_[2]) * 4 / 3;
	bytesLeftInBuffer_ -= 3;
	return result;
}

bool ImageParserBMP::endRow() {
	if (bytesLeftOnRow_ > 0) {
		readSize_ += bytesLeftOnRow_;
		if (cardReader_.getCurrentFile().read(buff_, bytesLeftOnRow_) != bytesLeftOnRow_) {
			return false;
		}
	}
	return true;
}

bool ImageParserStripe::startRow() {
	x_ = 0;
	return true;
}

uint16_t ImageParserStripe::nextColumn() {
	uint16_t val = 1000;
	if (y_ < 25) {
		if ((x_ <= 5) || ((x_ - 2) % (getWidth() / 10) <= 5) || (x_ >= (getWidth() - 5))) {
			val = 0;
		}
	} else {
		val = 1000 - (x_ * (1000 / getWidth()));
	}

	x_++;
	readSize_++;
	return val;
}

bool ImageParserStripe::endRow() {
	y_++;
	return true;
}

ProtocolParserEngraveFileBounds::ProtocolParserEngraveFileBounds(CancelNotifier *cancelNotifier,
		ProtocolParser *protocolParser, ImageParser *imageParser) :
	ProtocolParserFile(cancelNotifier),
	protocolParser_(protocolParser), imageParser_(imageParser) {
}

ProtocolParserEngraveFileBounds::~ProtocolParserEngraveFileBounds() {
	delete imageParser_;
}

bool ProtocolParserEngraveFileBounds::init_file(const char *fileName) {
	setFileName(fileName);
	return imageParser_->loadFile(fileName);
}

bool ProtocolParserEngraveFileBounds::parse_file_input() {
	float width = ((float) imageParser_->getWidth()) * DEFAULT_BITMAP_SCALE;
	float height = ((float) imageParser_->getHeight()) * DEFAULT_BITMAP_SCALE;

	protocolParser_->printPgmString(PSTR("M3S"));
	protocolParser_->print(settings.jog_power);
	protocolParser_->printPgmString(PSTR("\r\n"));
	protocolParser_->printPgmString(PSTR("G1X0Y0F"));
	protocolParser_->print(settings.bitmap_return_feed_rate, 0);
	protocolParser_->printPgmString(PSTR("1800\r\nG1X"));
	protocolParser_->print(width, 2);
	protocolParser_->printPgmString(PSTR("\r\nG1Y"));
	protocolParser_->print(height, 2);
	protocolParser_->printPgmString(PSTR("\r\nG1X0\r\n"));
	protocolParser_->printPgmString(PSTR("G1X0Y0\r\n"));
	protocolParser_->printPgmString(PSTR("M3S0\r\n"));

	ImageParser::wait_for_completion();
	return false;
}

ProtocolParserEngraveFileRun::ProtocolParserEngraveFileRun(CancelNotifier *cancelNotifier,
		ProtocolParser *protocolParser, ImageParser *imageParser) :
	ProtocolParserFile(cancelNotifier),
	protocolParser_(protocolParser), imageParser_(imageParser),
	percentage_(0.0f), startTime_(0), totalTime_(0), timeLeft_(0) {
}

ProtocolParserEngraveFileRun::~ProtocolParserEngraveFileRun() {
	delete imageParser_;
}

bool ProtocolParserEngraveFileRun::init_file(const char *fileName) {
	setFileName(fileName);
	return imageParser_->loadFile(fileName);
}

void ProtocolParserEngraveFileRun::send_line(float *target, float feed_rate, uint16_t spindle_speed, float acceleration) {
#ifdef USE_LINE_NUMBERS
	mc_line_with_speed(target, feed_rate, false, 0, spindle_speed, acceleration);
#else
	mc_line_with_speed(target, feed_rate, false, spindle_speed, acceleration);
#endif
}

bool ProtocolParserEngraveFileRun::parse_file_input() {
	imageParser_->start();

	delay_ms(250);

	// Update positions
    plan_sync_position();
    gc_sync_position();
	plan_reset();
    st_reset();

	// Use g-code to position the head, and to set the feed rate for the display
    protocolParser_->printPgmString(PSTR("M3S0\r\n"));
	protocolParser_->printPgmString(PSTR("G1X0Y0F"));
	protocolParser_->print(settings.bitmap_return_feed_rate, 0);
	protocolParser_->printPgmString(PSTR("\r\nG1X-10\r\n"));
	protocolParser_->printPgmString(PSTR("G1X0Y0F"));
	protocolParser_->print(settings.bitmap_feed_rate, 0);
	protocolParser_->printPgmString(PSTR("\r\n"));
	ImageParser::wait_for_completion();

	// Update positions
    plan_sync_position();
    gc_sync_position();
	plan_reset();
    st_reset();

	// Paper 10-14.5% @1200?
	// Wood 7%-40% @ 1200
	// Wood 10%-85% @ 1800 0.09 (House)
	// Wood 10%-80% @ 1800 0.09 (LightHouse 1)

	float top_power_f = (((float) PWM_MAX_VALUE * ((float) settings.bitmap_high_power)) / 100.0f);
	float bottom_power_f = (((float) PWM_MAX_VALUE * ((float) settings.bitmap_low_power)) / 100.0f);
	float diff_power_f = top_power_f - bottom_power_f;

	startTime_ = poller_seconds;
	unsigned int start_ticks = poller_count;

	float target[N_AXIS];
	long width = imageParser_->getWidth();
	long height = imageParser_->getHeight();
	long x, y;
	float bitmap_acceleration = 10000.0f * 60 * 60;
	for (y=0; y<height; y++) {
		if (cancelNotifier_->isCanceled()) break;

		target[Y_AXIS] = ((float) y) * DEFAULT_BITMAP_SCALE;
		target[X_AXIS] = -10.0f;
		send_line(target, settings.bitmap_return_feed_rate, 1, 0.0f);
		target[X_AXIS] = -5.0f;
		send_line(target, settings.bitmap_feed_rate, 1, 0.0f);
		target[X_AXIS] = -DEFAULT_BITMAP_SCALE;
		send_line(target, settings.bitmap_feed_rate, 1, bitmap_acceleration);

		if (!imageParser_->startRow()) {
			return false;
		}

		for (x=0; x<width; x++) {
			uint16_t color = imageParser_->nextColumn();
			if (cancelNotifier_->isCanceled()) break;
			if (color == ((uint16_t) -1)) {
				cancelNotifier_->cancel();
				break;
			}

			percentage_ = ((float) imageParser_->getReadSize()) / ((float) imageParser_->getFileSize());

			float power = 1000.0f - ((float) color);
			if (power == 0.0f) {
				power = 1.0f;
			} else {
				power = ((power / 1000.0f) * diff_power_f) + bottom_power_f;
				if (power > (float) PWM_MAX_VALUE) power = (float) PWM_MAX_VALUE;
				else if (power < 1.0f) power = 1.0f;
			}

			target[X_AXIS] = ((float) x) * DEFAULT_BITMAP_SCALE;

			uint16_t p = (uint16_t) power;
			send_line(target, settings.bitmap_feed_rate, p, bitmap_acceleration);
			protocol_parse_serial();
			protocol_execute_realtime();
		}
		if (cancelNotifier_->isCanceled()) break;

		/*
		printString("-----\r\n");
		printFloat((float) imageParser_->getReadSize(), 2);
		printString("\r\n");
		printFloat((float) imageParser_->getFileSize(), 2);
		printString("\r\n");
		printFloat(percentage_, 2);
		printString("\r\n");
		*/

		if (!imageParser_->endRow()) {
			return false;
		}

		/*for (;x<width+100; x++) {
			target[X_AXIS] = ((float) x) * DEFAULT_BITMAP_SCALE;
			mc_line_with_speed(target, settings.bitmap_feed_rate, false, 1, bitmap_acceleration);
		}*/

		target[X_AXIS] = ((float) (width)) * DEFAULT_BITMAP_SCALE + 10.0f;
		send_line(target, settings.bitmap_return_feed_rate, 1, 0.0f);

		if (y <= 10) {
			unsigned int ticks_so_far = poller_count - start_ticks;
			totalTime_ = (unsigned int) ((((float) ticks_so_far) / percentage_) / 50.0f);
		}
	}

	// Move back to start position
	ImageParser::wait_for_completion();
    plan_sync_position();
    gc_sync_position();
	protocolParser_->printPgmString(PSTR("G1X0Y0\r\n"));

	ImageParser::wait_for_completion();
	ImageParser::wait_for_completion();
	return false;
}

unsigned int ProtocolParserEngraveFileRun::get_parsing_time() {
	if (totalTime_ > 0) {
		unsigned int time_taken_so_far = poller_seconds - startTime_;
		if (time_taken_so_far < totalTime_) {
			timeLeft_ = totalTime_ - time_taken_so_far;
		} else {
			timeLeft_ = 0;
		}
	}
	return timeLeft_;
}
