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

/* TODO RESTRUCTURE PROGRAM SO THAT A FUNCTION THAT CALLS CHECK_MALLOC EXITS IF MALOC PROBLEM
RIGHT NOW IT ONLY PRINTS AN ERROR AND DOESNT SIGNAL TO THE FUNCTION THAT CALLED IT THAT THE MALLOC DIDNT WORK*/
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
    int label_encountered = 0;  /* Flag if a label was defined in the line */
    int command_start = 0; /* Used to tell (in cases of labels) where the command starts */

    /* Duplicate the original line so we can modify it */
    char *line = strdup(original_line);
    if (!line) {
        perror("strdup failed");
        return 0;
    }

    /* Remove trailing newline */
    line[strcspn(line, "\n")] = '\0';

    /* Clear the tokens array */
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        tokens[i] = NULL;
    }

    /* Skip any initial whitespace (spaces or tabs) */
    p = skip_ws(line);
    if (*p == '\0') {
        free(line);
        return 0;  /* Empty or blank line */
    }

    /*
     * Walk over the line character by character. delimiters are: ' ' ',' ':', '\t'
     */
    while (*p != '\0') {
        token_start = p;
        while (*p != '\0' &&
               *p != ' ' && *p != '\t' &&
               *p != ',' && *p != ':') {
            p++;
        }

        /* If we have a token (non-empty substring), process it */
        if (p > token_start) {
            if (*p == ':') {
              if(token_count > 0){
                print_error("Label not first", "", 0);
                free(line);
                return 0;
              }
                /* If a colon has already been encountered, report an error and abort */
              if (label_encountered) {
                    print_error("Multiple ':' encountered in line", ":", 0);
                    free(line);
                    return 0;
                }
                label_encountered = 1;
                command_start = 1;
                *p = '\0';  /* Terminate the token (colon not included) */
                p++;        /* Advance past the colon */
            } else if (*p != '\0') {
                /* For spaces, tabs, or commas, terminate the token */
                *p = '\0';
                p++;
            }
            /* Duplicate the token so that it remains valid after 'line' is freed */
            tokens[token_count++] = strdup(token_start);
        }

        /* Skip any extra delimiters: spaces, tabs, or commas */
        while (*p == ' ' || *p == '\t' || *p == ',') {
            p++;
        }
    }

    /* Free the duplicated line since each token has been separately duplicated */
    free(line);

    /* --- Macro scan handling --- */
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
    }

  if(label_encountered){
      if(!valid_label(tokens[0])){
        print_error("Label definition", tokens[0], 0);
        return 0;
      }
    }

    if(label_encountered){return 2;}
    return 1;
}
