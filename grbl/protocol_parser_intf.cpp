#include "Tone.h"
#include "protocol_parser_intf.h"
#include "protocol_parser.h"

static ProtocolParser serialProtocolParser;
static char command_string[80];
static char *last_file = 0, *last_directory = 0;

bool protocol_parse_serial_input(uint8_t c) {
	return serialProtocolParser.parse_input(c);
}

bool protocol_parse_serial_input_str(const char *str) {
	const char *s = str;
	for (;*s; s++) {
		if (!protocol_parse_serial_input(*s)) return false;
	}
	return true;
}

uint32_t protocol_parse_serial_get_failed_line() {
	return serialProtocolParser.get_failed_line();
}

static ProtocolParserFile *fileProtocolParser = 0;

void protocol_parse_input() {
	if (command_string[0] &&
		serialProtocolParser.get_char_counter() == 0) {
		serialProtocolParser.print(command_string);
		command_string[0] = '\0';
	}

	if (fileProtocolParser) {
		if (!fileProtocolParser->parse_file_input()) {
			delete fileProtocolParser;
			fileProtocolParser = 0;

			printPgmString(PSTR("Parser finished\r\n"));
			tone(BUZZER_PIN, 3000, 100);
		}
	}
}

void protocol_parse_set_command_input(const char *command) {
	strcpy(command_string, command);
}

bool is_protocol_parsing_engrave(const char *file) {
	return ImageParser::supported_format(file);
}

static void protocol_parse_set_file_input_impl(const char *file, bool showBoundsOnly) {
	delete fileProtocolParser;
	fileProtocolParser = 0;

	ProtocolParserFile *localFileProtocolParser = 0;
	CancelNotifier *cancelNotifier = new CancelNotifier();
	if (is_protocol_parsing_engrave(file)) {
		ImageParser *imageParser = ImageParser::parser_factory(file, cancelNotifier);
		if (imageParser) {
			if (showBoundsOnly) {
				localFileProtocolParser = new ProtocolParserEngraveFileBounds(cancelNotifier, &serialProtocolParser, imageParser);
			} else {
				localFileProtocolParser = new ProtocolParserEngraveFileRun(cancelNotifier, &serialProtocolParser, imageParser);
			}
		}
	} else {
		localFileProtocolParser = new ProtocolParserGCFile(cancelNotifier, &serialProtocolParser);
	}

	if (!localFileProtocolParser->init_file(file)) {
		delete localFileProtocolParser;
	} else {
		fileProtocolParser = localFileProtocolParser;
	}
}

void protocol_paser_file_cancel() {
	if (fileProtocolParser) {
		fileProtocolParser->cancel();
	}
}

const char *protocol_parse_get_file_last_used() {
	return last_file;
}

const char *protocol_parse_get_directory_last_used() {
	return last_directory;
}

bool protocol_parse_has_file_last_used() {
	return last_file != 0;
}

void protocol_parse_set_file_input(const char *directory, const char *file, bool showBoundsOnly) {
	delete []last_file;
	last_file = new char[strlen(file) + 1];
	strcpy(last_file, file);
	delete []last_directory;
	last_directory = new char[strlen(directory) + 1];
	strcpy(last_directory, directory);


	char path[256];
	strcpy(path, directory);
	strcat(path, "/");
	strcat(path, file);
	protocol_parse_set_file_input_impl(path, showBoundsOnly);
}

bool is_protocol_parsing_file_input() {
	return fileProtocolParser != 0;
}

const char *get_protocol_parsing_file_name() {
	if (fileProtocolParser) {
		return fileProtocolParser->getFileName();
	}
	return "";
}

float get_protocol_parsing_file_percentage() {
	if (fileProtocolParser) {
		return fileProtocolParser->get_read_percent();
	}
	return 0.0f;
}

unsigned int get_protocol_parsing_time() {
	if (fileProtocolParser) {
		return fileProtocolParser->get_parsing_time();
	}
	return 0;
}
