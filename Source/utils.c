#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/utils.h"
#include "../Headers/error.h"
#include "../Headers/global.h"

void check_malloc(void *ptr){
  if(ptr == NULL){
    print_error();
  }
}

char *get_first_word(const char *line) {
    if (line == NULL) {
        return NULL;
    }

    while (*line && isspace((unsigned char)*line)) {
        line++;
    }

    if (*line == '\0') {
        return strdup("");
    }

    const char *start = line;

    while (*line && !isspace((unsigned char)*line)) {
        line++;
    }

    size_t len = line - start;

    char *word = malloc(len + 1);
    if (word == NULL) {
        return NULL;
    }

    memcpy(word, start, len);
    word[len] = '\0';

    return word;
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

int tokanize_line(char *line, char *tokens[4]) {
    if (line == NULL || tokens == NULL) {
        return 0;
    }

    char *line_copy = strdup(line);
    if (line_copy == NULL) {
        return 0;
    }

    int count = 0;
    char *token = strtok(line_copy, " \t\n,");

    while (token != NULL) {
        if (count >= 4) {
            free(line_copy);
            return 0;
        }

        if (count == 0) {
            size_t len = strlen(token);
            if (len > 0 && token[len - 1] == ':') {
                token[len - 1] = '\0';
            }
        }

        tokens[count++] = token;
        token = strtok(NULL, " \t\n,");
    }

    while (count < 4) {
        tokens[count++] = NULL;
    }

    /*
     * IMPORTANT:
     * The tokens point into the memory allocated by strdup (line_copy).
     * If you need the tokens to persist beyond the current scope,
     * consider managing the lifetime of 'line_copy' appropriately.
     *
     * For this example, we are not freeing 'line_copy' immediately because
     * the tokens would then point to freed memory. In a real application,
     * you should either return 'line_copy' to the caller (so they can free it later)
     * or manage its lifetime within a larger context.
     */

    return 1;
}
