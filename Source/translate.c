#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../Headers/linked_list.h"
#include "../Headers/translate.h"
#include "../Headers/utils.h"
#include "../Headers/global.h"

/* TODO MOVE THIS TO GLOBAL VVV */

commandSem command_table[] = {
    /*  CMD   funct op */
    {CMD_MOV, -1,  0},
    {CMD_CMP, -1,  1},
    {CMD_ADD,  1,  2},
    {CMD_SUB,  2,  2},
    {CMD_LEA, -1,  4},

    {CMD_CLR,  1,  5},
    {CMD_NOT,  2,  5},
    {CMD_INC,  3,  5},
    {CMD_DEC,  4,  5},
    {CMD_JMP,  1,  9},
    {CMD_BNE,  2,  9},
    {CMD_JSR,  3,  9},
    {CMD_RED, -1, 12},
    {CMD_PRN, -1, 13},

    {CMD_RTS, -1, 14},
    {CMD_STOP,-1, 15},

    {CMD_EXTERN, -1, -1},
    {CMD_ENTRY, -1, -1},
    {CMD_DATA, -1, -1},
    {CMD_STRING, -1, -1}
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
int insert_symbol(symbolTable *table, char *name, int address, labelType type) {
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

    table->size++;
    return 1;  /* Success */
}

/* Search for a symbol by name in the symbol table */
symbol *find_symbol(symbolTable *table, char *name) {
    int i;

    /* Check if table is valid */
    if (table == NULL || name == NULL) {
        return NULL;
    }

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

  printf("@[PUTTING src_mode:%d, dst_mode:%d, funct: %d]\n", src_mode, dst_mode, funct);

  /* Set up the instruction word */
  new_word.instruction.opcode = opcode;
  print_word_binary(new_word);

  new_word.instruction.src_mode = src_mode;
  print_word_binary(new_word);

  new_word.instruction.src_reg = src_reg;
  print_word_binary(new_word);

  new_word.instruction.dst_mode = dst_mode;
  print_word_binary(new_word);

  new_word.instruction.dst_reg = dst_reg;
  new_word.instruction.funct = funct;
  print_word_binary(new_word);

  new_word.instruction.a = 1; /* Typically absolute for instructions */
  new_word.instruction.r = 0;
  new_word.instruction.e = 0;
  print_word_binary(new_word);

  /* Add the new word to the linked list */
  add_word_node(&(table[index].node), new_word);
}

void print_word_binary(word w) {
  int i;
    /* Check if this is likely a data word (all high bits are 0) */
    if (w.instruction.opcode == 0 && w.instruction.src_mode == 0 &&
        w.instruction.src_reg == 0 && w.instruction.dst_mode == 0 &&
        w.instruction.dst_reg == 0 && w.instruction.funct < 0x10) {

        /* It's probably a data word, print all 24 bits */
        printf("Binary (data): ");
        for (i = 23; i >= 0; i--) {
            printf("%d", (w.data_word.data & (1 << i)) ? 1 : 0);
        }
        printf("\n");
    } else {
        /* Print as instruction word with field groupings */
        printf("Binary (instruction): %06u %02u %03u %02u %03u %05u %u %u %u\n",
               w.instruction.opcode, w.instruction.src_mode,
               w.instruction.src_reg, w.instruction.dst_mode,
               w.instruction.dst_reg, w.instruction.funct,
               w.instruction.a, w.instruction.r, w.instruction.e);
    }
}

/* Print the symbol table */
void print_symbol_table(const symbolTable *table) {
    int i;
    printf("\nSymbol Table Contents:\n");
    printf("---------------------\n");
    for (i = 0; i < table->size; i++) {
        printf("[%d] Name: %-20s Address: %-6d Type: %s\n",
               i,
               table->symbols[i].name,
               table->symbols[i].address,
               table->symbols[i].type == LBL_CODE ? "CODE" : "DATA");
    }
    printf("---------------------\n");
    printf("Total symbols: %d (Capacity: %d)\n", table->size, table->capacity);
}


int insert_extra_word(transTable *table, int index, int address, char *source_code, int op_type, int value) {
  word new_word = {0}; /* Initialize to all zeros */

  /* Check if this is the first word being added for cases such as .data .string*/
  if (table[index].node == NULL) {
    /* Initialize the table entry if it hasn't been initialized yet */
    initialize_transTable_entry(&table[index], address, source_code);
  }

  if (op_type == 0) { /* For a number */
    printf(">>>>>>>GOT IMM NUMBER:%d\n", value);
    /* Set addressing flags */
    new_word.extra_word.value = (unsigned)value & 0x1FFFFF; /* 0x1FFFFF = 21 bits of 1s */
    new_word.extra_word.a = 1; /* Absolute addressing for numbers */
    new_word.extra_word.r = 0; /* Not relocatable */
    new_word.extra_word.e = 0; /* Not external */
  }
  else if (op_type == 4) { /* For a commands of data (.string .data) */
    /* Store the value in the full 24-bit data_word */
    printf(">>>>>>>GOT DATA VALUE:%d\n", (unsigned)value);

    /* Directly assign the value */
    new_word.data_word.data = (unsigned)value;
  }

  /* Always append the new word to the end of the list */
  add_word_node(&(table[index].node), new_word);
  return 1; /* Success */
}
