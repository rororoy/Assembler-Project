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


int tokanize_line(char *original_line, char *tokens[MAX_LINE_LENGTH], int macro_scan)
{
    char *p;
    int token_count = 0;
    int i;
    char *start;
    int command_index = 0;
    int len;

    /* Duplicate the original line so we can modify it */
    char *line = strdup(original_line);
    if (!line) {
        perror("strdup failed");
        return 0;
    }
    line[strcspn(line, "\n")] = '\0';  /* Remove trailing newline */

    /* Clear the tokens array */
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        tokens[i] = NULL;
    }

    /* Skip any initial whitespace */
    p = skip_ws(line);
    if (*p == '\0' || *p == '\n') {
        free(line);
        return 0;  /* Empty line */
    }

    /*
     * Tokenize the line.
     * We treat whitespace and commas as delimiters.
     * A token that begins with a double-quote is treated as a string literal.
     */
    while (*p && *p != '\n') {
        /* Skip any delimiter characters */
        while (*p && (*p == ' ' || *p == '\t' || *p == ',')) {
            p++;
        }
        if (*p == '\0' || *p == '\n') {
            break;
        }
        /* If token begins with a double quote, scan until the closing quote */
        if (*p == '"') {
            start = p;  /* Include the opening quote */
            p++;  /* Skip opening quote */
            while (*p && *p != '\n' && *p != '"') {
                p++;
            }
            if (*p != '"') {
                print_error("Missing closing quote", start, 0);
                free(line);
                return 0;
            }
            p++;  /* Skip closing quote */
            /* Optionally, null-terminate if a delimiter follows */
            if (*p && (*p == ' ' || *p == '\t' || *p == ',')) {
                *p = '\0';
                p++;
            }
            tokens[token_count++] = start;
        } else {
            /* Non-quoted token */
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
        /* Skip any delimiters that follow */
        while (*p && (*p == ' ' || *p == '\t' || *p == ',')) {
            p++;
        }
    }

    /* --- Check if the first token contains a colon (label delimiter) --- */
    if (token_count > 0) {
        char *colon = strchr(tokens[0], ':');
        if (colon != NULL) {
            /* We found a colon in the first token.
             * Replace it with '\0' to end the label.
             * If there is text immediately after the colon, insert it as a new token.
             */
            *colon = '\0';  /* Now tokens[0] is just the label */
            if (*(colon + 1) != '\0') {
                /* Shift tokens right to make room for the new token */
                for (i = token_count; i > 0; i--) {
                    tokens[i + 1] = tokens[i];
                }
                tokens[1] = colon + 1;
                token_count++;
            }
        }
    }

    /* --- Check for an optional label --- */
    if (token_count > 0) {
        len = strlen(tokens[0]);
        /* If the first token came from a label declaration, it should now be only letters/digits.
           Validate it with valid_label. */
        if (valid_label(tokens[0])) {
            command_index = 1;  /* Command is expected as the next token */
        } else {
            command_index = 0;  /* No valid label; treat first token as command */
        }
        if (command_index >= token_count) {
            print_error("Missing command after label", "", 0);
            free(line);
            return 0;
        }
    } else {
        free(line);
        return 0;
    }

    /* --- Handle directives that begin with '.' --- */
    if (tokens[command_index][0] == '.') {
        if (strcmp(tokens[command_index], ".string") == 0) {
            /* The .string directive must have exactly one parameter (a quoted string) */
            if (token_count != command_index + 2) {
                print_error("Invalid number of parameters for .string directive", "", 0);
                free(line);
                return 0;
            }
            char *strParam = tokens[command_index + 1];
            int strLen = strlen(strParam);
            if (strLen < 2 || strParam[0] != '"' || strParam[strLen - 1] != '"') {
                print_error("Invalid string literal in .string directive", strParam, 0);
                free(line);
                return 0;
            }
            /* Optionally: strip the quotes and process the string */
        }
        else if (strcmp(tokens[command_index], ".data") == 0) {
            /* The .data directive must have at least one parameter (an integer) */
            if (token_count <= command_index + 1) {
                print_error("Missing parameters in .data directive", "", 0);
                free(line);
                return 0;
            }
            for (i = command_index + 1; i < token_count; i++) {
                char *numToken = tokens[i];
                char *endptr;
                long val = strtol(numToken, &endptr, 10);
                if (*endptr != '\0') {
                    print_error("Invalid number in .data directive", numToken, 0);
                    free(line);
                    return 0;
                }
                /* Process the number 'val' as needed (e.g., add it to your data image) */
            }
        }
        else if (strcmp(tokens[command_index], ".extern") == 0) {
            /* Placeholder for .extern handling */
        }
        else if (strcmp(tokens[command_index], ".entry") == 0) {
            /* Placeholder for .entry handling */
        }
    }

    /* --- Macro scan handling --- */
    if (macro_scan) {
        if (strcmp(tokens[command_index], "mcroend") == 0) {
            /* "mcroend" must be the only token (or only token after a label) */
            if (token_count != command_index + 1) {
                print_error("Extraneous text after mcroend", "mcroend", 0);
                free(line);
                return 0;
            }
        }
        else if (strcmp(tokens[command_index], "mcro") == 0) {
            /* "mcro" must be followed by exactly one token (the macro name) */
            if (token_count > command_index + 2) {
                print_error("Extraneous text after mcro", "mcro", 0);
                free(line);
                return 0;
            }
            else if (token_count < command_index + 2) {
                print_error("No macro name specified after mcro", "", 0);
                free(line);
                return 0;
            }
        }
    }

    /*
     * (Optionally) Set any remaining tokens to NULL.
     * (This may not be necessary if your tokens array is larger than token_count.)
     */
    for (i = token_count; i < MAX_LINE_LENGTH; i++) {
        tokens[i] = NULL;
    }

    free(line);
    return 1;
}
