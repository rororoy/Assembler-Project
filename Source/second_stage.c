#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/second_stage.h"
#include "../Headers/global.h"
#include "../Headers/hash_table.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"
#include "../Headers/utils.h"
#include "../Headers/error.h"

int second_pass(char *filename, hashTable *pending_labels, transTable *translation_table, symbolTable *symbol_table) {
  int i;
  hashBucket *pending_entry;
  symbol *symbol_entry;
  int IS_RELATIVE_LABEL = 0;
  char *label;

  /* Loop through all pending_labels and resolve them */
  for (i = 0; i < pending_labels->size; i++) {
    pending_entry = &(pending_labels->bucket[i]);

    if(!pending_entry->is_taken) continue;

    if(pending_entry->label_name[0] == '&'){
      label = pending_entry->label_name + 1;
      IS_RELATIVE_LABEL = 1;
    }else{
      label = pending_entry->label_name;
      IS_RELATIVE_LABEL = 0;
    }

    printf("Looking at: %s\n\n", label);
    printf("| %07d  | %-30s | \n\n\n", translation_table[pending_entry->command_index].address, translation_table[pending_entry->command_index].source_code ? translation_table[pending_entry->command_index].source_code : "");

    symbol_entry = find_symbol(symbol_table, label);

    if(!symbol_entry){
      printf("THE PENDING LABEL WASNT FOUND IN THE SYMBOLS COULDNT RESOLVE\n");

    } else {
      /* Get the pointer to the word we need to resolbe first */
      wordNode *node_ptr = get_word_at_index(translation_table[pending_entry->command_index].node, pending_entry->word_number);

      if(!node_ptr) {
        printf("DIDNT FIND WORD\n");
      } else {
        /* Access the data field through the pointer */
        word current_word = node_ptr->data;
        printf("FOUND WORD:\n");
        print_word_binary(current_word);

        /* RESOLVING PENDING PLACEHOLDER WORDS: */
        /* If the ARE = ARE_NONE (000) THEN CHECK SYMBOL TABLE FOR THE CONTEXT */
        /* If the ARE = ARE_E (000) NOT POSSIBLE ELSE WE WOULDNT PLACE A PLACE HOLDER */
        /* If the ARE = ARE_R (010) THIS IS AN ENTRY WE SAY AND DIDNT RESOLVE - TAKE ADDRESS FROM SYMBOL TABLE */
        /* IF THE ARE = ARE_A ()  */

        if(!resolve_word(pending_entry, translation_table, symbol_entry, node_ptr, IS_RELATIVE_LABEL)){
          printf("FAILED TO RESOLVE\n");
          return 0;
        }

      }
    }
  }
  create_output_files(filename, translation_table, symbol_table);
  return 1;
}


int resolve_word(hashBucket *pending_entry, transTable *translation_table, symbol *symbol_entry, wordNode *node_ptr, int IS_RELATIVE_LABEL){
  int symbol_IC = symbol_entry->address;
  int command_IC = translation_table[pending_entry->command_index].address;
  word current_word = node_ptr->data;
  labelContext context = symbol_entry->context;

  if(IS_RELATIVE_LABEL){ /* Is the addressing type - relative (type 2) unrelated to R bit */
    update_word(node_ptr, symbol_IC - command_IC , A);
  }

  else if(context == CONTEXT_EXTERN){
    update_word(node_ptr, 0 , E);

  }else if(context == CONTEXT_ENTRY){
    update_word(node_ptr, symbol_IC , R);
  }else{ /* Normal */
    update_word(node_ptr, symbol_IC , R);
  }

  return 1;
}

/**
 * Creates the output files (.ob, .ext, .ent) for the assembled program.
 *
 * @param filename Base filename for output files.
 * @param translation_table The translation table containing the assembled code.
 * @param symbol_table The symbol table for resolving labels.
 * @return 1 if successful, 0 if an error occurred.
 */
int create_output_files(char *filename, transTable *translation_table, symbolTable *symbol_table) {
    FILE *file;
    char *ob_file = append_extension(filename, ".ob");
    char *ext_file = append_extension(filename, ".ext");
    char *ent_file = append_extension(filename, ".ent");
    int success = 1;

    /* Open the ob file for writing */
    file = fopen(ob_file, "w");
    if (file == NULL) {
        print_error("File write", filename, 0);
        free(ob_file);
        free(ext_file);
        free(ent_file);
        return 0;
    }

    /* Generate the object file */
    if (!generate_ob_file(file, translation_table)) {
        success = 0;
    }

    fclose(file);

    /* Will implement ext and ent file generation later */

    free(ob_file);
    free(ext_file);
    free(ent_file);

    return success;
}

