#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "./global.h"

#define INITIAL_TABLE_SIZE 10
#define GROWTH_FACTOR 2


typedef struct{
  /* Highest bits first */
  unsigned opcode     : 6; /* bits 18...23 */
  unsigned src_mode   : 2; /* bits 16...17  */
  unsigned src_reg    : 3; /* bits 13...15 */
  unsigned dst_mode   : 2; /* bits 11...12 */
  unsigned dst_reg    : 3; /* bits 8....10 */
  unsigned funct      : 5; /* bits 7...3  */
  signed   a          : 1; /* bits 2 */
  signed   r          : 1; /* bits 1 */
  signed   e          : 1; /* bits 0 */
} instructionWord;

/* Command symanyics */
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

typedef struct{
  int address;
  char *source_code;
  instructionWord binary[3];
} transTable;

/* Structure to hold the symbol table and its metadata */
typedef struct {
  symbol *symbols;    /* Pointer to array of symbols */
  int size;          /* Current number of symbols */
  int capacity;      /* Total capacity of array */
} symbolTable;

extern char *allowed_commands[];

extern commandSem command_table[];

commandSem *command_lookup(char *cmd_name);

symbolTable* create_symbol_table();

int resize_symbol_table(symbolTable *table);

int insert_symbol(symbolTable *table, const char *name, int address, labelType type);

int is_valid_command();

void print_symbol_table(const symbolTable *table);


#endif
