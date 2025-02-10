#ifndef UTILS_H
#define UTILS_H

#include "../Headers/global.h"

void check_malloc(void *ptr);

int empty_line(char *line);

char *strdup(char *s);

char* append_extension(char *filename, const char *extension);

int tokanize_line(char *line, char *tokens[MAX_LINE_LENGTH], int macro_scan);

#endif
