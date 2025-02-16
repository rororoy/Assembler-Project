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

int first_pass(char *filename){
  FILE *file;
  int i;
  char line[MAX_LINE_LENGTH + 2]; /* Buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
  char *tokens[MAX_LINE_LENGTH];
  int tokens_mode;

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
    return 0  ;
  }

  /* Loop through the line checking for different cases */
  while (fgets(line, sizeof(line), file) != NULL) {
    LINE_NUMBER++;

    if(!(tokens_mode = tokanize_line(line, tokens, 0))) return 0;

    printf("Tokanized-->");
    for(i = 0; i<MAX_LINE_LENGTH; i++){
      if(tokens[i] == NULL){break;}
      printf("%s|", tokens[i]);
    }
    printf("\n");

    /* Encountered a decleration of a label in the line */
    if(tokens_mode == 2){
      if((tokens[1] != NULL) && (strcmp(tokens[1],".data") == 0 || strcmp(tokens[1],".string") == 0)){
        if(!insert_symbol(symbol_table, tokens[0], IC+DC, LBL_DATA)){
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }

        if(strcmp(tokens[1],".string") == 0){
          DC += strlen(tokens[1]) + 1;
        }else{
          /* Count reserved space in memory for each data type declared */
          for(i = 2; i<MAX_LINE_LENGTH; i++){
            if(tokens[i] == NULL){ break; }
            DC++;
          }
        }


      }else{
        if(!insert_symbol(symbol_table, tokens[0], IC, LBL_CODE)){
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }
      }
      command_start = 1;
    }

    if(!is_valid_command(command_start, tokens)){

    }

    command_start = 0;
    IC++;
  }

  printf("SYMBOL TABLE \n\n\n");
  print_symbol_table(symbol_table);

  return 1;

}