/**
 * Generates the object (.ob) file from the translation table.
 * The file format is: IC and DC on first line, followed by addresses and hex words.
 *
 * @param file The output file pointer.
 * @param translation_table The translation table containing the assembled code.
 * @return 1 if successful, 0 if an error occurred.
 */
 /**
  * Generates the object (.ob) file from the translation table.
  * The file format is: IC and DC on first line, followed by addresses and hex words.
  *
  * @param file The output file pointer.
  * @param translation_table The translation table containing the assembled code.
  * @return 1 if successful, 0 if an error occurred.
  */
  /**
  * Converts a word to a hexadecimal string based on its type
  * (determined by position in the linked list and source code).
  *
  * @param word_data The word to convert
  * @param is_first_word Flag indicating if this is the first word in a linked list
  * @param is_data_entry Flag indicating if this is a data entry
  * @param hex_str The output buffer for the hex string (at least 7 bytes)
  */
 void word_to_hex_by_type(word word_data, int is_first_word, int is_data_entry, char *hex_str) {
     unsigned int value = 0;
     int i;

     if (is_data_entry) {
         /* For data entries, all words use the data_word format */
         value = word_data.data_word.data & 0xFFFFFF; /* Mask to 24 bits */
     } else if (is_first_word) {
         /* For first word in a non-data entry, use instruction format */
         value = (word_data.instruction.opcode << 18) |
                 (word_data.instruction.src_mode << 16) |
                 (word_data.instruction.src_reg << 13) |
                 (word_data.instruction.dst_mode << 11) |
                 (word_data.instruction.dst_reg << 8) |
                 (word_data.instruction.funct << 3) |
                 (word_data.instruction.a << 2) |
                 (word_data.instruction.r << 1) |
                 (word_data.instruction.e);
     } else {
         /* For subsequent words in a non-data entry, use extra_word format */
         value = (word_data.extra_word.value << 3) |
                 (word_data.extra_word.a << 2) |
                 (word_data.extra_word.r << 1) |
                 (word_data.extra_word.e);
     }

     /* Convert to lowercase hex string */
     snprintf(hex_str, 7, "%06x", value);

     /* Ensure all characters are lowercase */
     for (i = 0; hex_str[i]; i++) {
         hex_str[i] = tolower(hex_str[i]);
     }
 }

 /**
  * Generates the object (.ob) file from the translation table.
  * This implementation follows the same approach as print_complete_transTable.
  *
  * @param file The output file pointer.
  * @param translation_table The translation table containing the assembled code.
  * @return 1 if successful, 0 if an error occurred.
  */
 int generate_ob_file(FILE *file, transTable *translation_table) {
     int i;
     wordNode *current_node;
     int word_index;
     char hex_str[7]; /* 6 hex chars + null terminator */
     int is_data_entry;
     int IC = 25; /* Placeholder value for IC */
     int DC = 9;  /* Placeholder value for DC */

     /* Write the IC and DC values at the top of the file */
     fprintf(file, "%d %d\n", IC, DC);

     /* Iterate through the translation table */
     for (i = 0; translation_table[i].node != NULL; i++) {
         current_node = translation_table[i].node;

         /* Skip entries with no nodes */
         if (current_node == NULL) {
             continue;
         }

         /* Check if this is a command or data entry by looking at the source code */
         is_data_entry = (translation_table[i].source_code &&
                         (strstr(translation_table[i].source_code, ".data") ||
                          strstr(translation_table[i].source_code, ".string")));

         /* Iterate through the linked list of wordNodes */
         word_index = 0;
         while (current_node != NULL) {
             /* Calculate the address for this word (base address + index) */
             int word_address = translation_table[i].address + word_index;

             /* Convert the word to hex based on its type */
             word_to_hex_by_type(current_node->data, word_index == 0, is_data_entry, hex_str);

             fprintf(file, "%07d %s\n", word_address, hex_str);

             /* Move to the next node and increment the word index */
             current_node = current_node->next;
             word_index++;
         }
     }

     return 1;
 }

/**
 * Wrapper function to test word_to_hex on a small set of example words
 * to check if the conversion works as expected.
 */
int generate_ext_file(FILE *file, symbolTable *symbol_table){
  return 1;
}

int generate_ent_file(FILE *file, symbolTable *symbol_table){
  return 1;
}
