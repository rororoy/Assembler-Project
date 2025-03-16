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
#include "../Headers/hash_table.h"

int first_pass(char *filename) {
  FILE *file;
  int i;
  char line[MAX_LINE_LENGTH + 2]; /* Buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
  char *tokens[MAX_LINE_LENGTH];
  int tokens_mode;
  addressModes operands_adress;
  /* Define the addressing type - 0 IMM, 1 DIRECT, 2 RELATIVE, 3 IMM REG, -1 ERR */
  int addressing_mode;
  char *am_file = append_extension(filename, ".am");
  int DC = 0, IC = 100, prev_DC = 0;
  int command_start = 0;
  int tablepointer = 0;

  symbolTable *symbol_table = create_symbol_table();
  transTable *my_table = create_transTable(50);
  hashTable *pending_labels = make_hash_table(HASH_TABLE_INITIAL_SIZE);

  LINE_NUMBER = 0; /* Zero the global variable */

  if (symbol_table == NULL) {
    printf("Failed to create symbol table\n");
    return 0;
  }

  /* Create a hash table for pending labels */
  if (pending_labels == NULL) {
    printf("Failed to create pending hash table\n");
    return 0;
  }

  /* Open the am file */
  file = fopen(am_file, "r");
  if (file == NULL) {
    print_error("File read", filename, 0);
    return 0;
  }

  /* Loop through the line checking for different cases */
  while (fgets(line, sizeof(line), file) != NULL) {
    LINE_NUMBER++;
    if (!(tokens_mode = tokanize_line(line, tokens, 0))){
      return 0;
    }

    /* Printing of tokanization */
    printf("Tokanized-->");
    for (i = 0; i < MAX_LINE_LENGTH; i++) {
        if (tokens[i] == NULL) {
            break;
        }
        printf("%s|", tokens[i]);
    }
    printf("\n");



    /* If encountered a decleration of a label in the line */
    command_start = tokens_mode == 2 ? 1 : 0;
    addressing_mode = is_valid_command(command_start, tokens, &operands_adress);

    process_assembly_command(pending_labels, my_table, &tablepointer, tokens, IC+DC, operands_adress.source_op, operands_adress.destination_op, command_start, symbol_table);

    /* If ecnountered a label definition at the start of the line */
    if (tokens_mode == 2) {
      if ((tokens[command_start] != NULL) && (strcmp(tokens[command_start], ".data") == 0 || strcmp(tokens[command_start], ".string") == 0)) {
        /* Incase of .data and .string - special DC treatment is needed */
        if (!insert_symbol(symbol_table, tokens[0], IC+DC, LBL_DATA)) {
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }

        if (strcmp(tokens[command_start], ".string") == 0) {
          prev_DC = strlen(tokens[command_start+1]) + 1;
        } else { /* .data */
          /* Count reserved space in memory for each data type declared */
          prev_DC = addressing_mode - 1;
        }

      } else {
        if (!insert_symbol(symbol_table, tokens[0], IC+DC, LBL_CODE)) {
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }
        IC++;
      }
    }else{
      /* Incase of any other regular commands without a label defined */
      if (strcmp(tokens[command_start], ".string") == 0) {
        prev_DC = strlen(tokens[command_start+1]) + 1;

      } else if (strcmp(tokens[command_start], ".data") == 0){ /* .data */
        /* Count reserved space in memory for each data type declared */
        prev_DC = addressing_mode - 1;

      }else{
        /* Count the command with IC */
        IC++;
      }
    }


    IC += (operands_adress.destination_op != 3 && operands_adress.destination_op != -1) ? 1 : 0;
    IC += (operands_adress.source_op != 3 && operands_adress.source_op != -1) ? 1 : 0;
    DC += prev_DC;
    prev_DC = 0;

    printf("[[[[[[[[CHECK: DEST:%d, SRC:%d IC:%d, DC:%d}}}}}}}}}}\n", operands_adress.destination_op, operands_adress.source_op, IC, DC);
    print_complete_transTable(my_table, tablepointer); /* Print just the first entry */

    printf("\n");
  }

  /* Print the table */
  printf("TransTable contents:\n");
  print_complete_transTable(my_table, tablepointer); /* Print just the first entry */

  /* Free the allocated memory */
  free_transTable(my_table, tablepointer);
  printf("\n\nSYMBOL TABLE \n\n\n");
  print_symbol_table(symbol_table);


  printf("\n\n\nPENDING TABLE \n");
  print_pending_labels(pending_labels);
  return 1;
}

