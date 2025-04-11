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

/*
  TODO ADD WRAPPER FUNCTION TO FGETS WHEN READING NEW LINES - TO HANDLE STUFF LIKE SKIPPING NEW LINES AND CHECKING FOR ERRORS
  TODO CHECK WHY test2 doesnt unpack correctly and fails pre proc stage
  TODO CHANGE THE WAY ERRORS ARE PRINTED SO THAT THE LINE IS FIRST TO MAKE ERROR MSG CLEARER
*/

int main(int argc, char *argv[]){
  hashTable *macro_table;
  int f_count = 1;

  printf("[*] Started assembler program\n\n");

  /* Check if a file name is provided as an argument */
  if (argc < 2) {
      print_error("Usage", "", 0); /* ERROR: BAD USAGE */
      return 1;
  }

  while(f_count < argc){
    clean_output_files(argv[f_count]);

    /***************        Pre assembler stage       *************************/
    macro_table = make_hash_table(HASH_TABLE_INITIAL_SIZE);
    printf("[*] Starting the pre assembler stage on %s\n", argv[f_count]);


    if(pre_assembler(argv[f_count], macro_table)){
      printf("[*] Finished the pre assembler stage on %s\n\n", argv[f_count]);
    }else{
      printf("[!] Failed the pre assembler stage on %s\n\n", argv[f_count]);
      break;
    }

    /***************    First ans Second assembler stage    *******************/
    printf("[*] Starting the first assembler stage on %s\n", argv[f_count]);

    if(first_pass(argv[f_count], macro_table)){
      printf("[*] Finished the assembler stage on %s\n\n", argv[f_count]);
    }else{
      printf("[!] Failed the assembler stage on %s\n\n", argv[f_count]);
    }

    if(ERROR_ENCOUNTERED){
      printf("[!] No output files were generated for %s\n\n", argv[f_count]);
      ERROR_ENCOUNTERED = 0;
    }

    f_count++;
  }
  return 1;
}
