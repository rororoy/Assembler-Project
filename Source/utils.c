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
  Recieves a line from the assembly file and checks if it falls withing the 80
  chars length limit
  @return 0 if the line length is too long return 1 if valid
*/
int valid_length_line(char *line){
  return(strlen(line) <= MAX_LINE_LENGTH);
}
