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
  /* Define the addressing type - 0 IMM, 1 DIRECT, 2 RELATIVE, 3 IMM REG, -1 ERR */
  int addressing_mode;
  char *am_file = append_extension(filename, ".am");
  int DC = 0, IC = 100;
  int command_start = 0;
  symbolTable *symbol_table = create_symbol_table();



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
    i = command_start + 1;
    while(tokens[i] != NULL){

    }

  }

  printf("\n\nSYMBOL TABLE \n\n\n");
  print_symbol_table(symbol_table);
  return 1;
}
