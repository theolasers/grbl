#ifndef PROTOCOL_PARSER_H_
#define PROTOCOL_PARSER_H_

#include <stdbool.h>
#include <stdint.h>

#include "screenutil.h"
#include "protocol.h"
#include "screenmenu.h"
#include "sdcardreader.h"

// Define different comment types for pre-parsing.
#define PARSER_STATE_NONE 0
#define PARSER_STATE_COMMENT_TYPE_PARENTHESES 1
#define PARSER_STATE_COMMENT_TYPE_SEMICOLON 2
#define PARSER_STATE_CHECKSUM 3
#define PARSER_STATE_LINE_NUMBER 4

// NOTE: While comment, spaces, and block delete(if supported) handling should technically
// be done in the g-code parser, doing it here helps compress the incoming data into Grbl's
// line buffer, which is limited in size. The g-code standard actually states a line can't
// exceed 256 characters, but the Arduino Uno does not have the memory space for this.
// With a better processor, it would be very easy to pull this initial parsing out as a
// seperate task to be shared by the g-code parser and Grbl's system commands.
class ProtocolParser : public PrintExt {
public:
	ProtocolParser();
	virtual ~ProtocolParser();

	bool parse_input(uint8_t c);
	virtual size_t write(uint8_t c);

	uint8_t get_char_counter() { return char_counter; }
	uint32_t get_failed_line() { return failed_line; }

protected:
	char line[LINE_BUFFER_SIZE]; // Line to be executed. Zero-terminated.
	char line_checksum[5], line_number[10];
	uint8_t state, line_checksum_c, line_number_c;
	uint8_t char_counter;
	uint8_t checksum;
	uint32_t failed_line;

private:
	void reset();
};

class CancelNotifier {
public:
	CancelNotifier() : canceled_(false) {}

	bool isCanceled() { return canceled_; }
	void cancel() { canceled_ = true; }

private:
	bool canceled_;
};

class ProtocolParserFile {
public:
	ProtocolParserFile(CancelNotifier *cancelNotifier);
	virtual ~ProtocolParserFile();

	virtual bool init_file(const char *fileName) = 0;
	virtual bool parse_file_input() = 0;
	virtual float get_read_percent() = 0;
	virtual unsigned int get_parsing_time() = 0;

	const char *getFileName() { return fileName_.getString(); }
	void setFileName(const char *fileName);

	void cancel() { cancelNotifier_->cancel(); }

protected:
	CancelNotifier *cancelNotifier_;
private:
	ScreenMenuString fileName_;
};

class ProtocolParserGCFile : public ProtocolParserFile {
public:
	ProtocolParserGCFile(CancelNotifier *cancelNotifier, ProtocolParser *protocolParser);
	virtual ~ProtocolParserGCFile();

	virtual bool init_file(const char *fileName);
	virtual bool parse_file_input();
	virtual float get_read_percent();
	virtual unsigned int get_parsing_time();

private:
	SDCardReader cardReader_;
	ProtocolParser *protocolParser_;
	char buff_[LINE_BUFFER_SIZE];
	uint32_t fileSize_, readSize_;
	unsigned int startTime_;

	void read_next();
};

class ImageParser {
public:
	ImageParser(CancelNotifier *cancelNotifier) : cancelNotifier_(cancelNotifier) {}
	virtual ~ImageParser() {}

	virtual bool loadFile(const char *fileName) = 0;
	virtual long getWidth() = 0;
	virtual long getHeight() = 0;;
	virtual uint32_t getFileSize() = 0;
	virtual uint32_t getReadSize() = 0;

	virtual void start() {}
	virtual bool startRow() = 0;
	virtual uint16_t nextColumn() = 0;
	virtual bool endRow() = 0;

	static bool supported_format(const char *fileName);
	static ImageParser *parser_factory(const char *fileName, CancelNotifier *cancelNotifier);
	static void wait_for_completion();

protected:
	CancelNotifier *cancelNotifier_;
};

class ImageParserBMP : public ImageParser {
public:
	ImageParserBMP(CancelNotifier *cancelNotifier);
	virtual ~ImageParserBMP();

	virtual bool loadFile(const char *fileName);
	virtual long getWidth() { return width_; }
	virtual long getHeight() { return height_; }
	virtual uint32_t getFileSize() { return fileSize_; }
	virtual uint32_t getReadSize() { return readSize_; }

	virtual bool startRow();
	virtual uint16_t nextColumn();
	virtual bool endRow();

private:
	SDCardReader cardReader_;
	long width_, height_;
	uint32_t fileSize_, readSize_;
	uint8_t *buff_, *rgb_;
	int bufferWidth_, bytesWidth_;
	int bytesLeftOnRow_, bytesLeftInBuffer_;

	bool read_header();
};

class ImageParserStripe : public ImageParser {
public:
	ImageParserStripe(CancelNotifier *cancelNotifier, bool small) :
		ImageParser(cancelNotifier), small_(small), x_(0), y_(0), readSize_(0) {}
	virtual ~ImageParserStripe() {}

	virtual bool loadFile(const char *fileName) { return true; }
	virtual long getWidth() { return small_?500:1000; }
	virtual long getHeight() { return 70; }
	virtual uint32_t getFileSize() { return (uint32_t) getWidth() * (uint32_t) 70; }
	virtual uint32_t getReadSize() { return readSize_; }

	virtual bool startRow();
	virtual uint16_t nextColumn();
	virtual bool endRow();

private:
	bool small_;
	long x_, y_;
	uint32_t readSize_;
};

class ProtocolParserEngraveFileBounds : public ProtocolParserFile {
public:
	ProtocolParserEngraveFileBounds(CancelNotifier *cancelNotifier, ProtocolParser *protocolParser, ImageParser *imageParser);
	virtual ~ProtocolParserEngraveFileBounds();

	virtual bool init_file(const char *fileName);
	virtual bool parse_file_input();
	virtual float get_read_percent() { return 0.0f; }
	virtual unsigned int get_parsing_time() { return 0; }

private:
	ProtocolParser *protocolParser_;
	ImageParser *imageParser_;
};

class ProtocolParserEngraveFileRun : public ProtocolParserFile {
public:
	ProtocolParserEngraveFileRun(CancelNotifier *cancelNotifier, ProtocolParser *protocolParser, ImageParser *imageParser);
	virtual ~ProtocolParserEngraveFileRun();

	virtual bool init_file(const char *fileName);
	virtual bool parse_file_input();
	virtual float get_read_percent() { return percentage_; }
	virtual unsigned int get_parsing_time();

private:
	ProtocolParser *protocolParser_;
	ImageParser *imageParser_;
	float percentage_;
	unsigned int startTime_, totalTime_, timeLeft_;

	void send_line(float *target, float feed_rate, uint16_t spindle_speed, float acceleration);
};

#endif /* PROTOCOL_PARSER_H_ */
