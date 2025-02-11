#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../Headers/validate.h"
#include "../Headers/global.h"
#include "../Headers/error.h"

char *RESERVED_WORDS[] = {
  "mov",
  "add",
  "cmp",
  "sub",
  "lea",
  "clr",
  "not",
  "inc",
  "dec",
  "jmp",
  "bne",
  "jsr",
  "red",
  "prn",
  "rts",
  "stop",
  ".string",
  ".extern",
  ".data",
  ".entry",
  "mcro",
  "mcroend"
};

/*
  Recieves a line from the assembly file and checks if it falls withing the 80
  chars length limit
  @return 0 if the line length is too long return 1 if valid
*/
int valid_length_line(char *line){
  return(strlen(line) <= MAX_LINE_LENGTH);
}

int is_saved_word(char *str){
  int i;

  for(i = 0; i < NUM_RESERVED_WORDS; i++){
    if(strcmp(str, RESERVED_WORDS[i]) == 0){
      print_error("Saved word", str, 0); /* Saved word */
      return 1;
    }
  }

  return 0;
}

int valid_label(char *label) {
    int i;

    /* Check for a NULL pointer or an empty string */
    if (label == NULL || label[0] == '\0') {
        return 0;
    }

    /* The first character must be a letter */
    if (!isalpha(label[0])) {
        return 0;
    }

    /* Check that every character is alphanumeric */
    for (i = 0; label[i] != '\0'; i++) {
        if (!isalnum(label[i])) {
            return 0;
        }
    }

    /* Use is_saved_word to reject reserved words */
    if (is_saved_word(label)) {
        return 0;
    }

    /* Passed all checks: the label is valid */
    return 1;
}
