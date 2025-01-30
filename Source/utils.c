#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/error.h"

void check_malloc(void *ptr){
  if(ptr == NULL){
    print_error(1);
  }
}

/*
  @return 0 if non-empty line, return 1 if empty line
*/
int empty_line(char *line){
  if(line == NULL){
    return 1;
  }

  while(*str)
}

/*
  Recieves a line from the assembly file and checks if it falls withing the 80
  chars length limit
  @return 0 if the line length is too long return 1 if valid
*/
int valid_length_line(char *line){
  return(strlen(line) <= MAX_LINE_LENGTH);
}
