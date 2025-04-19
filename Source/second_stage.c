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

    symbol_entry = find_symbol(symbol_table, label);

    if(!symbol_entry){
      print_error("Label didnt reolve", label, translation_table[pending_entry->command_index].address - 99); /* IC - 100 = LINE */

    } else {
      /* Get the pointer to the word we need to resolbe first */
      wordNode *node_ptr = get_word_at_index(translation_table[pending_entry->command_index].node, pending_entry->word_number);

      if(node_ptr){
        /* Access the data field through the pointer */

        /* RESOLVING PENDING PLACEHOLDER WORDS: */
        /* If the ARE = ARE_NONE (000) THEN CHECK SYMBOL TABLE FOR THE CONTEXT */
        /* If the ARE = ARE_E (000) NOT POSSIBLE ELSE WE WOULDNT PLACE A PLACE HOLDER */
        /* If the ARE = ARE_R (010) THIS IS AN ENTRY WE SAY AND DIDNT RESOLVE - TAKE ADDRESS FROM SYMBOL TABLE */
        /* IF THE ARE = ARE_A ()  */

        if(!resolve_word(pending_entry, translation_table, symbol_entry, node_ptr, IS_RELATIVE_LABEL)){
          print_error("Label didnt reolve", label, translation_table[pending_entry->command_index].address - 99); /* IC - 100 = LINE*/
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
