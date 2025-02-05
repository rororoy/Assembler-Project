#include <stdio.h>
#include <stdlib.h>
#include "../Headers/pre_assembler.h"
#include "../Headers/error.h"
#include "../Headers/utils.h"

/*
  TODO ADD WRAPPER FUNCTION TO FGETS WHEN READING NEW LINES - TO HANDLE STUFF LIKE SKIPPING NEW LINES AND CHECKING FOR ERRORS
  TODO CHECK WHY test2 doesnt unpack correctly and fails pre proc stage
*/

int main(int argc, char *argv[]){
  int f_count = 1;

  printf("[*] Started assembler program\n\n");

  /* Check if a file name is provided as an argument */
  if (argc < 2) {
      print_error("Usage", "", 0); /* ERROR: BAD USAGE */
      return 1;
  }

  while(f_count < argc){
    printf("[*] Starting the pre assembler stage on %s\n", argv[f_count]);
    if(pre_assembler(argv[f_count])){
      printf("[*] Finished the pre assembler stage on %s\n\n", argv[f_count]);
    }else{
      printf("[!] Failed the pre_assembler stage on %s\n\n", argv[f_count]);
    }

    f_count++;
  }
  return 1;
}
