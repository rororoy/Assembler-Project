#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/linked_list.h"
#include "../Headers/translate.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"

/* TODO MOVE THIS TO GLOBAL VVV */

/* Command table with addressing modes as bit flags */
commandSem command_table[] = {
    /* CMD     |funct|op|type|                    src_modes                   |                     dest_modes                */
    {CMD_MOV,  -1,   0, 1,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,     ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_CMP,  -1,   1, 1,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_ADD,   1,   2, 1,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,     ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_SUB,   2,   2, 1,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,     ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_LEA,  -1,   4, 1,     ADDR_DIRECT,                                      ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_CLR,   1,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_NOT,   2,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_INC,   3,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_DEC,   4,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_JMP,   1,   9, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_RELATIVE                  },
    {CMD_BNE,   2,   9, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_RELATIVE                  },
    {CMD_JSR,   3,   9, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_RELATIVE                  },
    {CMD_RED,  -1,  12, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_PRN,  -1,  13, 2,     ADDR_NONE,                                        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_RTS,  -1,  14, 3,     ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_STOP, -1,  15, 3,     ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_EXTERN, -1, -1, 4,    ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_ENTRY, -1, -1, 4,     ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_DATA, -1, -1, 4,      ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_STRING, -1, -1, 4,    ADDR_NONE,                                        ADDR_NONE                                    }
};

/* This array must match the order of the enum exactly. */
char *allowed_commands[] = {
    "mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "clr",
    "not",
    "inc",
    "dec",
    "jmp",
    "bne",
    "jsr",
    "red",
    "prn",
    "rts",
    "stop",
    ".extern",
    ".entry",
    ".data",
    ".string"
};

char *registers[] = {"r1", "r2", "r3", "r4", "r5", "r6"};


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
    check_malloc(table);

    table->symbols = (symbol*)malloc(INITIAL_TABLE_SIZE * sizeof(symbol));
    check_malloc(table->symbols);

    table->size = 0;
    table->capacity = INITIAL_TABLE_SIZE;
    return table;
}

