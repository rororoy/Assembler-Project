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

void handle_exit(FILE *in_file, FILE *am_file, char *filename1, char *filename2){
  if(in_file) { fclose(in_file); }
  if(am_file) { fclose(am_file); }
  free(filename1);
  free(filename2);
}

int pre_assembler(char *filename, hashTable *macro_table) {
  FILE *file, *am_file;
  char line[MAX_LINE_LENGTH + 2]; /* buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
  char *macro_name;
  char *as_filename, *am_filename;
  hashBucket *ht_bucket;
  char *tokens[MAX_LINE_LENGTH];
  int token_mode;
  int result;

  LINE_NUMBER = 0; /* line count */

  as_filename = append_extension(filename, ".as");
  am_filename = append_extension(filename, ".am");

  /* open the original file */
  file = fopen(as_filename, "r");
  if (file == NULL) {
    print_error("File read", filename, LINE_NUMBER);
    handle_exit(NULL, NULL, as_filename, am_filename);
    return 0;
  }

  /* open the temporary output file */
  am_file = fopen(am_filename, "w");
  if (am_file == NULL) {
    print_error("File write", am_filename, LINE_NUMBER);
    handle_exit(file, NULL, as_filename, am_filename);
    return 0;
  }


  while (fgets(line, sizeof(line), file) != NULL) {
    LINE_NUMBER++;

    if (!valid_length_line(line)) {
      print_error("Line length", "", LINE_NUMBER);
      handle_exit(file, am_file, as_filename, am_filename);
      return 0;
    }

    result = tokanize_line(line, tokens, 1);
    token_mode = result; /* Initialize token_mode with the result */
    if(!result){
      /* Encountered an error in the pre-proc stage - terminate */
      handle_exit(file, am_file, as_filename, am_filename);
      return 0;
    }

    /* Comment lines (3) or empty lines (4) should be written to the file but not processed further */
    if (result == 3 || result == 4) {
      /* Write the line to the output file */
      if (fprintf(am_file, "%s", line) < 0) {
        print_error("Failed writing", as_filename, LINE_NUMBER);
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;
      }
      continue;
    }

    /* Check for a start of a macro definition */
    if (strcmp(tokens[0], "mcro") == 0) {
      macro_name = tokens[1];

      if (macro_name == NULL){ /* No macro name provided */
        print_error("No macro", "", LINE_NUMBER);
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;

      }else if(is_saved_word(macro_name)) { /* Macro is a saved word */
        print_error("Saved word", "", LINE_NUMBER);
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;
      }

      ht_bucket = insert_entry(macro_table, macro_name); /* Log the macro */
      if(ht_bucket == NULL){ /* If failed inserting */
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;
      }

      /* Run through the macro and log all of it */
      if(!process_macro_definition(file, ht_bucket)){
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;
      }
      continue;
    }

    /* Check for a macro call: if the first token matches a defined macro */
    ht_bucket = search_table(macro_table, tokens[0]);
    if (ht_bucket != NULL && token_mode != 2) {
      /* We also checked if the first token isnt a label - used for the first pass*/
      /* search_table now only returns BUCKET_MACRO type buckets */
      if (!write_list_to_file(am_file, ht_bucket->code_nodes, as_filename)) {
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;
      }

    } else {
      /* A normal line: write it to the output file */
      if (fprintf(am_file, "%s", line) < 0) {
        print_error("Failed writing", as_filename, LINE_NUMBER);
        handle_exit(file, am_file, as_filename, am_filename);
        return 0;
      }
    }
  }

  handle_exit(file, am_file, as_filename, am_filename);
  return 1;
}

int process_macro_definition(FILE *file, hashBucket *ht_bucket) {
  char line[MAX_LINE_LENGTH + 2];
  char *tokens[MAX_LINE_LENGTH];
  int found_macro_end = 0;  /* Flag to indicate that "mcroend" was encountered */
  int result;


  while (fgets(line, sizeof(line), file) != NULL) {
    LINE_NUMBER++;

    if(!valid_length_line(line)){
      print_error("Line length", "", LINE_NUMBER);
      return 0;
    }

    result = tokanize_line(line, tokens, 1);
    if(!result){
      return 0;
    }

    /* Comment lines (3) or empty lines (4) should be processed by just adding them to the macro */
    if (result == 3 || result == 4) {
      /* Add the comment or empty line to the macro definition */
      add_node(&ht_bucket->code_nodes, line);
      continue;
    }

    if(strcmp(tokens[0], "mcroend") == 0){
      found_macro_end = 1;
      break;  /* End of macro definition */
    }

    /* Log the non-empty macro line */
    add_node(&ht_bucket->code_nodes, line);
  }

  if(!found_macro_end){
    print_error("Macro reached EOF", "", LINE_NUMBER);
    return 0;
  }
  return 1;
}
