#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/linked_list.h"
#include "../Headers/translate.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"
#include "../Headers/error.h"

commandSem *command_lookup(char *cmd_name) {
  int i;
  for (i = 0; i < NUM_ALLOWED_COMMANDS; i++) {
    if (strcmp(cmd_name, allowed_commands[i]) == 0) {
      return &command_table[i]; /* Found match at index i */
    }
  }
  return NULL; /* Not found */
}

/* Create a new symbol table */
symbolTable* create_symbol_table() {
  symbolTable *table = (symbolTable*)malloc(sizeof(symbolTable));
  if (!check_malloc(table)) {
    print_error("Failed creating structure", "symbolTable", 0);
    return NULL;
  }

  table->symbols = (symbol*)malloc(INITIAL_TABLE_SIZE * sizeof(symbol));
  if (!check_malloc(table->symbols)) {
    free(table);
    return NULL;
  }

  table->size = 0;
  table->capacity = INITIAL_TABLE_SIZE;
  return table;
}

/* Resize the symbol table */
int resize_symbol_table(symbolTable *table) {
  int new_capacity = table->capacity * GROWTH_FACTOR;
  symbol *new_symbols = (symbol*)realloc(table->symbols, new_capacity * sizeof(symbol));

  if (!check_malloc(new_symbols)) {
    return 0; /* Failed to resize */
  }

  table->symbols = new_symbols;
  table->capacity = new_capacity;
  return 1;  /* Success */
}

/* Insert a symbol into the table */
int insert_symbol(symbolTable *table, char *name, int address, labelType type, labelContext context) {
  /* Check if we need to resize */
  if (table->size >= table->capacity) {
    if (!resize_symbol_table(table)) {
      return 0;  /* Failed to resize */
    }
  }

  /* Allocate memory for the name */
  table->symbols[table->size].name = (char*)malloc(strlen(name) + 1);
  if (!check_malloc(table->symbols[table->size].name)) {
    return 0; /* Failed to allocate memory for name */
  }

  /* Copy the data */
  strcpy(table->symbols[table->size].name, name);
  table->symbols[table->size].address = address;
  table->symbols[table->size].type = type;
  table->symbols[table->size].context = context;

  /* Initialize the external references fields */
  table->symbols[table->size].ext_references = NULL;
  table->symbols[table->size].ext_ref_count = 0;
  table->symbols[table->size].ext_ref_capacity = 0;

  /* Increment size */
  table->size++;

  return 1;  /* Success */
}

/* Search for a symbol by name in the symbol table */
symbol *find_symbol(symbolTable *table, char *name) {
  int i;

  /* Linear search through the table */
  for (i = 0; i < table->size; i++) {
    if (strcmp(table->symbols[i].name, name) == 0) {
      /* Found the symbol */
      return &(table->symbols[i]);
    }
  }

  /* Symbol not found */
  return NULL;
}

int update_symbol_address(symbolTable *symbol_table, char *symbol_name, int new_address, int new_context) {
  symbol *symbol_entry;

  if (symbol_table == NULL || symbol_name == NULL) {
    return 0;
  }

  /* Use the existing find_symbol function to locate the symbol */
  symbol_entry = find_symbol(symbol_table, symbol_name);

  if (symbol_entry != NULL) {
    /* Symbol found, update its address */
    symbol_entry->address = new_address;

    /* Update context if a new one was specified (not -1) */
    if (new_context != -1) {
      symbol_entry->context = new_context;
    }

    return 1; /* Success */
  }

  /* Symbol not found */
  return 0;
}

int is_missing_symbols(symbolTable *table){
  int i;

  /* Linear search through the table */
  for (i = 0; i < table->size; i++) {
    if (table->symbols[i].address == -1) {
      /* Found the symbol */
      return 1;
    }
  }

  /* Symbol not found */
  return 0;
}


/* Initialize a single transTable entry */
void initialize_transTable_entry(transTable *entry, int address, char *source_code) {
  /* Set the address */
  entry->address = address;

  /* Allocate and copy the source code */
  if (source_code) {
    entry->source_code = strdup(source_code); /* Creates a duplicate of the string */
  } else {
    entry->source_code = NULL;
  }

  /* Initialize word node pointer to NULL */
  entry->node = NULL;
}

/* Create and initialize a dynamic array of transTable entries */
transTable* create_transTable(int initial_size) {
  int i;
  transTable *table = (transTable *)malloc(initial_size * sizeof(transTable));
  if (!table) {
      return NULL; /* Memory allocation failed */
  }

  /* Initialize all entries */
  for (i = 0; i < initial_size; i++) {
    initialize_transTable_entry(&table[i], 0, NULL);
    /* Now each entry has address=0, source_code=NULL, and node=NULL */
  }

  return table;
}

/* Clean up a transTable entry when it's no longer needed */
void free_transTable_entry(transTable *entry) {
  if (entry->source_code) {
    free(entry->source_code);
    entry->source_code = NULL;
  }

  /* Free the linked list of words if it exists */
  if (entry->node) {
    free_word_list(entry->node);
    entry->node = NULL;
  }
}

/* Clean up the entire transTable array */
void free_transTable(transTable *table, int size) {
  int i;
  if (!table) return;

  for (i = 0; i < size; i++) {
    free_transTable_entry(&table[i]);
  }

  free(table);
}

