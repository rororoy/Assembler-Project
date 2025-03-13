#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/first_stage.h"
#include "../Headers/error.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"
#include "../Headers/validate.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"

int first_pass(char *filename) {
  FILE *file;
  int i;
  char line[MAX_LINE_LENGTH + 2]; /* Buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
  char *tokens[MAX_LINE_LENGTH];
  int tokens_mode;
  addressModes operands_adress;
  int src, tgt;
  /* Define the addressing type - 0 IMM, 1 DIRECT, 2 RELATIVE, 3 IMM REG, -1 ERR */
  int addressing_mode;
  char *am_file = append_extension(filename, ".am");
  int DC = 0, IC = 100;
  int command_start = 0;
  symbolTable *symbol_table = create_symbol_table();
  transTable *my_table = create_transTable(50);


  int tablepointer = 0;

  LINE_NUMBER = 0;

  if (symbol_table == NULL) {
    printf("Failed to create symbol table\n");
    return 0;
  }

  /* Open the am file */
  file = fopen(am_file, "r");
  if (file == NULL) {
    print_error("File read", filename, 0);
    return 0;
  }

  /* Loop through the line checking for different cases */
  while (fgets(line, sizeof(line), file) != NULL) {
    LINE_NUMBER++;
    if (!(tokens_mode = tokanize_line(line, tokens, 0)))
        return 0;

    printf("Tokanized-->");
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        if (tokens[i] == NULL) {
            break;
        }
        printf("%s|", tokens[i]);
    }
    printf("\n");



    /* Encountered a decleration of a label in the line */
    command_start = tokens_mode == 2 ? 1 : 0;
    addressing_mode = is_valid_command(command_start, tokens, &operands_adress);

    if (tokens_mode == 2) {
      if ((tokens[1] != NULL) && (strcmp(tokens[command_start], ".data") == 0 || strcmp(tokens[command_start], ".string") == 0)) {
        /* Incase of .data and .string - special DC treatment is needed */
        if (!insert_symbol(symbol_table, tokens[0], IC+DC, LBL_DATA)) {
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }

        if (strcmp(tokens[command_start], ".string") == 0) {
          DC += strlen(tokens[command_start+1]) + 1;
          printf("THE STRING:%s\n", tokens[command_start+1]);
        } else {
          /* Count reserved space in memory for each data type declared */
          DC += addressing_mode - 1;
        }
        /* TODO FIX THIS DUMB FIX */
        IC--;

      } else {
        if (!insert_symbol(symbol_table, tokens[0], IC+DC, LBL_CODE)) {
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }

        IC += (operands_adress.destination_op != 3 && operands_adress.destination_op != -1) ? 1 : 0;
        IC += (operands_adress.source_op != 3 && operands_adress.source_op != -1) ? 1 : 0;
      }
    }else{
      /* Incase of any other regular non label commands */

      IC += (operands_adress.destination_op != 3 && operands_adress.destination_op != -1) ? 1 : 0;
      IC += (operands_adress.source_op != 3 && operands_adress.source_op != -1) ? 1 : 0;
    }
    IC++;


    /* Run through the command operands and log into the symb table */
    /*
    i = command_start + 1;
    while(tokens[i] != NULL){

    }*/

    process_assembly_command(my_table, &tablepointer, tokens, IC,
                         operands_adress.source_op, operands_adress.destination_op);


  }

  /* Print the table */
  printf("TransTable contents:\n");
  print_complete_transTable(my_table, 9); /* Print just the first entry */

  /* Free the allocated memory */
  free_transTable(my_table, 10);
  printf("\n\nSYMBOL TABLE \n\n\n");
  print_symbol_table(symbol_table);
  return 1;
}

void process_assembly_command(transTable *my_table, int *tablepointer, char **tokens, int IC,
                               int operand_src_type, int operand_dst_type) {
    int src_reg = 0;
    int dst_reg = 0;
    int command_start = 0;
    int opcode = 0; /* Default opcode - will be updated based on actual command */
    int funct = 0;  /* Default function code - will be updated based on actual command */
    char *source_line;
    int extra_words_count = 0;

    /* Find where the command starts (skip label if present) */
    if (tokens[0] != NULL && tokens[1] != NULL && strstr(tokens[0], ":") != NULL) {
        command_start = 1;
    }

    /* Join tokens to create the source line */
    source_line = join_tokens(tokens);
    if (source_line == NULL) {
        fprintf(stderr, "Memory allocation failed for source line\n");
        return;
    }

    /* Determine register numbers if register addressing is used */
    if (operand_src_type == 3) { /* Register addressing for source */
        /* Extract register number - assuming format like "r3" */
        if (tokens[command_start + 1] != NULL && tokens[command_start + 1][0] == 'r') {
            src_reg = tokens[command_start + 1][1] - '0';
        }
    }

    if (operand_dst_type == 3) { /* Register addressing for destination */
        /* Extract register number - assuming format like "r3" */
        if (tokens[command_start + 2] != NULL && tokens[command_start + 2][0] == 'r') {
            dst_reg = tokens[command_start + 2][1] - '0';
        }
    }

    /* Insert the main instruction word */
    insert_command_entry(my_table, *tablepointer, IC, source_line,
                         opcode, operand_src_type, src_reg,
                         operand_dst_type, dst_reg, funct);

    /* Handle extra words based on addressing types */
    int current_word = 1; /* Start after the main word */

    /* Process source operand extra word if needed */
    if (operand_src_type == 0) { /* Immediate addressing (#value) */
        /* Extract the immediate value, assuming format like "#123" */
        if (tokens[command_start + 1] != NULL) {
            int value = atoi(&tokens[command_start + 1][1]); /* Skip the '#' character */
            insert_extra_word(&my_table[*tablepointer], 0, value, current_word++);
            extra_words_count++;
        }
    }
    else if (operand_src_type == 1) { /* Direct addressing (variable name) */
        /* Here you would look up the address of the label in your symbol table */
        /* For now, using placeholder value */
        insert_extra_word(&my_table[*tablepointer], 0, IC, current_word++);
        extra_words_count++;
    }
    else if (operand_src_type == 2) { /* Relative addressing (&label) */
        /* Calculate relative distance - would require symbol table lookup */
        /* For now, using placeholder value */
        insert_extra_word(&my_table[*tablepointer], 0, 50, current_word++);
        extra_words_count++;
    }

    /* Process destination operand extra word if needed */
    if (operand_dst_type == 0) { /* Immediate addressing */
        if (tokens[command_start + 2] != NULL) {
            int value = atoi(&tokens[command_start + 2][1]); /* Skip the '#' character */
            insert_extra_word(&my_table[*tablepointer], 0, value, current_word++);
            extra_words_count++;
        }
    }
    else if (operand_dst_type == 1) { /* Direct addressing */
        /* For now, using placeholder value */
        insert_extra_word(&my_table[*tablepointer], 0, IC, current_word++);
        extra_words_count++;
    }
    else if (operand_dst_type == 2) { /* Relative addressing */
        /* For now, using placeholder value */
        insert_extra_word(&my_table[*tablepointer], 0, 75, current_word++);
        extra_words_count++;
    }

    /* Increment table pointer and update IC based on words used */
    (*tablepointer)++;

    /* Free the source line memory */
    free(source_line);
}
