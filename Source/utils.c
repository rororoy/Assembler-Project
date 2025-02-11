#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/utils.h"
#include "../Headers/error.h"
#include "../Headers/global.h"
#include "../Headers/validate.h"

/*
 * skip_ws
 *
 * Skips any leading whitespace. This helper is modular so that you can later
 * change (or remove) this behavior.
 */
char *skip_ws(char *s)
{
    while (*s && isspace(*s)) {
        s++;
    }
    return s;
}

void check_malloc(void *ptr){
  if(ptr == NULL){
    print_error("Malloc", "", 0);
  }
}

/*
  @return 0 if non-empty line, return 1 if empty line
*/
int empty_line(char *line){
  int i;
  if(line == NULL){
    return 1;
  }

  for(i = 0; line[i] != '\0'; i++){
    if(line[i] != ' ' && line[i] != '\t' && line[i] != '\n'){
      return 0;
    }
  }
  return 1;
}


char *strdup(char *s){
    char *dup = malloc(strlen(s) + 1);
    if (dup) strcpy(dup, s);
    return dup;
}

char* append_extension(char *filename, const char *extension) {
  size_t total_length;
  char *new_filename;

  if (filename == NULL || extension == NULL){
    print_error("Missing arguments", "", 0);
    return NULL;
  }

  /* Calculate the total length needed: original length + extension length + 1 for null terminator */
  total_length = strlen(filename) + strlen(extension) + 1;
  new_filename = malloc(total_length);
  if (new_filename == NULL) {
    perror("malloc failed");
    exit(EXIT_FAILURE);
  }

  /* Copy the original filename and append the extension */
  strcpy(new_filename, filename);
  strcat(new_filename, extension);

  return new_filename;
}

char *get_substring(char *start, char *end) {
    size_t len = end - start;
    char *substr = malloc(len + 1);
    if (substr == NULL) {
        return NULL;
    }
    memcpy(substr, start, len);
    substr[len] = '\0';
    return substr;
}

int tokanize_line(char *original_line, char *tokens[MAX_LINE_LENGTH], int macro_scan) {
    char *p;
    int i;
    int token_count = 0;
    char *token_start;

    char *line = strdup(original_line);
    if (!line) {
        perror("strdup failed");
        return 0;
    }

    line[strcspn(line, "\n")] = '\0';  /* Remove trailing newline */

    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        tokens[i] = NULL;
    }

    p = skip_ws(line);
    if (*p == '\0' || *p == '\n') {
        return 0;  /* Caller must free this */
    }

    while (*p != '\0' && *p != '\n') {
        token_start = p;
        while (*p != '\0' && *p != '\n' && *p != ' ' && *p != '\t' && *p != ',' && *p != ':') {
            p++;
        }
        if (p > token_start) {
            if (*p != '\0' && *p != '\n') {
                *p = '\0';
                p++;
            }
            tokens[token_count++] = token_start;
        }
        while (*p == ' ' || *p == '\t' || *p == ',') {
            p++;
        }
    }

    if (macro_scan) {
        if (strcmp(tokens[0], "mcroend") == 0) {
            if (token_count > 1) {
                print_error("Extraneous text after mcroend", "mcroend", 0);
                return 0;
            }
        } else if (strcmp(tokens[0], "mcro") == 0) {
            if (token_count > 2) {
                print_error("Extraneous text after mcro", "mcro", 0);
                return 0;
            } else if (token_count < 2) {
                print_error("No macro name specified after mcro", "", 0);
                return 0;
            }
        }
        return 1;
    }

    return 1;
}