void process_assembly_command(hashTable *pending_labels, transTable *my_table, int *tablepointer, char **tokens, int IC,
                               int operand_src_type, int operand_dst_type, int command_start, symbolTable *symbol_table) {
    int src_reg = 0;
    int dst_reg = 0;
    char *source_line;
    commandSem *cmnd = command_lookup(tokens[command_start]);
    int opcode = cmnd->op_code == -1 ? 0 : cmnd->op_code;
    int funct = cmnd->funct == -1 ? 0 : cmnd->funct;


    /* Define the opcode and funct - if dont exist - put 0 */

    /* Join tokens to create the source line */
    source_line = join_tokens(tokens);
    if (source_line == NULL) {
        fprintf(stderr, "Memory allocation failed for source line\n");
        return;
    }

    /* This is the case for .data, .string .extern. entry only */
    if(cmnd->op_code == -1 && cmnd->funct == -1){

      /* Loop through all of the the data components of the commands and insert */
      if(strcmp(tokens[command_start], ".data") == 0){
        int i = command_start + 1;

        while(tokens[i] != NULL){
          insert_extra_word(my_table, *tablepointer, IC, source_line, 4, atoi(tokens[i]));
          i++;
        }
      }else if(strcmp(tokens[command_start], ".string") == 0){
        int i = 0;
        while(tokens[command_start+1][i] != '\0'){
          insert_extra_word(my_table, *tablepointer, IC, source_line, 4, tokens[command_start+1][i]);
          i++;
        }
        insert_extra_word(my_table, *tablepointer, IC, source_line, 4, tokens[command_start+1][i]); /* For '\0' */
      }
      (*tablepointer)++;
      free(source_line);
      return;
    }

    /* Determine register numbers if register addressing is used */
    if (operand_src_type == 3) { /* Register addressing for src */
        /* Extract register number - assuming format like "r3" */
        if (tokens[command_start + 1] != NULL && tokens[command_start + 1][0] == 'r') {
            src_reg = tokens[command_start + 1][1] - '0';
        }
    }

    if (operand_dst_type == 3) { /* Register addressing for destination */
        /* Extract register number - assuming format like "r3" */
        if(tokens[command_start + 2] == NULL){ /* This is a case of a type 2 address */
          dst_reg = tokens[command_start + 1][1] - '0';
        }else if (tokens[command_start + 2] != NULL && tokens[command_start + 2][0] == 'r') {
          dst_reg = tokens[command_start + 2][1] - '0';
        }
    }

    printf("PUTTING THE COMMAND IN THE TABLE AT INDEX [%d]\n", *tablepointer);
    /* Insert the main instruction word */
    insert_command_entry(my_table, *tablepointer, IC, source_line,
      opcode, operand_src_type, src_reg,
      operand_dst_type, dst_reg, funct);


    /* Handle extra words based on addressing types */
    /* Process source operand extra word if needed */
    if (operand_src_type == 0) { /* Immediate addressing (#value) */
        /* Extract the immediate value, assuming format like "#123" */
        if (tokens[command_start + 1] != NULL) {
          int value = atoi(&tokens[command_start + 1][1]); /* Skip the '#' character */
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, value);
        }
    }
    else if (operand_src_type == 1) { /* Direct addressing (variable name) */
        /* Here you would look up the address of the label in your symbol table */
        char *src_lbl = tokens[command_start+1];
        symbol *symbol_entry = find_symbol(symbol_table, src_lbl);

        if(symbol_entry != NULL){
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, symbol_entry->address);
        }else{
          int word_place = 2;
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, -1);
          handle_undefined_label(pending_labels, src_lbl, *tablepointer, word_place);

        }
    }
    else if (operand_src_type == 2) { /* Relative addressing (&label) */
        /* Calculate relative distance - would require symbol table lookup */
        char *src_lbl = tokens[command_start+1];
        symbol *symbol_entry = find_symbol(symbol_table, src_lbl+1);

        if(symbol_entry != NULL){
          int relative_jump = symbol_entry->address - IC;
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, relative_jump);
        }else{
          int word_place = 2;
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, -1);
          handle_undefined_label(pending_labels, src_lbl, *tablepointer, word_place);
        }
    }

    /* Process destination operand extra word if needed */
    if (operand_dst_type == 0) { /* Immediate addressing */
      int value;
      /* If no 2nd operand but des is defnied then this is a type 2 command */
      if(tokens[command_start + 2] == NULL){
        value = atoi(&tokens[command_start + 1][1]); /* Skip the '#' character */
      }
      else if (tokens[command_start + 2] != NULL) {
        value = atoi(&tokens[command_start + 2][1]); /* Skip the '#' character */
      }
      insert_extra_word(my_table, *tablepointer, IC, source_line, 0, value);

    }

    else if (operand_dst_type == 1) { /* Direct addressing */
        /* For now, using placeholder value */
        char *dest_lbl = command_lookup(tokens[command_start])->type == 1 ? tokens[command_start+2] : tokens[command_start+1];
        symbol *symbol_entry = find_symbol(symbol_table, dest_lbl);


        if(symbol_entry != NULL){
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, symbol_entry->address);
        }else{
          int word_place = command_lookup(tokens[command_start])->type == 1 ? 3 : 2;
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, -1);
          handle_undefined_label(pending_labels, dest_lbl, *tablepointer, word_place);
        }


    }
    else if (operand_dst_type == 2) { /* Relative addressing */
        /* For now, using placeholder value */
        /* Here you would look up the address of the label in your symbol table */
        char *dest_lbl = command_lookup(tokens[command_start])->type == 1 ? tokens[command_start+2] : tokens[command_start+1];
        symbol *symbol_entry = find_symbol(symbol_table, dest_lbl+1);

        if(symbol_entry != NULL){
          int relative_jump = symbol_entry->address - IC;
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, relative_jump);
        }else{
          int word_place = command_lookup(tokens[command_start])->type == 1 ? 3 : 2;
          insert_extra_word(my_table, *tablepointer, IC, source_line, 0, -1);
          handle_undefined_label(pending_labels, dest_lbl, *tablepointer, word_place);
        }
    }

    /* Increment table pointer and update IC based on words used */
    (*tablepointer)++;

    /* Free the source line memory */
    free(source_line);
}

int handle_undefined_label(hashTable *pending_labels, char *label_name, int current_command_index, int word_position) {
    hashBucket *result;

    /* Validate parameters */
    if (pending_labels == NULL || label_name == NULL) {
        print_error("Invalid parameters", "handle_undefined_label", LINE_NUMBER);
        return 0;
    }

    /* Add the pending label to our hash table without modifying the label */
    result = insert_pending_label(pending_labels, label_name, current_command_index, word_position);

    if (result == NULL) {
      print_error("Failed to insert pending label", label_name, LINE_NUMBER);
      return 0;
    }else{
      printf("ADDED %s to pending table\n", label_name);
    }

    return 1; /* Success */
}
