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

#endif
