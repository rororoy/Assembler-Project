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

    printf("Looking at: %s\n\n", label);
    printf("| %07d  | %-30s | \n\n\n", translation_table[pending_entry->command_index].address, translation_table[pending_entry->command_index].source_code ? translation_table[pending_entry->command_index].source_code : "");

    symbol_entry = find_symbol(symbol_table, label);

    if(!symbol_entry){
      print_error("Label didnt reolve", label, translation_table[pending_entry->command_index].address);

    } else {
      /* Get the pointer to the word we need to resolbe first */
      wordNode *node_ptr = get_word_at_index(translation_table[pending_entry->command_index].node, pending_entry->word_number);

      if(!node_ptr) {
        printf("DIDNT FIND WORD\n");
      } else {
        /* Access the data field through the pointer */
        printf("FOUND WORD:\n");
        print_word_binary(node_ptr->data);

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

int create_output_files(char *filename, transTable *translation_table,
                      symbolTable *symbol_table, hashTable *pending_labels, int IC, int DC) {
    FILE *file;
    char *ob_file = append_extension(filename, ".ob");
    char *ent_file = append_extension(filename, ".ent");
    int success = 1;
    int has_externals = 0;
    int has_entries = 0;
    int i;

    /* Check if there are any external or entry symbols */
    for (i = 0; i < symbol_table->size; i++) {
        if (symbol_table->symbols[i].context == CONTEXT_EXTERN) {
            has_externals = 1;
        } else if (symbol_table->symbols[i].context == CONTEXT_ENTRY) {
            has_entries = 1;
        }
    }

    /* Open the ob file for writing */
    file = fopen(ob_file, "w");
    if (file == NULL) {
        print_error("File write", filename, 0);
        free(ob_file);
        free(ent_file);
        return 0;
    }

    /* Generate the object file */
    if (!generate_ob_file(file, translation_table, IC, DC)) {
        success = 0;
    }
    fclose(file);

    /* Generate the ext file if there are external symbols */
    if (has_externals && success) {
        /* Use our new function that handles both symbol table and pending labels */
        generate_externals_file(filename, symbol_table, pending_labels);
    }

    /* Generate the ent file if there are entry symbols */
    if (has_entries && success) {
        file = fopen(ent_file, "w");
        if (file == NULL) {
            print_error("File write", ent_file, 0);
            success = 0;
        } else {
            if (!generate_ent_file(file, symbol_table)) {
                success = 0;
            }
            fclose(file);
        }
    }

    free(ob_file);
    free(ent_file);
    return success;
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

 /**
  * Generates the object (.ob) file from the translation table.
  * This implementation follows the same approach as print_complete_transTable.
  *
  * @param file The output file pointer.
  * @param translation_table The translation table containing the assembled code.
  * @return 1 if successful, 0 if an error occurred.
  */

int generate_ob_file(FILE *file, transTable *translation_table, int IC, int DC) {
     int i;
     wordNode *current_node;
     int word_index;
     char hex_str[7]; /* 6 hex chars + null terminator */
     int is_data_entry;

     /* TODO CONVERT IC-100 TO A STRING AND CALCULATE HOW MUCH PADDING USING A STRING OF SPACES TO PAD */

     /* Write the IC and DC values at the top of the file */
     fprintf(file, "     %d %d\n", (IC-100), DC);

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

 /*
  * Generate the externals file (.ext) containing all external label references
  * Format: LABEL_NAME ADDRESS
  * References are sorted by address, not grouped by label
  */
 void generate_externals_file(char *filename, symbolTable *symbol_table, hashTable *pending_labels) {
     int i, j, ref_count = 0, capacity = 50;
     FILE *ext_file;
     char *ext_filename;
     char address_str[8]; /* For formatted 7-digit address */
     ExternalReference *all_refs;
     int current_index = 0;

     /* Create a dynamic array to hold all external references */
     all_refs = (ExternalReference*)malloc(capacity * sizeof(ExternalReference));
     if (all_refs == NULL) {
         printf("ERROR: Memory allocation failed in generate_externals_file\n");
         return;
     }

     /* First, collect external references from the symbol table */
     for (i = 0; i < symbol_table->size; i++) {
         symbol *sym = &symbol_table->symbols[i];

         if (sym->context == CONTEXT_EXTERN && sym->ext_ref_count > 0) {
             /* Add all references from this symbol's array */
             for (j = 0; j < sym->ext_ref_count; j++) {
                 /* Resize array if needed */
                 if (current_index >= capacity) {
                     capacity *= 2;
                     all_refs = (ExternalReference*)realloc(all_refs, capacity * sizeof(ExternalReference));
                     if (all_refs == NULL) {
                         printf("ERROR: Memory allocation failed during resize\n");
                         return;
                     }
                 }

                 all_refs[current_index].label_name = strdup(sym->name);
                 all_refs[current_index].address = sym->ext_references[j];
                 current_index++;
             }
         }
     }

     /* Next, collect references from the pending labels table */
     for (i = 0; i < pending_labels->size; i++) {
         if (pending_labels->bucket[i].is_taken &&
             pending_labels->bucket[i].type == BUCKET_PENDING_LABEL) {

             /* Check if this label is external */
             symbol *sym = find_symbol(symbol_table, pending_labels->bucket[i].label_name);
             if (sym != NULL && sym->context == CONTEXT_EXTERN) {
                 /* Resize array if needed */
                 if (current_index >= capacity) {
                     capacity *= 2;
                     all_refs = (ExternalReference*)realloc(all_refs, capacity * sizeof(ExternalReference));
                     if (all_refs == NULL) {
                         printf("ERROR: Memory allocation failed during resize\n");
                         return;
                     }
                 }

                 all_refs[current_index].label_name = strdup(pending_labels->bucket[i].label_name);
                 all_refs[current_index].address = pending_labels->bucket[i].addr;
                 current_index++;
             }
         }
     }

     /* Sort the references by address (simple bubble sort) */
     for (i = 0; i < current_index - 1; i++) {
         for (j = 0; j < current_index - i - 1; j++) {
             if (all_refs[j].address > all_refs[j + 1].address) {
                 /* Swap */
                 ExternalReference temp = all_refs[j];
                 all_refs[j] = all_refs[j + 1];
                 all_refs[j + 1] = temp;
             }
         }
     }

     /* Create the output file */
     ext_filename = append_extension(filename, ".ext");
     ext_file = fopen(ext_filename, "w");

     if (ext_file == NULL) {
         print_error("Failed to create externals file", filename, 0);

         /* Clean up */
         for (i = 0; i < current_index; i++) {
             free(all_refs[i].label_name);
         }
         free(all_refs);
         free(ext_filename);
         return;
     }

     /* Write all references to the file in order of address */
     for (i = 0; i < current_index; i++) {
         /* Format address as 7-digit number with leading zeros */
         sprintf(address_str, "%07d", all_refs[i].address);

         fprintf(ext_file, "%s %s\n", all_refs[i].label_name, address_str);
     }

     /* Clean up */
     fclose(ext_file);
     for (i = 0; i < current_index; i++) {
         free(all_refs[i].label_name);
     }
     free(all_refs);
     free(ext_filename);

     printf("Externals file created: %s (with %d references)\n", ext_filename, current_index);
 }

/**
 * Generates the entry (.ent) file from the symbol table.
 * Lists all entry symbols sorted by address.
 *
 * @param file The output file pointer.
 * @param symbol_table The symbol table containing entry symbols.
 * @return 1 if successful, 0 if an error occurred.
 */
int generate_ent_file(FILE *file, symbolTable *symbol_table) {
    int i, j;
    char **ent_names = NULL;
    int *ent_addresses = NULL;
    int num_entries = 0;

    /* First pass: count the number of entry symbols */
    for (i = 0; i < symbol_table->size; i++) {
        if (symbol_table->symbols[i].context == CONTEXT_ENTRY) {
            num_entries++;
        }
    }

    if (num_entries == 0) {
        return 1;  /* No entry symbols, return success */
    }

    /* Allocate memory for entry symbol names and addresses */
    ent_names = (char **)malloc(num_entries * sizeof(char *));
    ent_addresses = (int *)malloc(num_entries * sizeof(int));

    if (!ent_names || !ent_addresses) {
        if (ent_names) free(ent_names);
        if (ent_addresses) free(ent_addresses);
        return 0;
    }

    /* Second pass: record entry symbols */
    int index = 0;
    for (i = 0; i < symbol_table->size; i++) {
        if (symbol_table->symbols[i].context == CONTEXT_ENTRY) {
            ent_names[index] = symbol_table->symbols[i].name;
            ent_addresses[index] = symbol_table->symbols[i].address;
            index++;
        }
    }

    /* Sort entry symbols by address */
    for (i = 0; i < num_entries - 1; i++) {
        for (j = 0; j < num_entries - i - 1; j++) {
            if (ent_addresses[j] > ent_addresses[j + 1]) {
                int temp_addr = ent_addresses[j];
                ent_addresses[j] = ent_addresses[j + 1];
                ent_addresses[j + 1] = temp_addr;

                char *temp_name = ent_names[j];
                ent_names[j] = ent_names[j + 1];
                ent_names[j + 1] = temp_name;
            }
        }
    }

    /* Write sorted entry symbols to file */
    for (i = 0; i < num_entries; i++) {
        fprintf(file, "%-6s %07d\n", ent_names[i], ent_addresses[i]);
    }

    /* Clean up */
    free(ent_names);
    free(ent_addresses);

    return 1;
}
