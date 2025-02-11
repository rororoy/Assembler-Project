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
  node *labels_list = NULL;
  int i;
  char line[MAX_LINE_LENGTH + 2]; /* Buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
  char *tokens[MAX_LINE_LENGTH];
  int tokens_mode;

  char *am_file = append_extension(filename, ".am");

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

    /* TODO THIS DOESNT WORK - IT SHOULD RETURN A SPECIAL NUMBER SAYING WE ENCOUNTERED A LABEL IN THE TOKANIZE */
    if(tokens_mode == 2){
      /* TODO MOVE ERROR PRINTING FOR SAVED WORD OUTSIDE FROM THE FUNCTION */
      printf("   FOUND LABEL   ");
    }
    printf("\n");

  }

  return 0;

}
