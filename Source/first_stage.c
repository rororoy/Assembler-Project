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

  symbolTable *symbol_table = create_symbol_table();
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
    if(!(tokens_mode = tokanize_line(line, tokens, 0))) return 0;

    printf("Tokanized-->");
    for(i = 0; i<MAX_LINE_LENGTH; i++){
      if(tokens[i] == NULL){break;}
      printf("%s|", tokens[i]);
    }
    printf("\n");

    /* TODO THIS DOESNT WORK - IT SHOULD RETURN A SPECIAL NUMBER SAYING WE ENCOUNTERED A LABEL IN THE TOKANIZE */
    if(tokens_mode == 2){
      /* Encountered a decleration of a label in the line */
      /* TODO MOVE ERROR PRINTING FOR SAVED WORD OUTSIDE FROM THE FUNCTION */
      if(strcmp(tokens[1],".data") == 0 || strcmp(tokens[1],".string") == 0){
        if(!insert_symbol(symbol_table, tokens[0], IC, LBL_DATA)){
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }
      }else{
        if(!insert_symbol(symbol_table, tokens[0], IC, LBL_CODE)){
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }
      }
    }
    IC++;
  }

  printf("SYMBOL TABLE \n\n\n");
  print_symbol_table(symbol_table);

  return 1;

}
