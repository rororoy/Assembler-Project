#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "./global.h"

#define INITIAL_TABLE_SIZE 10
#define GROWTH_FACTOR 2

/*
  Define addressing mode bit flags:
    We represent the allowed addressing modes as a 4 bit binary number
    each bit representing a different mode and turning on if alloweed
*/
#define ADDR_IMMEDIATE  0x01  /* 0001 in binary */
#define ADDR_DIRECT     0x02  /* 0010 in binary */
#define ADDR_RELATIVE   0x04  /* 0100 in binary */
#define ADDR_REGISTER   0x08  /* 1000 in binary */
#define ADDR_ALL        0x0F  /* All modes allowed */
#define ADDR_NONE       0x00  /* No addressing modes allowed */

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
  int allowed_src_add_mode;
  int allowed_dest_add_mode;
} commandSem;

/* Define an enum for the supported assembly commands */
typedef enum {LBL_CODE, LBL_DATA} labelType;

/* Define an enum with the context of the label defined */
typedef enum {CONTEXT_EXTERN, CONTEXT_ENTRY, CONTEXT_NORMAL} labelContext;

/* Define a num for the flags ARE that should be turned on in the command translation */
typedef enum {A, R, E, ARE_NONE} commandARE;

/* Entry in the symbol table */
typedef struct{
  char *name;
  int address;
  labelType type;
  labelContext context;

  /* Dynamic array to track addresses where this label is mentioned */
  int *ext_references;     /* Array of word addresses */
  int ext_ref_count;       /* Number of references */
  int ext_ref_capacity;    /* Current capacity of the array */
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

/* Function prototypes - rest of the file remains the same */
commandSem *command_lookup(char *cmd_name);

symbolTable* create_symbol_table();

int resize_symbol_table(symbolTable *table);

int insert_symbol(symbolTable *table, char *name, int address, labelType type, labelContext context);

symbol* find_symbol(symbolTable *table, char *name);

int update_symbol_address(symbolTable *symbol_table, char *symbol_name, int new_address, int new_context);

int is_missing_symbols(symbolTable *table);

void initialize_transTable_entry(transTable *entry, int address, char *source_code);

transTable *create_transTable(int initial_size);

void free_transTable_entry(transTable *entry);

void free_transTable(transTable *table, int size);

void insert_command_entry(transTable *table, int index, int address, char *source_code,
                         int opcode, int src_mode, int src_reg, int dst_mode, int dst_reg, int funct);

void print_word_binary(word w);

int insert_extra_word(transTable *table, int index, int address, char *source_code, int op_type, int value, commandARE are_fields);

void print_complete_transTable(transTable *table, int size);

int is_valid_command();

void print_symbol_table(const symbolTable *table);

void update_word(wordNode *node_ptr, int value, commandARE are_flags);

char* word_to_hex(word w, char* hex_str);

void init_ext_references(symbol *sym);

int add_ext_reference(symbol *sym, int address);

void free_ext_references(symbol *sym);

#endif /* TRANSLATE_H */
