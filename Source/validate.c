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
    char *label_msg;
    int i, success = 0;
    int num_operands = 0;

    /* Check if the token at command_start is not NULL */
    if (tokens[command_start] == NULL) {
        printf("[EMPTY COMMAND]\n");
        return 0;
    }

    /* Get command info using the command_lookup function */
    cmd_info = command_lookup(tokens[command_start]);

    /* Check if command was found */
    if (cmd_info == NULL) {
      print_error("Unkown command", tokens[command_start], LINE_NUMBER);
        return 0;
    }

    /* Initialize addressing modes to invalid */
    operands_adress->destination_op = -1;
    operands_adress->source_op = -1;

    /* Special handling for DATA and STRING directives */
    if (cmd_info->name == CMD_DATA) {
        i = command_start + 1;
        while (tokens[i] != NULL) {
            char *endptr;
            strtol(tokens[i], &endptr, 10);
            if (*endptr != '\0') {
                print_error("Unexpected operand", "expected only numbers in a data decleration", LINE_NUMBER);
                return 0;
            }
            i++;
        }
        printf("DATA COMMAND: TOTAL %d ITEMS\n", i - (command_start));
        return i - (command_start);
    }
    else if (cmd_info->name == CMD_STRING) {
        printf("STRING COMMAND\n");
        return 1;
    }

    /* Determine number of operands based on command type */
    switch (cmd_info->type) {
        case 1: num_operands = 2; break; /* Two-operand commands */
        case 2: num_operands = 1; break; /* One-operand commands */
        case 3: num_operands = 0; break; /* Zero-operand commands */
        case 4: num_operands = 1; break; /* Directive commands .entry .extern */
    }

    /* Check if operands are syntactically valid */
    success = check_operands(command_start, tokens, num_operands, operands_adress, cmd_info->type);

    if (success) {
        /* Additional validation for specific commands */
        switch (cmd_info->name) {
            case CMD_MOV:
            case CMD_CMP:
            case CMD_ADD:
            case CMD_SUB:
            case CMD_LEA:
            case CMD_INC:
            case CMD_DEC:
            case CMD_PRN:
            case CMD_RTS:
            case CMD_STOP:
                /* These commands don't need additional validation beyond what check_operands does */
                break;

            case CMD_CLR:
            case CMD_NOT:
            case CMD_RED:
                if (!is_reg(tokens[command_start+1])) {
                    print_error("Unexpected operand", "operand should be a register", LINE_NUMBER);
                    return 0;
                }
                break;

            case CMD_JMP:
            case CMD_BNE:
            case CMD_JSR:
                if (!(operands_adress->destination_op == 2 || operands_adress->destination_op == 1)) {
                    print_error("Unexpected operand", "operand should be of label", LINE_NUMBER);
                    return 0;
                }
                break;

            case CMD_EXTERN:
            case CMD_ENTRY:
                if (operands_adress->source_op != 1) {
                    char *error_msg = (cmd_info->name == CMD_EXTERN) ?
                        "extern command should contain a label name as an operand to extern" :
                        "entry command should contain a label name as an operand to define as entry";
                    print_error("Unexpected operand", error_msg, LINE_NUMBER);
                    return 0;
                }
                break;

            default:
                print_error("Unkown command", tokens[command_start], LINE_NUMBER);
                return 0;
        }

        /* Check if addressing modes are allowed for this command type using bit flags */
        if (cmd_info->type != 3 && cmd_info->type != 4) { /* Skip for zero-operand and directive commands */
            int src_bit = (operands_adress->source_op >= 0) ? (1 << operands_adress->source_op) : 0;
            int dest_bit = (operands_adress->destination_op >= 0) ? (1 << operands_adress->destination_op) : 0;

            if ((cmd_info->type == 1 && (!(src_bit & cmd_info->allowed_src_add_mode) ||
                                         !(dest_bit & cmd_info->allowed_dest_add_mode))) ||
                (cmd_info->type == 2 && !(dest_bit & cmd_info->allowed_dest_add_mode))) {
                print_error("Invalid addressing mode", "", LINE_NUMBER);
                return 0;
            }
        }

        /* Success message */
        label_msg = (cmd_info->name == CMD_EXTERN || cmd_info->name == CMD_ENTRY) ?
            " FOUND LABEL IN EXTERN/ENTRY" : "";
        printf("ADDRESSING SUCCESS MODES: target%d source%d%s\n",
               operands_adress->destination_op,
               operands_adress->source_op,
               label_msg);
    }

    return success;
}

