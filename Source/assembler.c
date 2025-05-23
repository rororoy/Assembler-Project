#include <stdio.h>
#include <stdlib.h>
#include "../Headers/pre_assembler.h"
#include "../Headers/error.h"
#include "../Headers/utils.h"
#include "../Headers/first_stage.h"
#include "../Headers/linked_list.h"
#include "../Headers/hash_table.h"
#include "../Headers/global.h"
#include "../Headers/utils.h"
#include "../Headers/files.h"

int main(int argc, char *argv[]){
  hashTable *macro_table;
  int f_count = 1;
  int success;

  printf("[*] Started assembler program\n\n");

  /* Check if a file name is provided as an argument */
  if (argc < 2) {
      print_error("Usage", "", 0); /* ERROR: BAD USAGE */
      return 0;
  }

  while(f_count < argc){
    /* Clean up any existing output files at the start of processing each file */
    clean_output_files(argv[f_count]);

    /***************        Pre assembler stage       *************************/
    printf("[*] Starting the pre assembler stage on %s\n", argv[f_count]);

    /* Create macro table for this file */
    macro_table = make_hash_table(HASH_TABLE_INITIAL_SIZE);
    if (macro_table == NULL) {
      print_error("Failed creating structure", "macro table", 0);
      f_count++;
      continue;
    }

    /* Run pre-assembler stage */
    success = pre_assembler(argv[f_count], macro_table);

    if(!success) {
      printf("[!] Failed the pre assembler stage on %s\n\n", argv[f_count]);
      free_hash_table(macro_table);
      f_count++;
      continue;
    }

    printf("[*] Finished the pre assembler stage on %s\n\n", argv[f_count]);

    /***************    First and Second assembler stage    *******************/
    printf("[*] Starting the first assembler stage on %s\n", argv[f_count]);

    /* Run first pass (which includes the second pass internally) */
    success = first_pass(argv[f_count], macro_table);

    if(success) {
      printf("[*] Finished the assembler stage on %s\n\n\n", argv[f_count]);
    } else {
      printf("[!] Failed the assembler stage on %s\n\n\n", argv[f_count]);
    }

    /* Check if error was encountered during processing */
    if(ERROR_ENCOUNTERED) {
      printf("[!] No output files were generated for %s\n\n\n", argv[f_count]);
      ERROR_ENCOUNTERED = 0;
    }

    /* Free the macro table for this file */
    free_hash_table(macro_table);
    f_count++;
  }

  return 0;
}