void insert_command_entry(transTable *table, int index, int address, char *source_code,
                         int opcode, int src_mode, int src_reg, int dst_mode, int dst_reg, int funct) {
  word new_word;

  /* Initialize entry with address and source code */
  initialize_transTable_entry(&table[index], address, source_code);

  /* Handle invalid modes */
  src_mode = src_mode == -1 ? 0 : src_mode;
  dst_mode = dst_mode == -1 ? 0 : dst_mode;

  /* printf("@[PUTTING src_mode:%d, dst_mode:%d, funct: %d]\n", src_mode, dst_mode, funct); */

  /* Set up the instruction word */
  new_word.instruction.opcode = opcode;
  new_word.instruction.src_mode = src_mode;
  new_word.instruction.src_reg = src_reg;
  new_word.instruction.dst_mode = dst_mode;
  new_word.instruction.dst_reg = dst_reg;
  new_word.instruction.funct = funct;
  new_word.instruction.a = 1;
  new_word.instruction.r = 0;
  new_word.instruction.e = 0;

  /* Add the new word to the linked list */
  add_word_node(&(table[index].node), new_word);
}


int insert_extra_word(transTable *table, int index, int address, char *source_code, int op_type, int value, commandARE are_fields) {
  word new_word = {0}; /* Initialize to all zeros */

  /* Check if this is the first word being added for cases such as .data .string*/
  if (table[index].node == NULL) {
    /* Initialize the table entry if it hasn't been initialized yet */
    initialize_transTable_entry(&table[index], address, source_code);
  }

  /* Set the A R E flags incase of NONE - set nothing */
  if      (are_fields == A) new_word.extra_word.a = 1;
  else if (are_fields == R) new_word.extra_word.r = 1;
  else if (are_fields == E) new_word.extra_word.e = 1;


  if (op_type == 0) { /* For a number */
    /* Set addressing flags */
    new_word.extra_word.value = (unsigned)value & 0x1FFFFF; /* 0x1FFFFF = 21 bits of 1s */
  }
  else if (op_type == 1 || op_type == 4) { /* For a commands of data (.string .data) */
    /* Directly assign the value */
    new_word.data_word.data = (unsigned)value;
  }

  /* Always append the new word to the end of the list */
  add_word_node(&(table[index].node), new_word);
  return 1; /* Success */
}

void update_word(wordNode *node_ptr, int value, commandARE are_flags) {
  /* Reset the word to all zeros first */
  word new_word = {0};

  /* Set the value in the extra_word structure */
  new_word.extra_word.value = (unsigned)value & 0x1FFFFF; /* Mask to 21 bits */

  /* Set the appropriate ARE flags based on the enum */
  if (are_flags == A) {
      new_word.extra_word.a = 1;
  } else if (are_flags == R) {
      new_word.extra_word.r = 1;
  } else if (are_flags == E) {
      new_word.extra_word.e = 1;
  }
  /* If are_flags is ARE_NONE, all flags remain 0 */

  /* Update the word in the node */
  node_ptr->data = new_word;
}

/* Initialize external references array for a symbol */
void init_ext_references(symbol *sym) {
  sym->ext_references = NULL;
  sym->ext_ref_count = 0;
  sym->ext_ref_capacity = 0;
}

/* Add an external reference to a symbol */
int add_ext_reference(symbol *sym, int address) {
  int *new_array;
  int new_capacity;

  /* Validate input */
  if (sym == NULL) {
      print_error("Missing argument", "symbol", 0);
      return 0;
  }

  /* Initialize arrays if needed - this prevents segfaults */
  if (sym->ext_references == NULL) {
    sym->ext_ref_count = 0;
    sym->ext_ref_capacity = 0;
  }

  /* Check if we need to allocate or resize the array */
  if (sym->ext_ref_count >= sym->ext_ref_capacity) {
    new_capacity = sym->ext_ref_capacity == 0 ? 4 : sym->ext_ref_capacity * 2;

    /* First allocation or reallocation */
    if (sym->ext_references == NULL) {
      new_array = (int*)malloc(new_capacity * sizeof(int));
    } else {
      new_array = (int*)realloc(sym->ext_references, new_capacity * sizeof(int));
    }

    if (new_array == NULL) {
      print_error("Malloc", "in add_ext_reference", 0);
      return 0; /* Allocation failed */
    }

    sym->ext_references = new_array;
    sym->ext_ref_capacity = new_capacity;
    }

    /* Add the new reference */
    sym->ext_references[sym->ext_ref_count++] = address;
    return 1; /* Success */
}

/* Free the external references array */
void free_ext_references(symbol *sym) {
  if (sym->ext_references != NULL) {
      free(sym->ext_references);
      sym->ext_references = NULL;
  }
  sym->ext_ref_count = 0;
  sym->ext_ref_capacity = 0;
}

/* Free the entire symbol table */
void free_symbol_table(symbolTable *table) {
  int i;

  if (table == NULL) {
      return;
  }

  /* Free all symbols */
  for (i = 0; i < table->size; i++) {
    /* Free the symbol name */
    free(table->symbols[i].name);

    /* Free any external references array */
    free_ext_references(&table->symbols[i]);
  }

  /* Free the symbols array */
  free(table->symbols);

  /* Free the table structure itself */
  free(table);
}
