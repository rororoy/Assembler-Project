#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/first_stage.h"
#include "../Headers/second_stage.h"
#include "../Headers/error.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"
#include "../Headers/validate.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"
#include "../Headers/hash_table.h"

int first_pass(char *filename, hashTable *macro_table) {
  FILE *file;
  int i;
  char line[MAX_LINE_LENGTH + 2]; /* Buffer for a line: MAX_LINE_LENGTH + '\n' + '\0' */
  char *tokens[MAX_LINE_LENGTH];
  int tokens_mode;
  addressModes operands_adress;
  commandSem *cmnd;
  hashBucket *ht_bucket;
  /* Define the addressing type - 0 IMM, 1 DIRECT, 2 RELATIVE, 3 IMM REG, -1 ERR */
  int addressing_mode;
  char *am_file = append_extension(filename, ".am");
  int DC = 0, IC = 100, prev_DC = 0;
  int command_start = 0;
  int tablepointer = 0;
  int IGNORE_LABEL = 0; /* Flag used when a label was already defined and should be ignored */

  symbolTable *symbol_table = create_symbol_table();
  transTable *translation_table = create_transTable(50);
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

    if(!addressing_mode){
      continue;
    }

    printf("%d\n", addressing_mode);
    process_assembly_command(pending_labels, translation_table, &tablepointer, tokens, IC+DC, operands_adress.source_op, operands_adress.destination_op, command_start, symbol_table);


    /* Get command semantics to avoid repetitive string comparisons */
    cmnd = command_lookup(tokens[command_start]);

    /* If encountered a label definition at the start of the line */
    if (tokens_mode == 2) {
      /* First check if the label was already defined somewhere */
      symbol *symbol_entry = find_symbol(symbol_table, tokens[0]);
      if(symbol_entry != NULL){
        IGNORE_LABEL = 1;
        if(symbol_entry->address == -1){
          update_symbol_address(symbol_table, tokens[0], IC+DC, -1);
        }else if(symbol_entry->address == 0){
          print_error("Label already seen", "as external", LINE_NUMBER);
        }else{
          print_error("Label already seen", "", LINE_NUMBER);
        }

      }

      /* Check if the label is a macro name */
      ht_bucket = search_table(macro_table, tokens[0]);
      if (ht_bucket != NULL) {
        print_error("Label macro name", tokens[0], LINE_NUMBER);
      }

      /* Handle label insertion based on directive type */
      if (cmnd->name == CMD_DATA || cmnd->name == CMD_STRING) {
        /* Case of .data and .string - special DC treatment is needed */
        if (!IGNORE_LABEL && !insert_symbol(symbol_table, tokens[0], IC+DC, LBL_DATA, CONTEXT_NORMAL)) {
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }
      } else {
        if (!IGNORE_LABEL && !insert_symbol(symbol_table, tokens[0], IC+DC, LBL_CODE, CONTEXT_NORMAL)) {
          printf("ERROR INSERTING %s", tokens[0]);
          return 0;
        }
        IC++;
      }
    } else {
      /* Case of any other regular commands without a label defined */
      if (cmnd->name != CMD_EXTERN && cmnd->name != CMD_ENTRY) {
        /* Commands other than .extern and .entry increment IC */
        if (cmnd->name != CMD_DATA && cmnd->name != CMD_STRING) {
          IC++;
        }
      }
    }

    /* Calculate data size for directives - consolidated for both label and non-label cases */
    if (cmnd->name == CMD_STRING) {
      prev_DC = strlen(tokens[command_start+1]) + 1;
    } else if (cmnd->name == CMD_DATA) {
      prev_DC = addressing_mode - 1;
    }

    /* Update counters if not extern or entry */
    if (cmnd->name != CMD_EXTERN && cmnd->name != CMD_ENTRY) {
      IC += (operands_adress.destination_op != 3 && operands_adress.destination_op != -1) ? 1 : 0;
      IC += (operands_adress.source_op != 3 && operands_adress.source_op != -1) ? 1 : 0;
      DC += prev_DC;
    }
    prev_DC = 0;
    IGNORE_LABEL = 0;

    printf("[[[[[[[[CHECK: DEST:%d, SRC:%d IC:%d, DC:%d}}}}}}}}}}\n", operands_adress.destination_op, operands_adress.source_op, IC, DC);
    print_complete_transTable(translation_table, tablepointer); /* Print the table so far */

    printf("\n");
  }

  /* Print the table */
  printf("TransTable contents:\n");
  print_complete_transTable(translation_table, tablepointer); /* Print just the first entry */

  /* Free the allocated memory */
  printf("\n\nSYMBOL TABLE \n\n\n");
  print_symbol_table(symbol_table);


  printf("\n\n\nPENDING TABLE \n");
  print_pending_labels(pending_labels);

  printf("\n\n ERRORS IN LABEL DEFINITONS: \n\n");

  if(is_missing_symbols(symbol_table)){
    print_error("Label missing", "", 0);
  }else{
    printf("No undefined labels found\n");
  }

  printf("\nDC:%d\nICF:%d\n\n", DC, IC-100);

  /***************        Second assembler stage            *******************/
  printf("[*] Starting the second assembler stage on %s\n", filename);
  second_pass(filename, pending_labels, translation_table, symbol_table, IC, DC);

  /* Print the table */
  printf("\n\nTransTable contents:\n");
  print_complete_transTable(translation_table, tablepointer); /* Print just the first entry */

  free_transTable(translation_table, tablepointer);
  return 1;
}

