#ifndef VALIDATE_H
#define VALIDATE_H

#include "../Headers/translate.h"

typedef struct{
  int destination_op;
  int source_op;
} addressModes;

int valid_length_line(char *line);

int is_saved_word(char *str);

int valid_label(char *tok);

int is_valid_command(int command_start, char *tokens[MAX_LINE_LENGTH]);

int check_operands(int command_start, char *tokens[MAX_LINE_LENGTH], int correct_operands, addressModes *operands);

int is_reg(char *word);

#endif
