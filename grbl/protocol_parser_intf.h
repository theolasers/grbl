#ifndef PROTOCOL_PARSER_SERIAL_H_
#define PROTOCOL_PARSER_SERIAL_H_

#include "grbl.h"

#ifdef __cplusplus
extern "C" {
#endif

bool protocol_parse_serial_input(uint8_t c);
bool protocol_parse_serial_input_str(const char *str);
uint32_t protocol_parse_serial_get_failed_line();
void protocol_parse_input();

void protocol_parse_set_command_input(const char *command);
void protocol_parse_set_file_input(const char *directory, const char *file, bool showBoundsOnly);
void protocol_paser_file_cancel();
const char *protocol_parse_get_file_last_used();
const char *protocol_parse_get_directory_last_used();
bool protocol_parse_has_file_last_used();
bool is_protocol_parsing_file_input();
bool is_protocol_parsing_engrave(const char *file);
const char *get_protocol_parsing_file_name();
float get_protocol_parsing_file_percentage();
unsigned int get_protocol_parsing_time();

#ifdef __cplusplus
} // extern "C"
#endif

#endif /* PROTOCOL_PARSER_SERIAL_H_ */
