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
#include "../Headers/files.h"

int second_pass(char *filename, hashTable *pending_labels, transTable *translation_table, symbolTable *symbol_table, int IC, int DC) {
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

    /*printf("Looking at: %s\n\n", label);
    printf("| %07d  | %-30s | \n\n\n", translation_table[pending_entry->command_index].address, translation_table[pending_entry->command_index].source_code ? translation_table[pending_entry->command_index].source_code : "");
    */
    symbol_entry = find_symbol(symbol_table, label);

    if(!symbol_entry){
      print_error("Label didnt reolve", label, translation_table[pending_entry->command_index].address);

    } else {
      /* Get the pointer to the word we need to resolbe first */
      wordNode *node_ptr = get_word_at_index(translation_table[pending_entry->command_index].node, pending_entry->word_number);

      if(node_ptr){
        /* Access the data field through the pointer */
        /*print_word_binary(node_ptr->data);*/

        /* RESOLVING PENDING PLACEHOLDER WORDS: */
        /* If the ARE = ARE_NONE (000) THEN CHECK SYMBOL TABLE FOR THE CONTEXT */
        /* If the ARE = ARE_E (000) NOT POSSIBLE ELSE WE WOULDNT PLACE A PLACE HOLDER */
        /* If the ARE = ARE_R (010) THIS IS AN ENTRY WE SAY AND DIDNT RESOLVE - TAKE ADDRESS FROM SYMBOL TABLE */
        /* IF THE ARE = ARE_A ()  */

        if(!resolve_word(pending_entry, translation_table, symbol_entry, node_ptr, IS_RELATIVE_LABEL)){
          print_error("Label didnt reolve", label, translation_table[pending_entry->command_index].address);
          return 0;
        }

      }
    }
  }

  if(!ERROR_ENCOUNTERED){
    if(!create_output_files(filename, translation_table, symbol_table, pending_labels, IC, DC)){
      ERROR_ENCOUNTERED = 1;
    }
  }

  /* TODO FREE STUFF HERE */
  return !ERROR_ENCOUNTERED;
}


int resolve_word(hashBucket *pending_entry, transTable *translation_table, symbol *symbol_entry, wordNode *node_ptr, int IS_RELATIVE_LABEL){
  int symbol_IC = symbol_entry->address;
  int command_IC = translation_table[pending_entry->command_index].address;
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
