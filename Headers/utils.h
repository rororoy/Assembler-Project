#ifndef UTILS_H
#define UTILS_H

#include "../Headers/global.h"

void check_malloc(void *ptr);

int empty_line(char *line);

char *strdup(char *s);

char* append_extension(char *filename, const char *extension);

char *get_substring(char *start, char *end);

int tokanize_line(char *line, char *tokens[MAX_LINE_LENGTH], int macro_scan);

char *join_tokens(char **tokens);

#endif
