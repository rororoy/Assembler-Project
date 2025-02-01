#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/pre_assembler.h"
#include "../Headers/error.h"
#include "../Headers/hash_table.h"
#include "../Headers/linked_list.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"

/*
  TODO ADD A TOKANIZE FUNCTION THAT RUNS THROUGH A LINE AND RETURN AN ARRAY OF MAX 5-4 TOKENS
*/

/*
  Checks if a macro was defined in the start of a line
*/
int check_for_macro(char *line){
  char *ptr = line;
  while(isspace(*ptr)){ptr++;}


  if(strncmp(ptr, "mcro", 4) != 0){
    return 0;
  }
  return 1;
}

char *extract_macro(char *line){
  /* TODO CHECK FOR SAVED WORDS WITH AN IMPLEMENTATION OF SAVED_WORDS */
  char *macro_start;
  char *extracted_macro;
  char *ptr = line;
  while(isspace(*ptr)){ptr++;}
  ptr += 4;

  if(!(isspace(*ptr))){
    print_error(); /* Error in mcro defenition - unkown command */
    return NULL;
  }

  while(isspace(*ptr)){ptr++;}

  macro_start = ptr;

  while(!isspace(*ptr)){
    ptr++;
  }

  if(ptr-macro_start == 0){
    print_error(); /* No macro defined */
    return NULL;
  }


  extracted_macro = malloc(ptr-macro_start + 1);

  strncpy(extracted_macro, macro_start, ptr-macro_start);
  extracted_macro[(ptr-macro_start)] = '\0';

  return extracted_macro;
}



/*

  @param
  @return 0 if encountered error 1 if ok
*/
int pre_assembler(char *filename){
  FILE *file;
  char line[MAX_LINE_LENGTH + 2]; /* Create an 80 char buffer + \0 + \n chars */
  int line_count = 1; /* Line counter */
  hashTable *macro_table;
  char *macro_name;
  hashBucket *ht_bucket;


  /* Open the file */
  file = fopen(filename, "r");
  if(file == NULL) {
    print_error(); /* ERROR: FILE OPEN ERROR */
    return 0;
  }
  macro_table = make_hash_table(HASH_TABLE_INITIAL_SIZE);

  /* Scan and handle macros line by line*/
  while (fgets(line,sizeof(line), file) != NULL){
    printf(">%s\n", line);
    if(!empty_line(line)){ /* Skip empty lines */
      if(!valid_length_line(line)){
        print_error(); /* ERROR: LINE LENGTH */
        return 0;
      }

      if(check_for_macro(line)){ /* Check if the line has a macro defenition */
        if((macro_name = extract_macro(line)) != NULL){ /* Extract macro */
          printf("found macro %s\n", macro_name);
          ht_bucket = insert_entry(macro_table, macro_name);
          fgets(line,sizeof(line), file);
          while(!check_for_macro(line)){
            printf(">>%s\n", line);
            fgets(line,sizeof(line), file);
            insert_node(ht_bucket[hash_function(macro_name) % macro_table->size].code_nodes);
            /* TODO CHECK WHICH OBJ IS RETURNED FROM THE INSERT ENTRY FUNC */
          }
        }
      }
    }
  }
  free_hash_table(macro_table);
  fclose(file);
  return 1;
}
