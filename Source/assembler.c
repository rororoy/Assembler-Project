#include <stdio.h>
#include <stdlib.h>
#include "../Headers/pre_assembler.h"


int main(int argc, char *argv[]){
  int f_count;

  printf("[*] Started assembler program\n");

  /* Check if a file name is provided as an argument */
  if (argc != 2) {
      print_error(); /* ERROR: BAD USAGE */
      return 1;
  }

  while(f_count < argc){
    printf("[*] Starting the pre assembler stage\n");

    if(pre_assembler(argv[f_count])){
      printf("[!] Failed the pre_assembler stage on %s", argv[f_count]);
    }

    f_count--;
  }
  return 1;
}
