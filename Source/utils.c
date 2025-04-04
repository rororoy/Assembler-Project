#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/utils.h"
#include "../Headers/error.h"
#include "../Headers/global.h"
#include "../Headers/validate.h"
#include "../Headers/translate.h"

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

/*
 * Check if a line is a comment (first non-whitespace character is ';')
 * Returns 1 if the line is a comment, 0 otherwise
 */
int is_comment_line(const char *line) {
    /* Skip leading whitespace */
    while (*line != '\0' && isspace((unsigned char)*line)) {
        line++;
    }

    /* Check if first non-whitespace character is a semicolon */
    if (*line == ';') {
        return 1;  /* Line is a comment */
    }

    return 0;  /* Line is not a comment */
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
    int in_string = 0;
    char *string_start = NULL;  /* Added to track start of string content */

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

    while (*p != '\0') {
        if (!in_string) {
            token_start = p;

            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != ',' && *p != ':' && *p != '"') {
                p++;
            }

        } else {
            /* In string mode, just look for the closing quote */
            while (*p != '\0' && *p != '"') {
              p++;
            }
            if (*p == '\0') {
                print_error("Unterminated string", "", LINE_NUMBER);
                free(line);
                return 0;
            }
        }

        /* Process the token */
        if (p >= token_start) {
            if (*p == '"') {
                if (!in_string) {
                    in_string = 1;
                    p++;
                    string_start = p;
                    continue;
                } else {
                    in_string = 0;
                    *p = '\0';
                    tokens[token_count++] = strdup(string_start);
                    p++;
                }
            } else if (*p == ':' && !in_string) {
                if (token_count > 0) {
                    print_error("Label not first", "", LINE_NUMBER);
                    free(line);
                    return 0;
                }
                if (label_encountered) {
                    print_error("Multiple ':' encountered in line", ":", LINE_NUMBER);
                    free(line);
                    return 0;
                }
                label_encountered = 1;
                *p = '\0';
                tokens[token_count++] = strdup(token_start);
                p++;

            } else if (*p == '\0' || (!in_string && (*p == ' ' || *p == '\t' || *p == ','))) {
                /* Normal token termination */
                char saved = *p;
                *p = '\0';
                tokens[token_count++] = strdup(token_start);
                *p = saved;
                if (*p != '\0') p++;
            }
        }

        /* Skip delimiters (but only if we're not in a string) */
        if (!in_string) {
            while (*p == ' ' || *p == '\t' || *p == ',') {
                p++;
            }
        }
    }

    /* Free the duplicated line since each token has been separately duplicated */
    free(line);

    /* --- Macro scan handling --- */
    if (macro_scan) {
        if (strcmp(tokens[0], "mcroend") == 0) {
            if (token_count > 1) {
                print_error("Extraneous text after mcroend", "mcroend", LINE_NUMBER);
                return 0;
            }
        } else if (strcmp(tokens[0], "mcro") == 0) {
            if (token_count > 2) {
                print_error("Extraneous text after mcro", "mcro", LINE_NUMBER);
                return 0;
            } else if (token_count < 2) {
                print_error("No macro name specified after mcro", "", LINE_NUMBER);
                return 0;
            }
        }
    }

    if (label_encountered) {
        if (!valid_label(tokens[0])) {
            print_error("Label definition", tokens[0], LINE_NUMBER);
            return 0;
        }
    }

    if (label_encountered) {
        return 2;
    }
    return 1;
}

char* join_tokens(char **tokens) {
  int i;
  size_t total_len = 0;
  int token_count = 0;
  char *result;
    if (tokens == NULL)
        return NULL;

    /* First, calculate the total length needed and count tokens */

    for (i = 0; tokens[i] != NULL; i++) {
        total_len += strlen(tokens[i]);
        token_count++;

        /* Add space for a separator (except after the last token) */
        if (tokens[i+1] != NULL) {
            total_len += 1; /* Space character */
        }
    }

    /* If no tokens found, return empty string */
    if (token_count == 0) {
        char *empty = (char *)malloc(1);
        if (empty == NULL) return NULL;
        empty[0] = '\0';
        return empty;
    }

    /* Allocate memory for the joined string (plus null terminator) */
    result = (char *)malloc(total_len + 1);
    if (result == NULL)
        return NULL;

    /* Join the tokens */
    result[0] = '\0'; /* Start with empty string */
    for (i = 0; tokens[i] != NULL; i++) {
        strcat(result, tokens[i]);

        /* Add separator (except after the last token) */
        if (tokens[i+1] != NULL) {
            strcat(result, " ");
        }
    }

    return result;
}

/* Extract register number from a register operand */
int get_register_number(char *reg_token) {
    if (reg_token != NULL && reg_token[0] == 'r') {
        return reg_token[1] - '0';
    }
    return 0;  /* Default to r0 if not properly formatted */
}

int calculate_word_position(int is_source, commandSem *cmnd, int operand_src_type) {
    int word_pos = 0; /* Start counting from 0 as requested */

    /* If we're handling a source operand */
    if (is_source) {
        return 1; /* Source operand is always the first extra word */
    }

    /* If we're handling a destination operand */
    if (cmnd->type == 1) { /* Command has two operands */
        /* If source operand uses register addressing, it doesn't need an extra word */
        if (operand_src_type == 3) {
            return 1; /* Dest is the first extra word */
        } else {
            return 2; /* Dest is the second extra word (after source) */
        }
    } else { /* Command has one operand (destination only) */
        return 1; /* The only extra word is for the destination */
    }
}

char* int_to_str(int value) {
    /* Allocate memory for the string (max 12 chars including sign and null terminator) */
    char* buffer = (char*)malloc(12 * sizeof(char));

    if (buffer == NULL) {
        check_malloc(buffer);
    }

    sprintf(buffer, "%d", value);

    return buffer;
}
