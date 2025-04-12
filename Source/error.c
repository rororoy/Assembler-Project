#include <stdio.h>
#include <string.h>
#include "../Headers/error.h"
#include "../Headers/global.h"

error errors[] = {
  {INTERNAL, "File write", "Error opening file for writing on"},
  {INTERNAL, "File read", "Error opening file for reading on"},
  {INTERNAL, "Missing argument", "Missing arguents in function"},
  {INTERNAL, "Malloc", "Failed on memory alocation"},
  {INTERNAL, "Insert hash", "Encountered error inserting entry into the hash table"},
  {INTERNAL, "Failed writing", "Failed writing to file"},
  {INTERNAL, "Create hashtable", "Failed creating a new hashtable"},
  {INTERNAL, "Failed second pass", "Failed on the second assembler stage of"},
  {INTERNAL, "Failed creating structure", "Failed internally creating a strcture of type"},
  {INTERNAL, "Failed inserting", "Failed internally inserting data into a data structure"},
  {EXTERNAL, "Usage", "Program usage: ./assembler <filename1.as> <filename2.as>..."},
  {EXTERNAL, "Saved word", "Can't use saved words as arguments"},
  {EXTERNAL, "Extranous text", "Extranous text at the end of a command"},
  {EXTERNAL, "Label definition", "Faulty label definition - can only contain alphanumeric characters"},
  {EXTERNAL, "Unkown command", "Encountered an unkown command"},
  {EXTERNAL, "Line length", "Lines should be at a maximum length of 80"},
  {EXTERNAL, "No macro", "No macro provided after macro definition"},
  {EXTERNAL, "Macro reached EOF", "Reached the end of the file without closing the macro block"},
  {EXTERNAL, "Label not first", "A decleration of a label must be only at the begining of a line"},
  {EXTERNAL, "Label already seen", "The label was already declared previously"},
  {INTERNAL, "Label missing", "A label that was declared with an entry was never defined"},
  {EXTERNAL, "Label didnt reolve", "A label mentioned was never declared - couldn't resolve translation of the label"},
  {EXTERNAL, "Label type error", "Error in label decleration, label was already declared"},
  {EXTERNAL, "Label macro name", "Defined label can't share the same name with an already defined macro"},
  {EXTERNAL, "Unterminated string", "A decleration of a string wasn't closed with parentheses"},
  {EXTERNAL, "Too many operands", "Too many operands for the command"},
  {EXTERNAL, "Multiple ':' encountered in line", "A ':' can only be used once per line to define a label"},
  {EXTERNAL, "Missing operand between commas", "No operand was mentioned between two commas"},
  {EXTERNAL, "Invalid dig op", "Digits in a command should be preceded by a '#'"},
  {EXTERNAL, "Too few operands", "More operands expected for the command"},
  {EXTERNAL, "No number after #", "Expected a number as an operand after usage of # in"},
  {EXTERNAL, "Invalid digit in immediate operand", "Expected a number operand but got something else than a digit in"},
  {EXTERNAL, "Invalid addressing mode", "The specified addressing modes are not allowed for this command"},
  {EXTERNAL, "Unexpected operand", "Encountered an unexpected operand for a command:"},

  {EXTERNAL, NULL, NULL}
};

void print_error(char *name, char *additional_arg, int line_number){
  int i = 0;
  ERROR_ENCOUNTERED = 1;
  while(errors[i].name != NULL){
    if(strcmp(errors[i].name, name) == 0){
      printf("[!] ERROR ");

      if(errors[i].context == EXTERNAL){
        printf("[on line %d] - ", line_number);
      }

      printf("%s %s", errors[i].description, additional_arg);

      printf("\n");
      return;
    }
    i++;
  }
  printf("[!] ERROR: Unkown error encountered on line %d\n", line_number);
}
