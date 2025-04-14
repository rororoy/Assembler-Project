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

int check_malloc(void *ptr){
  if(ptr == NULL){
    print_error("Malloc", "", 0);
    return 0;
  }
  return 1;
}

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
    print_error("Missing argument", "", 0);
    return NULL;
  }

  /* Calculate the total length needed: original length + extension length + 1 for null terminator */
  total_length = strlen(filename) + strlen(extension) + 1;
  new_filename = malloc(total_length);
  if (new_filename == NULL) {
    check_malloc(new_filename);
    return NULL;
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
    int comma_seen = 0;         /* Track if we've just seen a comma */

    /* Duplicate the original line so we can modify it */
    char *line = strdup(original_line);
    if (!line) {
        print_error("Malloc", "function: tokanize_line", 0);
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
            /* Check for empty operand between commas */
            if (comma_seen && *p == ',' && !macro_scan) {
                print_error("Missing operand between commas", "", LINE_NUMBER);
                free(line);
                return 0;
            }

            token_start = p;

            while (*p != '\0' && *p != ' ' && *p != '\t' && *p != ',' && *p != ':' && *p != '"') {
                p++;
            }

        } else {
            /* In string mode, just look for the closing quote */
            while (*p != '\0' && *p != '"') {
              p++;
            }
            if (*p == '\0' && !macro_scan) {
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
                    comma_seen = 0;  /* Reset comma flag after a quote start */
                    continue;
                } else {
                    in_string = 0;
                    *p = '\0';
                    tokens[token_count++] = strdup(string_start);
                    p++;
                    comma_seen = 0;  /* Reset comma flag after a string token */
                }
            } else if (*p == ':' && !in_string) {
                if (token_count > 0 && !macro_scan) {
                    print_error("Label not first", "", LINE_NUMBER);
                    free(line);
                    return 0;
                }
                if (label_encountered && !macro_scan) {
                    print_error("Multiple ':' encountered in line", ":", LINE_NUMBER);
                    free(line);
                    return 0;
                }
                label_encountered = 1;
                *p = '\0';
                tokens[token_count++] = strdup(token_start);
                p++;
                comma_seen = 0;  /* Reset comma flag after a label */

            } else if (*p == '\0' || (!in_string && (*p == ' ' || *p == '\t' || *p == ','))) {
                /* Normal token termination */
                char saved = *p;
                *p = '\0';

                /* Only add token if there's content (handles spaces before comma) */
                if (strlen(token_start) > 0) {
                    tokens[token_count++] = strdup(token_start);
                    comma_seen = 0;  /* Reset since we found a token */
                }

                *p = saved;

                /* Set comma flag if we've hit a comma */
                if (*p == ',') {
                    comma_seen = 1;
                }

                if (*p != '\0') p++;
            }
        }

        /* Skip whitespace (but not commas) */
        while (*p == ' ' || *p == '\t') {
            p++;
        }

        /* Handle comma separately to detect missing operands */
        if (*p == ',') {
            if (comma_seen && !macro_scan) {
                print_error("Missing operand between commas", "", LINE_NUMBER);
                free(line);
                return 0;
            }
            comma_seen = 1;
            p++;
        }
    }

    /* Check for trailing comma at end of line */
    if (comma_seen && !macro_scan) {
        print_error("Missing operand between commas", "", LINE_NUMBER);
        free(line);
        return 0;
    }

    /* Free the duplicated line since each token has been separately duplicated */
    free(line);

    /* --- Macro scan handling --- */
    if (macro_scan) {
        if (strcmp(tokens[0], "mcroend") == 0) {
            if (token_count > 1) {
                print_error("Extranous text", "after mcroend", LINE_NUMBER);
                return 0;
            }
        } else if (strcmp(tokens[0], "mcro") == 0) {
            if (token_count > 2) {
                print_error("Extranous text", "after mcro", LINE_NUMBER);
                return 0;
            } else if (token_count < 2) {
                print_error("No macro", "", LINE_NUMBER);
                return 0;
            }
        }
    }

    if (label_encountered && !macro_scan) {
        if (!valid_label(tokens[0])) {
            /* No need to print an error here, valid_label already prints a specific error */
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

/**
 * Converts a word to a hexadecimal string based on its type
 * (determined by position in the linked list and source code).
 *
 * @param word_data The word to convert
 * @param is_first_word Flag indicating if this is the first word in a linked list
 * @param is_data_entry Flag indicating if this is a data entry
 * @param hex_str The output buffer for the hex string (at least 7 bytes)
 */
void word_to_hex_by_type(word word_data, int is_first_word, int is_data_entry, char *hex_str) {
     unsigned int value = 0;
     int i;

     if (is_data_entry) {
         /* For data entries, all words use the data_word format */
         value = word_data.data_word.data & 0xFFFFFF; /* Mask to 24 bits */
     } else if (is_first_word) {
         /* For first word in a non-data entry, use instruction format */
         value = (word_data.instruction.opcode << 18) |
                 (word_data.instruction.src_mode << 16) |
                 (word_data.instruction.src_reg << 13) |
                 (word_data.instruction.dst_mode << 11) |
                 (word_data.instruction.dst_reg << 8) |
                 (word_data.instruction.funct << 3) |
                 (word_data.instruction.a << 2) |
                 (word_data.instruction.r << 1) |
                 (word_data.instruction.e);
     } else {
         /* For subsequent words in a non-data entry, use extra_word format */
         value = (word_data.extra_word.value << 3) |
                 (word_data.extra_word.a << 2) |
                 (word_data.extra_word.r << 1) |
                 (word_data.extra_word.e);
     }

     /* Convert to lowercase hex string using C90-compliant approach */
     sprintf(hex_str, "%06x", value);

     /* Ensure all characters are lowercase */
     for (i = 0; hex_str[i]; i++) {
         hex_str[i] = tolower(hex_str[i]);
     }
}
