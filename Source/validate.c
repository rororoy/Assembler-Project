#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
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


int is_valid_command(int command_start, char *tokens[MAX_LINE_LENGTH], addressModes *operands_adress) {
    commandSem *cmd_info;
    int addressing_mode;
    int i;

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

    operands_adress->destination_op = -1;
    operands_adress->source_op = -1;

    /* Process the command based on its type - assuming cmd_info has a 'type' field of type 'commands' */
    switch (cmd_info->name) {
        case CMD_MOV:
          /* Correct syntax: mov <>, <> */
          if(check_operands(command_start, tokens, 2, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_CMP:
          /* Correct syntax: cmp <>, <> */
          if(check_operands(command_start, tokens, 2, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_ADD:
          /* Correct syntax: cmp <>, <> */
          if(check_operands(command_start, tokens, 2, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_SUB:
          /* Correct syntax: cmp <>, <> */
          if(check_operands(command_start, tokens, 2, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_LEA:
          /* Correct syntax: lea <>, <> */
          if(check_operands(command_start, tokens, 2, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_CLR:
          /* Correct syntax: clr <> */
          if(is_reg(tokens[command_start+1])){
            if(check_operands(command_start, tokens, 1, operands_adress)){
              printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }
          }else{
            print_error("Unexpected operand", "operand should be a register", LINE_NUMBER);
          }
          return 0;

        case CMD_NOT:
          /* Correct syntax: not <> */
          if(is_reg(tokens[command_start+1])){
            if(check_operands(command_start, tokens, 1, operands_adress)){
              printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }
          }else{
            print_error("Unexpected operand", "operand should be a register", LINE_NUMBER);
          }
          return 0;

        case CMD_INC:
          /* Correct syntax: inc <> */
          if(check_operands(command_start, tokens, 1, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }
          return 0;

        case CMD_DEC:
          /* Correct syntax: dec <> */
          if(check_operands(command_start, tokens, 1, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }
          return 0;

        case CMD_JMP:
          /* Correct syntax: jmp (&)<LABEL> */
          if(check_operands(command_start, tokens, 1, operands_adress)){
            if(operands_adress->destination_op == 2){
              printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }else{
              print_error("Unexpected operand", "operand should be of label", LINE_NUMBER);
            }
          }
          return 0;

        case CMD_BNE:
          /* Correct syntax: jmp (&)<LABEL> */
          if(check_operands(command_start, tokens, 1, operands_adress)){
            if(operands_adress->destination_op == 2){
              printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }else{
              print_error("Unexpected operand", "operand should be of label", LINE_NUMBER);
            }
          }
          return 0;

        case CMD_JSR:
          /* Correct syntax: jsr (&)<LABEL> */
          if(check_operands(command_start, tokens, 1, operands_adress)){
            if(operands_adress->destination_op == 2){
              printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }else{
              print_error("Unexpected operand", "operand should be of label", LINE_NUMBER);
            }
          }
          return 0;

        case CMD_RED:
          /* Correct syntax: red <reg> */
          if(is_reg(tokens[command_start+1])){
            if(check_operands(command_start, tokens, 1, operands_adress)){
              printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }
          }else{
            print_error("Unexpected operand", "operand should be a register", LINE_NUMBER);
          }
          return 0;

        case CMD_RTS:
          if(check_operands(command_start, tokens, 0, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_PRN:
          /* Correct syntax: prn <reg> */
          if(check_operands(command_start, tokens, 1, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }
          return 0;

        case CMD_STOP:
          if(check_operands(command_start, tokens, 0, operands_adress)){
            printf("ADDRESSING SUCCESS MODES: target%d source%d\n", operands_adress->destination_op, operands_adress->source_op);
            return 1;
          }else{
            return 0;
          }

        case CMD_EXTERN:
          if(check_operands(command_start, tokens, 1, operands_adress)){
            if(operands_adress->destination_op == 2){ /* if contains only a label as its only operand */
              printf("ADDRESSING SUCCESS MODES: target%d source%d FOUND LABEL IN EXTERN\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }else{
              print_error("Unexpected operand", "extern command should contain a label name as an operand to extern", LINE_NUMBER);
            }
          }
          return 0;

        case CMD_ENTRY:
          if(check_operands(command_start, tokens, 1, operands_adress)){
            if(operands_adress->destination_op == 2){ /* if contains only a label as its only operand */
              printf("ADDRESSING SUCCESS MODES: target%d source%d FOUND LABEL IN ENTRY\n", operands_adress->destination_op, operands_adress->source_op);
              return 1;
            }else{
              print_error("Unexpected operand", "entry command should contain a label name as an operand to define as entry", LINE_NUMBER);
            }
          }
          return 0;

        case CMD_DATA:
          i = command_start + 1;

          while (tokens[i] != NULL) {
            char *endptr;
            long value = strtol(tokens[i], &endptr, 10);

            /* Check if conversion was successful and the entire string was used */
            if (*endptr != '\0') {
              print_error("Unexpected operand", "expected only numbers in a data decleration", LINE_NUMBER);
              return 0;  /* Not a valid number */
            }

            /* Check for overflow/underflow (though in a 32-bit environment,
              we might need to be careful about range)
            if ((value == LONG_MAX || value == LONG_MIN) && errno == ERANGE) {
                return 0;   Overflow or underflow
            }
            */

            i++;
          }
          printf("DATA COMMAND: TOTAL %d ITEMS\n", i - (command_start));

          return i - (command_start);  /* All tokens are valid numbers */

        case CMD_STRING:
          /* All validation is done in the tokanize function no reason to validate */
          printf("STRING COMMAND\n");

          return 1;

        default:
            printf("[UNKNOWN COMMAND]\n");
            return 0;
    }
}

int check_operands(int command_start, char *tokens[MAX_LINE_LENGTH], int correct_operands, addressModes *operands){
  int i; /* Position of the current operand */
  int current_operator = 0; /* 0 for target op, 1 for source op */

  /* Adjust the loop so that it checks exactly the expected number of operands.
     If there's a label definition at the beginning, the operands start at command_start+1. */
  for(i = command_start + 1; i < command_start + correct_operands + 1; i++){
    if(is_reg(tokens[i])){
      current_operator ? (operands->source_op = 3) : (operands->destination_op = 3);
      current_operator++;
    }

    else if(tokens[i][0] == '&'){ /* external label mentioned */
      if(valid_label(tokens[i]+1)){
        operands->destination_op = 2;
        current_operator++;
      }
    }

    else if(tokens[i][0] == '#'){  /* Immediate operand */
      int j = 1;
      /* Check for optional sign */
      if (tokens[i][1] == '+' || tokens[i][1] == '-') {
        j = 2;  /* Move past the sign */
      }
      /* Check if there's at least one digit after '#' or the sign */
      if (tokens[i][j] == '\0') {
        print_error("No number after #", tokens[command_start], LINE_NUMBER);
        return 0;
      }
      /* Check that all remaining characters are digits */
      while (tokens[i][j] != '\0') {
        if (!isdigit(tokens[i][j])) {
          print_error("Invalid digit in immediate operand", tokens[command_start], LINE_NUMBER);
          return 0;
        }
        j++;
      }
      current_operator ? (operands->source_op) = 0 : (operands->destination_op = 0);
      current_operator++;
    }

    else {
      /* Assume operand is intended to be a label.
         Check that every character is alphanumeric (letters and digits only) */
      int j = 0;
      while (tokens[i][j] != '\0') {
        if (!isalnum(tokens[i][j])){
          print_error("Invalid label (non-alphanumeric character)", tokens[command_start], LINE_NUMBER);
          return 0;
        }
        j++;
      }
      current_operator ? (operands->source_op = 2) : (operands->destination_op = 2);
      current_operator++;
    }
  }

  /* If there is an extra operand beyond the allowed number, signal an error */
  if(tokens[i] != NULL){
    print_error("Too many operands", tokens[command_start], LINE_NUMBER);
    return 0;
  }

  return 1;
}


int is_reg(char *word) {
    int i;
    /* Loop through all registers */
    for (i = 0; registers[i] != NULL; i++) {
        if (strcmp(registers[i], word) == 0) {
            return 1;  /* Found a match */
        }
    }

    return 0;  /* No match found */
}
