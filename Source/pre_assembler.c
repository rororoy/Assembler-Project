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
#include "../Headers/validate.h"

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

int process_macro_definition(FILE *in, hashBucket *ht_bucket, int *line_count){
  return 0;
}

char *extract_macro(char *line){
  /* TODO CHECK FOR SAVED WORDS WITH AN IMPLEMENTATION OF SAVED_WORDS */
  char *macro_start;
  char *extracted_macro;
  char *ptr = line;
  while(isspace(*ptr)){ptr++;}
  ptr += 4;

  if(!(isspace(*ptr))){
    print_error("Unkown command", "", 0); /* Error in mcro defenition - unkown command */
    return NULL;
  }

  while(isspace(*ptr)){ptr++;}

  macro_start = ptr;

  while(!isspace(*ptr)){
    ptr++;
  }

  if(ptr-macro_start == 0){
    print_error("No macro", "", 0); /* No macro defined */
    return NULL;
  }


  extracted_macro = malloc(ptr-macro_start + 1);

  strncpy(extracted_macro, macro_start, ptr-macro_start);
  extracted_macro[(ptr-macro_start)] = '\0';

  return extracted_macro;
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
            free_hash_table(macro_table);
            fclose(file);
            fclose(temp_file);
            return 0;
        }

        /* Filter out empty lines */
        if (empty_line(line))
            continue;

        if (!tokanize_line(line, tokens, 1)) {
            free_hash_table(macro_table);
            fclose(file);
            fclose(temp_file);
            return 0;
        }

        /* Check for a macro definition start */
        if (strcmp(tokens[0], "mcro") == 0) {
            macro_name = tokens[1];
            if (macro_name == NULL || is_saved_word(macro_name)) {
                free_hash_table(macro_table);
                fclose(file);
                fclose(temp_file);
                return 0;
            }

            ht_bucket = insert_entry(macro_table, macro_name);
            if(ht_bucket == NULL){
              /* TODO ERROR HERE */
              break;
            }

            /* Read the macro body until "mcroend" is encountered.
               Empty lines within the macro definition are filtered out. */
            while (fgets(line, sizeof(line), file) != NULL) {
                line_count++;

                if (!valid_length_line(line)) {
                    print_error("Line length", "", line_count);
                    free_hash_table(macro_table);
                    fclose(file);
                    fclose(temp_file);
                    return 0;
                }

                if (empty_line(line))
                    continue;

                if (!tokanize_line(line, tokens, 1)) {
                    free_hash_table(macro_table);
                    fclose(file);
                    fclose(temp_file);
                    return 0;
                }

                if (strcmp(tokens[0], "mcroend") == 0){
                  break; /* End of macro definition */
                }
                add_node(&ht_bucket->code_nodes, line);
            }
            /* After storing the macro definition, move on to the next line */
            continue;
        }

        /* Check for a macro call: if the first token matches a defined macro */
        ht_bucket = search_table(macro_table, tokens[0]);
        if (ht_bucket != NULL) {
            if (!write_list_to_file(temp_file, ht_bucket->code_nodes, "temp.as")) {
                free_hash_table(macro_table);
                fclose(file);
                fclose(temp_file);
                return 0;
            }
        } else {
            /* A normal line: write it to the output file */
            if (fprintf(temp_file, "%s", line) < 0) {
                print_error("Failed writing", "temp.as", line_count);
                free_hash_table(macro_table);
                fclose(file);
                fclose(temp_file);
                return 0;
            }
        }
    }

    free_hash_table(macro_table);
    fclose(file);
    fclose(temp_file);
    return 1;
}
