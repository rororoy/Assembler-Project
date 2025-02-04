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
    print_error();
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

/*
  Recieves a line from the assembly file and checks if it falls withing the 80
  chars length limit
  @return 0 if the line length is too long return 1 if valid
*/
int valid_length_line(char *line){
  return(strlen(line) <= MAX_LINE_LENGTH);
}

int tokanize_line(char *original_line, char *tokens[4], int check_saved_word)
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
        return 0;  /* empty line is considered a failure */
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
            /* Too many tokens */
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

    /* Verify that the command token is one of the saved words */
    if (!is_saved_word(tokens[command_index]) && check_saved_word) {
      return 0;  /* Fail if macros are not allowed */
    }

    /* For each argument token (if not a string literal), ensure it is not a saved word */
    for (i = command_index + 1; i < token_count; i++) {
        if (tokens[i][0] != '"') {
            if (is_saved_word(tokens[i])) {
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
