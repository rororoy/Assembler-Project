#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/pre_assembler.h"
#include "../Headers/errors.h"
#include "../Headers/hash_table.h"
#include "../Headers/linked_list.h"

int check_for_macro(char *line){

}

/*

  @param
  @return 0 if encountered error 1 if ok
*/
int pre_assembler(char *filename){
  FILE *file;
  char line[MAX_LINE_LENGTH + 2]; /* Create an 80 char buffer + \0 + \n chars */
  int line = 1; /* Line counter */

  /* Open the file */
  file = fopen(filename, "r");
  if(file == NULL) {
    print_error(); /* ERROR: FILE OPEN ERROR */
    return 0;
  }

  hashTable *macro_table = make_hash_table(HASH_TABLE_INITIAL_SIZE);

  /* Scan and handle macros line by line*/
  while (fgets(line,sizeof(line), file) != NULL){
    if(!empty_line(line)){ /* Skip empty lines */
      if(!valid_length_line(line)){
        print_error(); /* ERROR: LINE LENGTH */
      }

      if(check_for_macro(line)){

      }
    }
  }

  fclose(file);
  return 1;
}