/* Main function to process assembly commands */
void process_assembly_command(hashTable *pending_labels, transTable *translation_table, int *tablepointer,
                              char **tokens, int IC, int operand_src_type, int operand_dst_type,
                              int command_start, symbolTable *symbol_table) {
    int src_reg = 0;
    int dst_reg = 0;
    char *source_line;
    commandSem *cmnd;
    int opcode;
    int funct;

    /* Get command information */
    cmnd = command_lookup(tokens[command_start]);
    opcode = cmnd->op_code == -1 ? 0 : cmnd->op_code;
    funct = cmnd->funct == -1 ? 0 : cmnd->funct;

    /* Join tokens to create the source line */
    source_line = join_tokens(tokens);
    if (source_line == NULL) {
        fprintf(stderr, "Memory allocation failed for source line\n");
        return;
    }

    /* This is the case for .data, .string .extern, .entry only */
    if (cmnd->op_code == -1 && cmnd->funct == -1) {
        process_directive(pending_labels, translation_table, tablepointer, tokens, IC,
                          command_start, symbol_table, source_line);
        free(source_line);
        return;
    }

    /* Determine register numbers if register addressing is used */
    if (operand_src_type == 3) { /* Register addressing for src */
        src_reg = get_register_number(tokens[command_start + 1]);
    }

    if (operand_dst_type == 3) { /* Register addressing for destination */
        if (tokens[command_start + 2] == NULL) { /* This is a case of a type 2 address */
            dst_reg = get_register_number(tokens[command_start + 1]);
        } else {
            dst_reg = get_register_number(tokens[command_start + 2]);
        }
    }

    printf("PUTTING THE COMMAND IN THE TABLE AT INDEX [%d]\n", *tablepointer);

    /* Insert the main instruction word */
    insert_command_entry(translation_table, *tablepointer, IC, source_line,
                         opcode, operand_src_type, src_reg,
                         operand_dst_type, dst_reg, funct);

    /* Process source operand extra word if needed */
    if (operand_src_type == 0) { /* Immediate addressing */
        process_immediate_addressing(translation_table, *tablepointer, IC, tokens,
                                     command_start, 1, source_line);
    } else if (operand_src_type == 1) { /* Direct addressing */
        process_direct_addressing(pending_labels, translation_table, *tablepointer, IC, tokens,
                                  command_start, 1, symbol_table, cmnd, source_line, operand_src_type);
    } else if (operand_src_type == 2) { /* Relative addressing */
        process_relative_addressing(pending_labels, translation_table, *tablepointer, IC, tokens,
                                    command_start, 1, symbol_table, cmnd, source_line, operand_src_type);
    }

    /* Process destination operand extra word if needed */
    if (operand_dst_type == 0) { /* Immediate addressing */
        process_immediate_addressing(translation_table, *tablepointer, IC, tokens,
                                     command_start, 0, source_line);
    } else if (operand_dst_type == 1) { /* Direct addressing */
        process_direct_addressing(pending_labels, translation_table, *tablepointer, IC, tokens,
                                  command_start, 0, symbol_table, cmnd, source_line, operand_src_type);
    } else if (operand_dst_type == 2) { /* Relative addressing */
        process_relative_addressing(pending_labels, translation_table, *tablepointer, IC, tokens,
                                    command_start, 0, symbol_table, cmnd, source_line, operand_src_type);
    }

    /* Increment table pointer */
    (*tablepointer)++;

    /* Free the source line memory */
    free(source_line);
}

