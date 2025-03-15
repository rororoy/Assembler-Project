#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "./global.h"

#define INITIAL_TABLE_SIZE 10
#define GROWTH_FACTOR 2

typedef struct wordNode wordNode;

typedef union {
  struct {
    unsigned data : 24;  /* Stores the data (bits 0-23) */
  } data_word;  /* Used when storing a data component */

  struct {
    unsigned value : 21;  /* Stores the number/address (bits 0-21) */
    unsigned a     : 1;   /* Absolute flag (bit 22) */
    unsigned r     : 1;   /* Relocatable flag (bit 23) */
    unsigned e     : 1;   /* External flag (bit 24) */
  } extra_word;  /* Used when storing an address or immediate value */

  struct {
    unsigned opcode   : 6;  /* bits 18-23 */
    unsigned src_mode : 2;  /* bits 16-17 */
    unsigned src_reg  : 3;  /* bits 13-15 */
    unsigned dst_mode : 2;  /* bits 11-12 */
    unsigned dst_reg  : 3;  /* bits 8-10 */
    unsigned funct    : 5;  /* bits 3-7 */
    unsigned a        : 1;  /* bit 2 */
    unsigned r        : 1;  /* bit 1 */
    unsigned e        : 1;  /* bit 0 */
  } instruction;  /* Used when storing the main instruction word */
} word;


/* Command symantics */
typedef struct{
  commands name;
  int funct;
  int op_code;
  int type;
} commandSem;

/* Define an enum for the supported assembly commands */
typedef enum {LBL_CODE, LBL_DATA} labelType;

/* Entry in the symbol table */
typedef struct{
  char *name;
  int address;
  labelType type;
} symbol;

/* Structure to hold the symbol table and its metadata */
typedef struct {
  symbol *symbols;    /* Pointer to array of symbols */
  int size;          /* Current number of symbols */
  int capacity;      /* Total capacity of array */
} symbolTable;

typedef struct{
  int address;
  char *source_code;
  wordNode *node;
} transTable;

extern char *allowed_commands[];

extern commandSem command_table[];

extern char *registers[];

commandSem *command_lookup(char *cmd_name);

symbolTable* create_symbol_table();

int resize_symbol_table(symbolTable *table);

int insert_symbol(symbolTable *table, char *name, int address, labelType type);

symbol* find_symbol(symbolTable *table, char *name);

/* Initialize a single transTable entry */
void initialize_transTable_entry(transTable *entry, int address, char *source_code);

/* Create and initialize a dynamic array of transTable entries */
transTable *create_transTable(int initial_size);

/* Free a single transTable entry */
void free_transTable_entry(transTable *entry);

/* Free the entire transTable */
void free_transTable(transTable *table, int size);

/* Insert a command entry in the transTable */
void insert_command_entry(transTable *table, int index, int address, char *source_code,
                         int opcode, int src_mode, int src_reg, int dst_mode, int dst_reg, int funct);

void print_word_binary(word w);

/* Insert extra word for a number or address - takes int value directly */
int insert_extra_word(transTable *table, int index, int address, char *source_code, int op_type, int value);

/* Print the complete transTable */
void print_complete_transTable(transTable *table, int size);

int is_valid_command();

void print_symbol_table(const symbolTable *table);


#endif
