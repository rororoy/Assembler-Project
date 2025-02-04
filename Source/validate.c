#include <string.h>
#include <ctype.h>
#include "../Headers/validate.h"
#include "../Headers/global.h"

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

int is_saved_word(char *str){
  int i;

  for(i = 0; i < NUM_RESERVED_WORDS; i++){
    if(strcmp(str, RESERVED_WORDS[i]) == 0){
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
        return 0;
    }
    for (i = 0; i < len - 1; i++) {
        if (!isalpha((unsigned char)tok[i])) {
            return 0;
        }
    }
    return 1;
}
