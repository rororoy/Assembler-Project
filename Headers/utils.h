#ifndef UTILS_H
#define UTILS_H

#include "../Headers/global.h"
#include "../Headers/translate.h"

void check_malloc(void *ptr);

int empty_line(char *line);

int is_comment_line(const char *line);

char *strdup(char *s);

char* append_extension(char *filename, const char *extension);

char *get_substring(char *start, char *end);

int tokanize_line(char *line, char *tokens[MAX_LINE_LENGTH], int macro_scan);

char *join_tokens(char **tokens);

int get_register_number(char *reg_token);

int calculate_word_position(int is_source, commandSem *cmnd, int operand_src_type);

/**
 * Converts a word to a hexadecimal string based on its type.
 *
 * @param word_data The word to convert
 * @param is_first_word Flag indicating if this is the first word in a linked list
 * @param is_data_entry Flag indicating if this is a data entry
 * @param hex_str The output buffer for the hex string (at least 7 bytes)
 */
void word_to_hex_by_type(word word_data, int is_first_word, int is_data_entry, char *hex_str);

#endif
