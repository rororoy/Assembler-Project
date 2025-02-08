#ifndef UTILS_H
#define UTILS_H

void check_malloc(void *ptr);

int empty_line(char *line);

char *strdup(char *s);

char* append_extension(char *filename, const char *extension);

int tokanize_line(char *line, char *tokens[4], int macro_scan);

#endif
