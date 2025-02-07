#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/hash_table.h"
#include "../Headers/error.h"
#include "../Headers/linked_list.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"
#include "../Headers/validate.h"
#include "../Headers/pre_assembler.h"

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


void exit_function(FILE *in_file, FILE *temp_file, hashTable *ht){
  free_hash_table(ht);
  fclose(in_file);
  fclose(temp_file);
}

/*
TODO make the function return and close all its contents for each return call
  @param
  @return 0 if encountered error 1 if ok
*/
int pre_assembler(char *filename) {
    FILE *file, *temp_file;
    char line[MAX_LINE_LENGTH + 2]; /* Buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
    hashTable *macro_table;
    char *macro_name;
    hashBucket *ht_bucket;
    int line_count = 0;
    char *tokens[4] = {"", "", "", ""};

    /* Open the original file */
    file = fopen(filename, "r");
    if (file == NULL) {
      print_error("File read", filename, line_count);
      return 0;
    }

    /* Open the temporary output file */
    temp_file = fopen("temp.as", "w");
    if (temp_file == NULL) {
      print_error("File write", "temp.as", line_count);
      fclose(file);
      return 0;
    }

    macro_table = make_hash_table(HASH_TABLE_INITIAL_SIZE);

    while (fgets(line, sizeof(line), file) != NULL) {
      line_count++;

      if (!valid_length_line(line)) {
        print_error("Line length", "", line_count);
        exit_function(file, temp_file, macro_table);
        return 0;
      }

      /* Filter out empty lines */
      if (empty_line(line))
        continue;

      if (!tokanize_line(line, tokens, 1)) {
        print_error("Line length", "", line_count);
        exit_function(file, temp_file, macro_table);
        return 0;
      }

      /* Check for a start of a macro definition */
      if (strcmp(tokens[0], "mcro") == 0) {
        macro_name = tokens[1];

        if (macro_name == NULL){ /* No macro name provided */
          print_error("No macro", "", line_count);
          exit_function(file, temp_file, macro_table);
          return 0;

        }else if(is_saved_word(macro_name)) { /* Macro is a saved word */
          print_error("Saved word", "", line_count);
          exit_function(file, temp_file, macro_table);
          return 0;
        }

        ht_bucket = insert_entry(macro_table, macro_name); /* Log the macro */
        if(ht_bucket == NULL){ /* If failed inserting */
          exit_function(file, temp_file, macro_table);
          return 0;
        }

        /* Run through the macro and log all of it */
        if(!process_macro_definition(file, ht_bucket, &line_count)){
          exit_function(file, temp_file, macro_table);
          return 0;
        }
      }

      /* Check for a macro call: if the first token matches a defined macro */
      ht_bucket = search_table(macro_table, tokens[0]);
      if (ht_bucket != NULL) {
        if (!write_list_to_file(temp_file, ht_bucket->code_nodes, "temp.as")) {
          exit_function(file, temp_file, macro_table);
          return 0;
        }

      } else {
        /* A normal line: write it to the output file */
        if (fprintf(temp_file, "%s", line) < 0) {
          print_error("Failed writing", "temp.as", line_count);
          exit_function(file, temp_file, macro_table);
          return 0;
        }
      }
    }

    exit_function(file, temp_file, macro_table);
    return 1;
}

int process_macro_definition(FILE *file, hashBucket *ht_bucket, int *line_count) {
  char line[MAX_LINE_LENGTH + 2];
  char *tokens[4] = {"", "", "", ""};
  int found_macro_end = 0;  /* Flag to indicate that "mcroend" was encountered */


  while (fgets(line, sizeof(line), file) != NULL) {
    (*line_count)++;

    if (!valid_length_line(line)) {
      print_error("Line length", "", *line_count);
      return 0;
    }

    if (empty_line(line)) {
      continue;
    }

    if (!tokanize_line(line, tokens, 1)) {
      return 0;
    }

    if (strcmp(tokens[0], "mcroend") == 0) {
      found_macro_end = 1;
      break;  /* End of macro definition */
    }

    /* Log the non-empty macro line */
    add_node(&ht_bucket->code_nodes, line);
  }

  if (!found_macro_end) {
    print_error("Macro reached EOF", "", *line_count);
    return 0;
  }
  return 1;
}