/* Resize the symbol table */
int resize_symbol_table(symbolTable *table) {
    int new_capacity = table->capacity * GROWTH_FACTOR;
    symbol *new_symbols = (symbol*)realloc(table->symbols, new_capacity * sizeof(symbol));

    check_malloc(new_symbols);

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
    check_malloc(table->symbols[table->size].name);

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

void print_complete_transTable(transTable *table, int size) {
    int i, j, k;
    wordNode *current;
    int is_data_entry;

    /* Print table header */
    printf("+----------+--------------------------------+-------------------------+\n");
    printf("| Address  | Source Code                    | Binary Machine Code     |\n");
    printf("| (decimal)|                                |                         |\n");
    printf("+----------+--------------------------------+-------------------------+\n");

    /* Print each entry in the table */
    for (i = 0; i < size; i++) {
        /* Print address in decimal, padded to 7 digits */
        printf("| %07d  | %-30s | ", table[i].address, table[i].source_code ? table[i].source_code : "");

        current = table[i].node;

        /* If there are no words, print empty cell */
        if (current == NULL) {
            printf("%24s |\n", "");
            continue;
        }

        /* Check if this is a command or data entry by looking at the source code */
        is_data_entry = (table[i].source_code &&
                            (strstr(table[i].source_code, ".data") ||
                             strstr(table[i].source_code, ".string")));

        if (is_data_entry) {
            /* For data entries, all words use the data_word format */
            char binary_str[25];

            /* Print the first word as a data word */
            for (k = 0; k < 24; k++) {
                binary_str[23-k] = (current->data.data_word.data & (1 << k)) ? '1' : '0';
            }
            binary_str[24] = '\0';

            printf("%s |\n", binary_str);

            /* Move to the next word */
            current = current->next;

            /* Print additional data words */
            while (current != NULL) {
                /* Print data word format (24 bits) */
                for (k = 0; k < 24; k++) {
                    binary_str[23-k] = (current->data.data_word.data & (1 << k)) ? '1' : '0';
                }
                binary_str[24] = '\0';

                printf("| %7s  | %-30s | %s |\n", "", "", binary_str);

                /* Move to the next word */
                current = current->next;
            }
        } else {
            /* For normal commands, follow the original logic */
            /* Print first binary word as instruction word */
            {
                char binary_str[25];

                /* Convert each field to binary string representation */
                char opcode_str[7], src_mode_str[3], src_reg_str[4], dst_mode_str[3],
                     dst_reg_str[4], funct_str[6], are_str[4];

                /* ... (original bit extraction code for instruction format) ... */
                /* Convert opcode (6 bits) */
                int val = current->data.instruction.opcode;
                for (j = 0; j < 6; j++) {
                    opcode_str[5-j] = (val & (1 << j)) ? '1' : '0';
                }
                opcode_str[6] = '\0';

                /* Convert src_mode (2 bits) */
                val = current->data.instruction.src_mode;
                for (j = 0; j < 2; j++) {
                    src_mode_str[1-j] = (val & (1 << j)) ? '1' : '0';
                }
                src_mode_str[2] = '\0';

                /* Convert src_reg (3 bits) */
                val = current->data.instruction.src_reg;
                for (j = 0; j < 3; j++) {
                    src_reg_str[2-j] = (val & (1 << j)) ? '1' : '0';
                }
                src_reg_str[3] = '\0';

                /* Convert dst_mode (2 bits) */
                val = current->data.instruction.dst_mode;
                for (j = 0; j < 2; j++) {
                    dst_mode_str[1-j] = (val & (1 << j)) ? '1' : '0';
                }
                dst_mode_str[2] = '\0';

                /* Convert dst_reg (3 bits) */
                val = current->data.instruction.dst_reg;
                for (j = 0; j < 3; j++) {
                    dst_reg_str[2-j] = (val & (1 << j)) ? '1' : '0';
                }
                dst_reg_str[3] = '\0';

                /* Convert funct (5 bits) */
                val = current->data.instruction.funct;
                for (j = 0; j < 5; j++) {
                    funct_str[4-j] = (val & (1 << j)) ? '1' : '0';
                }
                funct_str[5] = '\0';

                /* Convert ARE bits */
                are_str[0] = current->data.instruction.a ? '1' : '0';
                are_str[1] = current->data.instruction.r ? '1' : '0';
                are_str[2] = current->data.instruction.e ? '1' : '0';
                are_str[3] = '\0';

                /* Concatenate all parts */
                sprintf(binary_str, "%s%s%s%s%s%s%s",
                        opcode_str, src_mode_str, src_reg_str, dst_mode_str,
                        dst_reg_str, funct_str, are_str);

                printf("%s |\n", binary_str);
            }

            /* Move to the next word in the list */
            current = current->next;

            /* Print additional binary words as extra_words */
            while (current != NULL) {
                char binary_str[26]; /* Increased size by 1 for null terminator */

                /* Convert value (21 bits) - adjusted from 22 to 21 bits */
                for (k = 0; k < 21; k++) {
                    binary_str[20-k] = (current->data.extra_word.value & (1 << k)) ? '1' : '0';
                }

                /* Convert ARE bits */
                binary_str[21] = current->data.extra_word.a ? '1' : '0';
                binary_str[22] = current->data.extra_word.r ? '1' : '0';
                binary_str[23] = current->data.extra_word.e ? '1' : '0';
                binary_str[24] = '\0';

                printf("| %7s  | %-30s | %s |\n", "", "", binary_str);

                /* Move to the next word */
                current = current->next;
            }
        }
    }

    /* Print table footer */
    printf("+----------+--------------------------------+-------------------------+\n");
}

/* Insert a command entry in the transTable */
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

void print_word_binary(word w) {
    int j;
    char binary_str[25];
    char opcode_str[7], src_mode_str[3], src_reg_str[4], dst_mode_str[3],
         dst_reg_str[4], funct_str[6], are_str[4];

    /* Check if this is likely a data word */
    if (w.instruction.opcode == 0 && w.instruction.src_mode == 0 &&
        w.instruction.src_reg == 0 && w.instruction.dst_mode == 0 &&
        w.instruction.dst_reg == 0 && w.instruction.funct < 0x10) {

        /* Print as data word (24 bits) */
        for (j = 0; j < 24; j++) {
            binary_str[23-j] = (w.data_word.data & (1 << j)) ? '1' : '0';
        }
        binary_str[24] = '\0';
        printf("%s\n", binary_str);
    } else {
        /* Convert opcode (6 bits) */
        int val = w.instruction.opcode;
        for (j = 0; j < 6; j++) {
            opcode_str[5-j] = (val & (1 << j)) ? '1' : '0';
        }
        opcode_str[6] = '\0';

        /* Convert src_mode (2 bits) */
        val = w.instruction.src_mode;
        for (j = 0; j < 2; j++) {
            src_mode_str[1-j] = (val & (1 << j)) ? '1' : '0';
        }
        src_mode_str[2] = '\0';

        /* Convert src_reg (3 bits) */
        val = w.instruction.src_reg;
        for (j = 0; j < 3; j++) {
            src_reg_str[2-j] = (val & (1 << j)) ? '1' : '0';
        }
        src_reg_str[3] = '\0';

        /* Convert dst_mode (2 bits) */
        val = w.instruction.dst_mode;
        for (j = 0; j < 2; j++) {
            dst_mode_str[1-j] = (val & (1 << j)) ? '1' : '0';
        }
        dst_mode_str[2] = '\0';

        /* Convert dst_reg (3 bits) */
        val = w.instruction.dst_reg;
        for (j = 0; j < 3; j++) {
            dst_reg_str[2-j] = (val & (1 << j)) ? '1' : '0';
        }
        dst_reg_str[3] = '\0';

        /* Convert funct (5 bits) */
        val = w.instruction.funct;
        for (j = 0; j < 5; j++) {
            funct_str[4-j] = (val & (1 << j)) ? '1' : '0';
        }
        funct_str[5] = '\0';

        /* Convert ARE bits */
        are_str[0] = w.instruction.a ? '1' : '0';
        are_str[1] = w.instruction.r ? '1' : '0';
        are_str[2] = w.instruction.e ? '1' : '0';
        are_str[3] = '\0';

        /* Concatenate all parts */
        sprintf(binary_str, "%s%s%s%s%s%s%s",
                opcode_str, src_mode_str, src_reg_str, dst_mode_str,
                dst_reg_str, funct_str, are_str);

        printf("%s\n", binary_str);
    }
}

/* Print the symbol table */
void print_symbol_table(const symbolTable *table) {
    int i, j;
    printf("\nSymbol Table Contents:\n");
    printf("---------------------\n");
    for (i = 0; i < table->size; i++) {
        printf("[%d] Name: %-20s Address: %-6d Type: %-4s   Context: %-8s",
               i,
               table->symbols[i].name,
               table->symbols[i].address,
               table->symbols[i].type == LBL_CODE ? "CODE" : "DATA",
               table->symbols[i].context == CONTEXT_EXTERN ? "EXTERNAL" :
               (table->symbols[i].context == CONTEXT_ENTRY ? "ENTRY" : ""));

        /* Print the array of external references if any exist */
        if (table->symbols[i].ext_ref_count > 0) {
            printf("   LABEL MENTIONS: [");
            for (j = 0; j < table->symbols[i].ext_ref_count; j++) {
                printf("%d", table->symbols[i].ext_references[j]);
                if (j < table->symbols[i].ext_ref_count - 1) {
                    printf(",");
                }
            }
            printf("]\n");
        } else {
            printf("\n");
        }
    }
    printf("---------------------\n");
    printf("Total symbols: %d (Capacity: %d)\n", table->size, table->capacity);
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

/**
 * Converts a word to a 6-character hexadecimal string
 *
 * @param w The word to convert
 * @param hex_str The output string buffer (must be at least 7 bytes long)
 * @return Pointer to the hex string (same as hex_str)
 *
 * This function takes a word union and returns a representation
 * as a 6-character hex string (24 bits = 6 hex digits).
 */
char* word_to_hex(word w, char* hex_str) {
  unsigned int value = 0;

    /* Extract the appropriate bits based on the word type */
    /* For instruction words */
    if (w.instruction.opcode != 0 || w.instruction.funct != 0) {
        value = (w.instruction.opcode << 18) |
                (w.instruction.src_mode << 16) |
                (w.instruction.src_reg << 13) |
                (w.instruction.dst_mode << 11) |
                (w.instruction.dst_reg << 8) |
                (w.instruction.funct << 3) |
                (w.instruction.a << 2) |
                (w.instruction.r << 1) |
                (w.instruction.e);
    }
    /* For extra words */
    else if (w.extra_word.value != 0 || w.extra_word.a || w.extra_word.r || w.extra_word.e) {
        value = (w.extra_word.value << 3) |
                (w.extra_word.a << 2) |
                (w.extra_word.r << 1) |
                (w.extra_word.e);
    }
    /* For data words */
    else {
        value = w.data_word.data;
    }

    /* Convert to lowercase hex string */
    snprintf(hex_str, 7, "%06x", value);

    return hex_str;
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
        printf("ERROR: NULL symbol passed to add_ext_reference\n");
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
            printf("ERROR: Memory allocation failed in add_ext_reference\n");
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
