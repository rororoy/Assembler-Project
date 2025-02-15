#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include "../Headers/validate.h"
#include "../Headers/global.h"
#include "../Headers/error.h"
#include "../Headers/translate.h"

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
  "string",
  ".extern",
  "extern",
  ".data",
  "data",
  ".entry",
  "entry",
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
      print_error("Saved word", label, 0); /* Saved word */
      return 0;
    }

    /* Passed all checks: the label is valid */
    return 1;
}


int is_valid_command(int command_start, char *tokens[MAX_LINE_LENGTH]) {
    commandSem *cmd_info;

    /* Check if the token at command_start is not NULL */
    if (tokens[command_start] == NULL) {
        printf("[EMPTY COMMAND]\n");
        return 0;
    }

    /* Get command info using the command_lookup function */
    cmd_info = command_lookup(tokens[command_start]);

    /* Check if command was found */
    if (cmd_info == NULL) {
        printf("[UNKNOWN COMMAND]\n");
        return 0;
    }

    /* Process the command based on its type - assuming cmd_info has a 'type' field of type 'commands' */
    switch (cmd_info->name) {
        case CMD_MOV:
            /* Correct syntax: mov <>, <> */
            return 1;

        case CMD_CMP:
            printf("[CMP]\n");
            return 1;

        case CMD_ADD:
            printf("[ADD]\n");
            return 1;

        case CMD_SUB:
            printf("[SUB]\n");
            return 1;

        case CMD_LEA:
            printf("[LEA]\n");
            return 1;

        case CMD_CLR:
            printf("[CLR]\n");
            return 1;

        case CMD_NOT:
            printf("[NOT]\n");
            return 1;

        case CMD_INC:
            printf("[INC]\n");
            return 1;

        case CMD_DEC:
            printf("[DEC]\n");
            return 1;

        case CMD_JMP:
            printf("[JMP]\n");
            return 1;

        case CMD_BNE:
            printf("[BNE]\n");
            return 1;

        case CMD_JSR:
            printf("[JSR]\n");
            return 1;

        case CMD_RED:
            printf("[RED]\n");
            return 1;

        case CMD_RTS:
            printf("[RTS]\n");
            return 1;

        case CMD_PRN:
            printf("[PRN]\n");
            return 1;

        case CMD_STOP:
            printf("[STOP]\n");
            return 1;

        default:
            printf("[UNKNOWN COMMAND]\n");
            return 0;
    }
}