/* Helper function to process directive commands (.data, .string, .extern, .entry) */
 void process_directive(hashTable *pending_labels, transTable *translation_table, int *tablepointer,
                             char **tokens, int IC, int command_start, symbolTable *symbol_table,
                             char *source_line) {
    /* Handle .data directive */
    if (strcmp(tokens[command_start], ".data") == 0) {
        int i = command_start + 1;

        while (tokens[i] != NULL) {
            insert_extra_word(translation_table, *tablepointer, IC, source_line, 4, atoi(tokens[i]), ARE_NONE);
            i++;
        }
    }
    /* Handle .string directive */
    else if (strcmp(tokens[command_start], ".string") == 0) {
        int i = 0;

        while (tokens[command_start+1][i] != '\0') {
            insert_extra_word(translation_table, *tablepointer, IC, source_line, 4, tokens[command_start+1][i], ARE_NONE);
            i++;
        }
        /* For '\0' */
        insert_extra_word(translation_table, *tablepointer, IC, source_line, 4, tokens[command_start+1][i], ARE_NONE);
    }
    /* Handle .extern or .entry directive */
    else {
        char *lbl = tokens[command_start+1];
        symbol *symbol_entry = find_symbol(symbol_table, lbl);

        if (symbol_entry == NULL) { /* Not logged yet in the symbol table */
            if (strcmp(tokens[command_start], ".extern") == 0) {
                if (!insert_symbol(symbol_table, lbl, 0, LBL_CODE, CONTEXT_EXTERN)) {
                    printf("ERROR INSERTING EXTERN %s", lbl);
                }
            }
            else { /* .entry */
                /* insert with -1 as flag because label was not yet seen */
                if (!insert_symbol(symbol_table, lbl, -1, LBL_CODE, CONTEXT_ENTRY)) {
                    printf("ERROR INSERTING ENTRY %s", lbl);
                }
            }
        }
        else { /* Was already declared previously */
            if (strcmp(tokens[command_start], ".extern") == 0) {
                if (symbol_entry->address == -1) {
                    print_error("Label type error", "in an entry command as local", LINE_NUMBER);
                }
                else if (symbol_entry->address != 0) {
                    print_error("Label already seen", "localy in this file", LINE_NUMBER);
                }
            }
            else if (strcmp(tokens[command_start], ".entry") == 0) {
                if (symbol_entry->address == 0) {
                    print_error("Label type error", "in an extern command as external", LINE_NUMBER);
                }
                else {
                    /* Since it was mentioned in an entry once - tag symbol as entry */
                    symbol_entry->context = CONTEXT_ENTRY;
                }
            }
        }
        return;
    }

    (*tablepointer)++;
}

