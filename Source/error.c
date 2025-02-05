#include <stdio.h>
#include <string.h>
#include "../Headers/error.h"

error errors[] = {
  {INTERNAL, "File write", "Error opening file for writing"},
  {INTERNAL, "File read", "Error opening file for reading"},
  {INTERNAL, "Missing argument", "Missing arguents in function"},
  {INTERNAL, "Malloc", "Failed on memory alocation"},
  {INTERNAL, "Insert hash", "Encountered error inserting entry into the hash table"},
  {INTERNAL, "Failed writing", "Failed writing to file"},
  {INTERNAL, "Create hashtable", "Failed creating a new hashtable"},
  {EXTERNAL, "Usage", "Program usage: ./assembler <filename1.as> <filename2.as>..."},
  {EXTERNAL, "Saved word", "Can't use saved words as arguments"},
  {EXTERNAL, "Extranous text", "Extranous text at the end of a command"},
  {EXTERNAL, "Label definition", "Faulty label definition - can only be named with letters"},
  {EXTERNAL, "Unkown command", "Encountered an unkown command"},
  {EXTERNAL, "Line length", "Lines should be at a maximum length of 80"},
  {EXTERNAL, "No macro", "No macro provided after macro definition"},
  {EXTERNAL, "Macro reached EOF", "Reached the end of the file without closing the macro block"},
  {EXTERNAL, NULL, NULL}
};

void print_error(char *name, char *additional_arg, int line_number){
  int i = 0;
  while(errors[i].name != NULL){
    if(strcmp(errors[i].name, name) == 0){
      printf("[!] ERROR: %s %s on line %d\n", errors[i].description, additional_arg, line_number);
      return;
    }
    i++;
  }
  printf("[!] ERROR: Unkown error encountered on line %d\n", line_number);
}
