#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "../Headers/files.h"
#include "../Headers/error.h"
#include "../Headers/global.h"
#include "../Headers/utils.h"
#include "../Headers/translate.h"
#include "../Headers/second_stage.h"

void clean_output_files(char *filename) {
  char *full_filename;
  int i;
  char *extensions[] = {".ob", ".am", ".ent", ".ext"};

  /* Check each possible output file extension */
  for (i = 0; i < 4; i++) {
    /* Create the full filename with extension */
    full_filename = append_extension(filename, extensions[i]);

    if (full_filename != NULL) {
      /* Try to remove the file if it exists */
      if (remove(full_filename) == 0) {
        /* File was successfully removed */
      }
      /* Note: We don't report errors if file doesn't exist - that's expected */

      /* Free the memory allocated by append_extension */
      free(full_filename);
    }
  }
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
    /* Use our function that handles both symbol table and pending labels */
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

int generate_ob_file(FILE *file, transTable *translation_table, int IC, int DC) {
  int i;
  wordNode *current_node;
  int word_index;
  char hex_str[7]; /* 6 hex chars + null terminator */
  int is_data_entry;

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
    check_malloc(all_refs);
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
            check_malloc(all_refs);
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
            check_malloc(all_refs);
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
    print_error("Failed writing", filename, 0);

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
}

int generate_ent_file(FILE *file, symbolTable *symbol_table) {
  int i, j;
  char **ent_names = NULL;
  int *ent_addresses = NULL;
  int num_entries = 0;
  int index;
  char *temp_name;
  int temp_addr;

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
  index = 0;
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
        temp_addr = ent_addresses[j];
        ent_addresses[j] = ent_addresses[j + 1];
        ent_addresses[j + 1] = temp_addr;

        temp_name = ent_names[j];
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
