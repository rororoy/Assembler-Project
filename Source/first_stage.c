#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/first_stage.h"
#include "../Headers/error.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"
#include "../Headers/validate.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"

node *first_pass(char *filename){
  FILE *file;
  node *labels_list = NULL;

  am_file = append_extension(filename, ".am");

  /* Open the am file */
  file = fopen(as_file, "r");
  if (file == NULL) {
    print_error("File read", filename, line_count);
    return NULL;
  }

  return labels_list;

}
