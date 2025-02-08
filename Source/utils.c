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


int tokanize_line(char *original_line, char *tokens[4], int macro_scan)
{
    char *p;
    int token_count = 0;
    int i;
    char *start;
    int command_index = 0;
    int len;
    char *line = strdup(original_line);
    line[strcspn(line, "\n")] = '\0';

    /* Skip any initial whitespace */
    p = skip_ws(line);
    if (*p == '\0' || *p == '\n') {
        return 0;  /* Encountered an empty line - skip */
    }

    /* Loop over the line until we hit the newline or end of string */
    while (*p && *p != '\n') {
        /* Skip delimiters: whitespace and commas */
        while (*p && (*p == ' ' || *p == '\t' || *p == ',')) {
            p++;
        }
        if (*p == '\0' || *p == '\n') {
            break;
        }
        if (token_count >= 4) {
            print_error("Extranous text", "", 0); /* Extranous text at the end */
            return 0;
        }
        if (*p == '"') {
            /* A token starting with a double quote: string literal */
            start = p;  /* Include the opening quote */
            p++;  /* Advance past the opening quote */
            while (*p && *p != '\n' && *p != '"') {
                p++;
            }
            if (*p != '"') {
                /* No closing quote found */
                return 0;
            }
            p++;  /* Move past the closing quote */
            /* Optionally, if a delimiter follows, null-terminate here */
            if (*p && (*p == ' ' || *p == '\t' || *p == ',')) {
                *p = '\0';
                p++;
            }
            tokens[token_count++] = start;
        } else {
            /* A non-quoted token */
            start = p;
            while (*p && *p != '\0' && *p != '\n' &&
                   !isspace((unsigned char)*p) && *p != ',') {
                p++;
            }
            if (*p && *p != '\n') {
                *p = '\0';
                p++;
            }
            tokens[token_count++] = start;
        }
        /* Skip any delimiters following the token */
        while (*p && (*p == ' ' || *p == '\t' || *p == ',')) {
            p++;
        }
    }

    /* Check for an optional label */
    if (token_count > 0) {
        len = strlen(tokens[0]);
        if (len > 0 && tokens[0][len - 1] == ':') {
            if (!valid_label(tokens[0])) {
                return 0;
            }
            command_index = 1;  /* The command is the next token */
        } else {
            command_index = 0;
        }
        if (command_index >= token_count) {
            /* No command token found after the label */
            return 0;
        }
    } else {
        return 0;
    }

    if (!macro_scan) {
      if (is_saved_word(tokens[command_index])) {
          /* For each argument token (if not a string literal), ensure it is not a saved word */
          for (i = command_index + 1; i < token_count; i++) {
              if (tokens[i][0] != '"') {
                  if (is_saved_word(tokens[i])) {
                      return 0;
                  }
              }
          }
        }
    } else {
        /* Not a saved word: check for valid macro commands */
        if (strcmp(tokens[command_index], "mcroend") == 0) {
            /* "mcroend" must be the only token (or only token after a label) */
            if (token_count != command_index + 1){
              print_error("Extranous text", "mcroend", 0);
              return 0;
            }
        } else if (strcmp(tokens[command_index], "mcro") == 0) {
            /* "mcro" must be followed by exactly one token (the macro name) */
          if (token_count > 2){
            print_error("Extranous text", "mcro", 0);
            return 0;
          }else if(token_count < 2){
            print_error("No macro", "", 0);
            return 0;
          }
      }
  }

    /* Set any remaining tokens to NULL */
  for (i = token_count; i < 4; i++) {
    tokens[i] = NULL;
  }
  return 1;
}