int check_operands(int command_start, char *tokens[MAX_LINE_LENGTH], int correct_operands, addressModes *operands, int command_type){
  int i; /* Position of the current operand */
  int current_operator = 0; /* 0 for target op, 1 for source op */

  /* Adjust the loop so that it checks exactly the expected number of operands.
     If there's a label definition at the beginning, the operands start at command_start+1. */
  for(i = command_start + 1; i < command_start + correct_operands + 1; i++){
    if(is_reg(tokens[i])){
      /* If the command is of type 2 (one operand) - only dest operator is used */
      if(command_type == 2){
        current_operator ? (operands->source_op = 3) : (operands->destination_op = 3);
      }else{ /*If the command is of type 1 or 3 */
        current_operator ? (operands->destination_op = 3) : (operands->source_op = 3);
      }
      current_operator++;
    }

    else if(tokens[i][0] == '&'){ /* external label mentioned */
      if(valid_label(tokens[i]+1)){
        /* If the command is of type 2 (one operand) - only dest operator is used */
        if(command_type == 2){
          current_operator ? (operands->source_op = 2) : (operands->destination_op = 2);
        }else{ /*If the command is of type 1 or 3 */
          current_operator ? (operands->destination_op = 2) : (operands->source_op = 2);
        }
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
      /* If the command is of type 2 (one operand) - only dest operator is used */
      if(command_type == 2){
        current_operator ? (operands->source_op = 0) : (operands->destination_op = 0);
      }else{ /*If the command is of type 1 or 3 */
        current_operator ? (operands->destination_op = 0) : (operands->source_op = 0);
      }
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
      /* If the command is of type 2 (one operand) - only dest operator is used */
      if(command_type == 2){
        current_operator ? (operands->source_op = 1) : (operands->destination_op = 1);
      }else{ /*If the command is of type 1 or 3 */
        current_operator ? (operands->destination_op = 1) : (operands->source_op = 1);
      }
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

int is_valid_addressing_modes(char *cmd_name, addressModes *modes) {
  int src_bit, dest_bit;
    /* Get the command semantics using the provided lookup function */
    commandSem *cmd_sem = command_lookup(cmd_name);

    /* If command not found, return invalid */
    if (cmd_sem == NULL) {
        return 0;
    }

    /* Convert addressing modes to bit flags for elegant checking */
    src_bit = (modes->source_op >= 0) ? (1 << modes->source_op) : 0;
    dest_bit = (modes->destination_op >= 0) ? (1 << modes->destination_op) : 0;

    /* Check validity based on the command type */
    switch (cmd_sem->type) {
        case 1: /* Two-operand commands */
            /* Both source and destination must be valid */
            return (src_bit & cmd_sem->allowed_src_add_mode) &&
                   (dest_bit & cmd_sem->allowed_dest_add_mode);

        case 2: /* One-operand commands */
            /* No source operand allowed, and destination must be valid */
            return (modes->source_op == -1) &&
                   (dest_bit & cmd_sem->allowed_dest_add_mode);

        case 3: /* Zero-operand commands */
            /* Neither source nor destination operands allowed */
            return (modes->source_op == -1 && modes->destination_op == -1);

        case 4: /* Directive commands */
            /* For directives like .extern and .entry, only direct addressing is valid */
            if (strcmp(cmd_name, "extern") == 0 || strcmp(cmd_name, "entry") == 0) {
                return modes->destination_op == ADDR_DIRECT;
            }
            /* For other directives, addressing modes don't apply the same way */
            return 1;

        default:
            return 0; /* Unknown command type */
    }
}
