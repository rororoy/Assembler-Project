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

int valid_label(char *tok){
    int i, len;
    len = strlen(tok);
    if (len < 2) {
        return 0;  /* too short to be a label */
    }
    if (tok[len - 1] != ':') {
      print_error("Unkown command", tok, 0); /* unkown command used */
      return 0;
    }
    for (i = 0; i < len - 1; i++) {
        if (!isalpha((unsigned char)tok[i])) {
          print_error("Unkown command", tok, 0); /* Label defenition */
          return 0;
        }
    }
    return 1;
}
