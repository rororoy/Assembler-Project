#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "./global.h"

#define INITIAL_TABLE_SIZE 10
#define GROWTH_FACTOR 2


typedef union {
    struct {
        unsigned value : 22;  /* Stores the number/address (bits 0-21) */
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
  word binary[3];
} transTable;


extern char *allowed_commands[];

extern commandSem command_table[];

extern char *registers[];

commandSem *command_lookup(char *cmd_name);

symbolTable* create_symbol_table();

int resize_symbol_table(symbolTable *table);

int insert_symbol(symbolTable *table, char *name, int address, labelType type);

void initialize_transTable_entry(transTable *entry, int address, char *source_code);

int is_valid_command();

void print_symbol_table(const symbolTable *table);


#endif
