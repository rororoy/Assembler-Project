#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "../Headers/global.h"
#include "../Headers/hash_table.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"

int second_pass(char *filename, hashTable *pending_labels, transTable *translation_table, symbolTable *symbol_table) {
  int i;
  hashBucket *entry_bucket;
  symbol *symbol_entry;
  /* Loop through all pending_labels */
  for (i = 0; i < pending_labels->size; i++) {
    entry_bucket = &(pending_labels->bucket[i]);
    if(!entry_bucket->is_taken) continue;
    printf("Looking at: %s\n\n", entry_bucket->label_name);
    printf("| %07d  | %-30s | \n\n\n", translation_table[entry_bucket->command_index].address, translation_table[entry_bucket->command_index].source_code ? translation_table[entry_bucket->command_index].source_code : "");
    symbol_entry = find_symbol(symbol_table, entry_bucket->label_name);
    if(!symbol_entry){
      printf("THE PENDING LABEL WASNT FOUND IN THE SYMBOLS\n");
    } else {
      int symbol_IC = symbol_entry->address;
      /* Get the pointer to the wordNode first */
      wordNode *node_ptr = get_word_at_index(translation_table[entry_bucket->command_index].node, entry_bucket->word_number);
      if(!node_ptr) {
        printf("DIDNT FIND WORD\n");
      } else {
        /* Access the data field through the pointer */
        word current_word = node_ptr->data;
        printf("FOUND WORD:\n");
        print_word_binary(current_word);
        
      }
    }
  }
  return 1;
}
