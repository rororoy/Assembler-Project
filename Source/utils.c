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