/* Helper function to process immediate addressing mode (#value) */
 void process_immediate_addressing(transTable *translation_table, int tablepointer, int IC,
                                         char **tokens, int command_start, int is_source,
                                         char *source_line) {
    int value;

    if (is_source || tokens[command_start + 2] == NULL) {
        /* Source operand or type 2 command with single operand */
        value = atoi(&tokens[command_start + 1][1]); /* Skip the '#' character */
    } else {
        /* Destination operand */
        value = atoi(&tokens[command_start + 2][1]); /* Skip the '#' character */
    }

    insert_extra_word(translation_table, tablepointer, IC, source_line, 0, value, A);
}

/* Helper function to process direct addressing mode (label) */
 void process_direct_addressing(hashTable *pending_labels, transTable *translation_table,
                                      int tablepointer, int IC, char **tokens, int command_start,
                                      int is_source, symbolTable *symbol_table, commandSem *cmnd,
                                      char *source_line, int operand_src_type) {
    char *label;
    symbol *symbol_entry;
    int word_place;

    /* Determine which token contains the label */
    if (is_source) {
        label = tokens[command_start + 1];
    } else {
        if (cmnd->type == 1 && tokens[command_start + 2] != NULL) {
            label = tokens[command_start + 2];
        } else {
            label = tokens[command_start + 1];
        }
    }

    /* Calculate the correct word position using our helper function */
    word_place = calculate_word_position(is_source, cmnd, operand_src_type);

    symbol_entry = find_symbol(symbol_table, label);

    if (symbol_entry != NULL) {
        int address = symbol_entry->address;

        if (address == -1) {
            /* Label declared but address missing */
            insert_extra_word(translation_table, tablepointer, IC, source_line, 0, -1, R);
            handle_undefined_label(pending_labels, label, tablepointer, word_place);
        } else {
            /* Label found with valid address */
            if (symbol_entry->context == CONTEXT_EXTERN) {
                insert_extra_word(translation_table, tablepointer, IC, source_line, 0, address, E);
            } else {
                insert_extra_word(translation_table, tablepointer, IC, source_line, 0, address, R);
            }
        }
    } else {
        /* Label not yet encountered - put placeholder in table */
        insert_extra_word(translation_table, tablepointer, IC, source_line, 0, -1, ARE_NONE);
        handle_undefined_label(pending_labels, label, tablepointer, word_place);
    }
}

/* Helper function to process relative addressing mode (&label) */
 void process_relative_addressing(hashTable *pending_labels, transTable *translation_table,
                                        int tablepointer, int IC, char **tokens, int command_start,
                                        int is_source, symbolTable *symbol_table, commandSem *cmnd,
                                        char *source_line, int operand_src_type) {
    char *label;
    symbol *symbol_entry;
    int word_place;

    /* Determine which token contains the label */
    if (is_source) {
        label = tokens[command_start + 1];
    } else {
        if (cmnd->type == 1 && tokens[command_start + 2] != NULL) {
            label = tokens[command_start + 2];
        } else {
            label = tokens[command_start + 1];
        }
    }

    /* Calculate the correct word position */
    word_place = calculate_word_position(is_source, cmnd, operand_src_type);

    symbol_entry = find_symbol(symbol_table, label + 1); /* Skip the '&' character */

    if (symbol_entry != NULL) {
      if(symbol_entry->address == 0){ /* If the label is a delcared .extern type */
        insert_extra_word(translation_table, tablepointer, IC, source_line, 0, 0, E);
      }else if(symbol_entry->address == -1){ /* If the label is a delcared .entry type */
        insert_extra_word(translation_table, tablepointer, IC, source_line, 0, -1, A);
      }else{
        /* In the other case that the label was seen already and has address */
        int relative_jump = symbol_entry->address - IC;
        insert_extra_word(translation_table, tablepointer, IC, source_line, 0, relative_jump, A);
      }

    } else {
      /* In the case that the label wasn't yet seen */
      insert_extra_word(translation_table, tablepointer, IC, source_line, 0, -1, ARE_NONE);
      handle_undefined_label(pending_labels, label, tablepointer, word_place);
    }
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
