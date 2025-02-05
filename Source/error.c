#include <stdio.h>
#include "../Headers/error.h"

error errors[] = {
  {INTERNAL, "File write", "Error opening file for writing"},
  {INTERNAL, "File read", "Error opening file for reading"},
  {INTERNAL, "Missing Argument", "Missing arguents in function"},
  {INTERNAL, "Malloc", "Failed on memory alocation"},
  {INTERNAL, "Insert hash", "Encountered error inserting entry into the hash table"},
  {EXTERNAL, "Usage", "Program usage: ./assembler <filename1.as> <filename2.as>..."},
  {EXTERNAL, "Saved word", "Can't use saved words as arguments"},
  {EXTERNAL, "Extranous text", "Extranous text at the end of a command"},
  {EXTERNAL, "Label definition", "Faulty label definition - can only be named with letters"},
  {EXTERNAL, "Unkown command", "Encountered an unkown command"},
  {EXTERNAL, "Line length", "Lines should be at a maximum length of 80"},
  {EXTERNAL, },
  {NULL, NULL, NULL};
}

void print_error(char *name, char *additional_arg, int line_number){
  int i = 0;
  while(errors[i].name != NULL){
    if(strcmp(errors[i].name, name) == 0){
      printf("[!] ERROR: %s %s on line %d", errors[i].description, additional_arg, line_number);
      return;
    }
    i++;
  }
  printf("[!] ERROR: %s %s on line %d", );
}
