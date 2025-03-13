#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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
    {CMD_RTS, -1, 13},
    {CMD_PRN, -1, 14},
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
    "rts",
    "prn",
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
symbol* find_symbol(symbolTable *table, char *name) {
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

    /* Initialize all binary words to zero */
    memset(entry->binary, 0, sizeof(entry->binary));
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
    }

    return table;
}

/* Clean up a transTable entry when it's no longer needed */
void free_transTable_entry(transTable *entry) {
    if (entry->source_code) {
        free(entry->source_code);
        entry->source_code = NULL;
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
    int i, j, bit;
    int has_binary;
    char binary_str[25]; /* For 24-bit binary representation */

    /* Print table header */
    printf("+----------+--------------------------------+-------------------------+\n");
    printf("| Address  | Source Code                    | Binary Machine Code     |\n");
    printf("| (decimal)|                                |                         |\n");
    printf("+----------+--------------------------------+-------------------------+\n");

    /* Print each entry in the table */
    for (i = 0; i < size; i++) {
        /* Print address in decimal, padded to 7 digits */
        printf("| %07d  | %-30s | ", table[i].address, table[i].source_code ? table[i].source_code : "");

        /* Check if this entry has any binary data to print */
        has_binary = 0;
        for (j = 0; j < 3; j++) {
            if (*(unsigned int*)&table[i].binary[j] != 0) {
                has_binary = 1;
                break;
            }
        }

        /* Print first binary word and close the row */
        if (has_binary) {
            /* Convert the first word to binary string representation */
            for (bit = 0; bit < 24; bit++) {
                /* Extract each bit from the word */
                unsigned int word_value = *(unsigned int*)&table[i].binary[0];
                binary_str[23-bit] = ((word_value >> bit) & 1) ? '1' : '0';
            }
            binary_str[24] = '\0'; /* Null-terminate the string */
            printf("%s |\n", binary_str);

            /* Print additional binary words if they exist */
            for (j = 1; j < 3; j++) {
                if (*(unsigned int*)&table[i].binary[j] != 0) {
                    /* For additional words, print blank in address and source columns */
                    printf("| %7s  | %-30s | ", "", "");

                    /* Convert to binary string */
                    for (bit = 0; bit < 24; bit++) {
                        unsigned int word_value = *(unsigned int*)&table[i].binary[j];
                        binary_str[23-bit] = ((word_value >> bit) & 1) ? '1' : '0';
                    }
                    binary_str[24] = '\0';
                    printf("%s |\n", binary_str);
                }
            }
        } else {
            /* No binary data for this entry */
            printf("%24s |\n", "");
        }
    }

    /* Print table footer */
    printf("+----------+--------------------------------+-------------------------+\n");
}

/* Insert a command entry in the transTable */
void insert_command_entry(transTable *table, int index, int address, char *source_code,
                         int opcode, int src_mode, int src_reg, int dst_mode, int dst_reg, int funct) {
    /* Initialize entry with address and source code */
    initialize_transTable_entry(&table[index], address, source_code);

    /* Set up the instruction word */
    table[index].binary[0].instruction.opcode = opcode;
    table[index].binary[0].instruction.src_mode = src_mode;
    table[index].binary[0].instruction.src_reg = src_reg;
    table[index].binary[0].instruction.dst_mode = dst_mode;
    table[index].binary[0].instruction.dst_reg = dst_reg;
    table[index].binary[0].instruction.funct = funct;
    table[index].binary[0].instruction.a = 1; /* Typically absolute for instructions */
    table[index].binary[0].instruction.r = 0;
    table[index].binary[0].instruction.e = 0;
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


int insert_extra_word(transTable *tb, int wordtype, int value, int word_index) {
    /* Make sure we're only using valid indices (0, 1, or 2) */
    if (word_index < 0 || word_index > 2) {
        return 0; /* Invalid index */
    }

    if (wordtype == 0) { /* For a number */
        /* Store the number in the value field (22 bits) */
        tb->binary[word_index].extra_word.value = (unsigned)value & 0x3FFFFF;
        /* Set addressing flags */
        tb->binary[word_index].extra_word.a = 1; /* Absolute addressing for numbers */
        tb->binary[word_index].extra_word.r = 0; /* Not relocatable */
        tb->binary[word_index].extra_word.e = 0; /* Not external */
    }
    return 1; /* Success */
}
