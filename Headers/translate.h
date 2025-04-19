#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "./global.h"

/* Initial size of the translation table */
#define INITIAL_TABLE_SIZE 10

  /* Factor by which the translation table grows when resize */
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

/* Forward declaration of wordNode structure */
typedef struct wordNode wordNode;

/*
 * This union contains different views of a 24-bit machine word,
 * allowing access to different bit fields based on the word's purpose.
 */
typedef union {
  /* View for a data word (used for .data directive) */
  struct {
    unsigned data : 24;  /**< Stores the data (bits 0-23) */
  } data_word;

  /*View for an extra word (used for address or immediate value) */
  struct {
    unsigned value : 21;  /* Stores the number/address (bits 0-21) */
    unsigned a     : 1;   /* Absolute flag (bit 22) */
    unsigned r     : 1;   /* Relocatable flag (bit 23) */
    unsigned e     : 1;   /* External flag (bit 24) */
  } extra_word;

  /* View for an instruction word (main operation) */
  struct {
    unsigned opcode   : 6;  /*  Operation code (bits 18-23) */
    unsigned src_mode : 2;  /*  Source addressing mode (bits 16-17) */
    unsigned src_reg  : 3;  /*  Source register (bits 13-15) */
    unsigned dst_mode : 2;  /*  Destination addressing mode (bits 11-12) */
    unsigned dst_reg  : 3;  /*  Destination register (bits 8-10) */
    unsigned funct    : 5;  /*  Function code (bits 3-7) */
    unsigned a        : 1;  /*  Absolute flag (bit 2) */
    unsigned r        : 1;  /*  Relocatable flag (bit 1) */
    unsigned e        : 1;  /*  External flag (bit 0) */
  } instruction;
} word;

/* Types of labels in the program */
typedef enum {
  LBL_CODE,  /* Label points to code (instruction) */
  LBL_DATA   /* Label points to data */
} labelType;

/* Context in which a label is defined or used */
typedef enum {
  CONTEXT_EXTERN,  /* Label is external (defined in another file) */
  CONTEXT_ENTRY,   /* Label is an entry point (accessible from other files) */
  CONTEXT_NORMAL   /* Label is local to this file */
} labelContext;

/* Address Reference Encoding (ARE) flags for machine words */
typedef enum {
  A,        /* Absolute value */
  R,        /* Relocatable address */
  E,        /* External address */
  ARE_NONE  /* No ARE flags set */
} commandARE;

/**
 * Entry in the symbol table
 *
 * Contains information about a label, including its name, address,
 * type, context, and any references to it from external labels.
 */
typedef struct{
  char *name;              /* Label name */
  int address;             /* Memory address of the label */
  labelType type;          /* Type of the label (code or data) */
  labelContext context;    /* Context of the label (normal, extern, entry) */

  /* Dynamic array to track addresses where this label is mentioned */
  int *ext_references;     /* Array of word addresses where this label is referenced */
  int ext_ref_count;       /* Number of references to this label */
  int ext_ref_capacity;    /* Current capacity of the ext_references array */
} symbol;

/*  Structure to hold the symbol table and its metadata */
typedef struct {
  symbol *symbols;    /* Array of symbol entries */
  int size;           /* Current number of symbols in the table */
  int capacity;       /* Total capacity of the symbols array */
} symbolTable;

/**
 * Entry in the translation table
 *
 * Contains information about a machine word, including its address,
 * the source code that generated it, and a linked list of words.
 */
typedef struct{
  int address;        /* Memory address */
  char *source_code;  /* Source code that generated this entry */
  wordNode *node;     /* Linked list of machine words */
} transTable;

/* Function prototypes - rest of the file remains the same */
/**
 * Looks up a command by name in the command table
 *
 * @param cmd_name (in) Name of the command to look up
 * @return Pointer to the command semantics, or NULL if not found
 */
commandSem *command_lookup(char *cmd_name);

/**
 * Creates a new symbol table
 *
 * @return Pointer to the newly created symbol table
 */
symbolTable* create_symbol_table();

/**
 * Resizes the symbol table when it gets too full
 *
 * @param table (in/out) Symbol table to resize
 * @return 1 if successful, 0 if memory allocation failed
 */
int resize_symbol_table(symbolTable *table);

/**
 * Inserts a new symbol into the symbol table
 *
 * @param table (in/out) Symbol table to insert into
 * @param name (in) Name of the symbol
 * @param address (in) Memory address of the symbol
 * @param type (in) Type of the symbol (code or data)
 * @param context (in) Context of the symbol (normal, extern, entry)
 * @return 1 if successful, 0 if an error occurred
 */
int insert_symbol(symbolTable *table, char *name, int address, labelType type, labelContext context);

/**
 * Finds a symbol in the symbol table by name
 *
 * @param table (in) Symbol table to search
 * @param name (in) Name of the symbol to find
 * @return Pointer to the symbol if found, NULL otherwise
 */
symbol* find_symbol(symbolTable *table, char *name);

/**
 * Updates the address and/or context of a symbol
 *
 * @param symbol_table (in/out) Symbol table containing the symbol
 * @param symbol_name (in) Name of the symbol to update
 * @param new_address (in) New address for the symbol
 * @param new_context (in) New context for the symbol
 * @return 1 if successful, 0 if the symbol was not found
 */
int update_symbol_address(symbolTable *symbol_table, char *symbol_name, int new_address, int new_context);

/**
 * Checks if there are any missing symbols in the symbol table
 *
 * @param table (in) Symbol table to check
 * @return Number of missing symbols, or 0 if all symbols are defined
 */
int is_missing_symbols(symbolTable *table);

/**
 * Initializes an entry in the translation table
 *
 * @param entry (out) Entry to initialize
 * @param address (in) Memory address for this entry
 * @param source_code (in) Source code that generated this entry
 */
void initialize_transTable_entry(transTable *entry, int address, char *source_code);

/**
 * Creates a new translation table with the specified initial size
 *
 * @param initial_size (in) Initial size of the translation table
 * @return Pointer to the newly created translation table
 */
transTable *create_transTable(int initial_size);

/**
 * Frees memory allocated for a translation table entry
 *
 * @param entry (in) Entry to free
 */
void free_transTable_entry(transTable *entry);

/**
 * Frees memory allocated for an entire translation table
 *
 * @param table (in) Translation table to free
 * @param size (in) Size of the translation table
 */
void free_transTable(transTable *table, int size);

/**
 * Inserts a command entry into the translation table
 *
 * @param table (in/out) Translation table to insert into
 * @param index (in) Index in the table where to insert
 * @param address (in) Memory address for this entry
 * @param source_code (in) Source code that generated this entry
 * @param opcode (in) Operation code
 * @param src_mode (in) Source addressing mode
 * @param src_reg (in) Source register
 * @param dst_mode (in) Destination addressing mode
 * @param dst_reg (in) Destination register
 * @param funct (in) Function code
 */
void insert_command_entry(transTable *table, int index, int address, char *source_code,
                         int opcode, int src_mode, int src_reg, int dst_mode, int dst_reg, int funct);

/**
 * Inserts an extra word (operand) into the translation table
 *
 * @param table (in/out) Translation table to insert into
 * @param index (in) Index in the table where to insert
 * @param address (in) Memory address for this entry
 * @param source_code (in) Source code that generated this entry
 * @param op_type (in) Operand type
 * @param value (in) Value or address
 * @param are_fields (in) ARE flags for this word
 * @return 1 if successful, 0 if an error occurred
 */
int insert_extra_word(transTable *table, int index, int address, char *source_code, int op_type, int value, commandARE are_fields);

/**
 * Validates if a command is valid
 *
 * @return 1 if the command is valid, 0 otherwise
 */
int is_valid_command();

/**
 * Prints the symbol table (for debugging)
 *
 * @param table (in) Symbol table to print
 */
void print_symbol_table(const symbolTable *table);

/**
 * Updates a machine word with a new value and ARE flags
 *
 * @param node_ptr (in/out) Pointer to the word node to update
 * @param value (in) New value
 * @param are_flags (in) New ARE flags
 */
void update_word(wordNode *node_ptr, int value, commandARE are_flags);

/**
 * Initializes the external references array in a symbol
 *
 * @param sym (in/out) Symbol to initialize
 */
void init_ext_references(symbol *sym);

/**
 * Adds an external reference to a symbol
 *
 * @param sym (in/out) Symbol to add the reference to
 * @param address (in) Address where the reference occurs
 * @return 1 if successful, 0 if memory allocation failed
 */
int add_ext_reference(symbol *sym, int address);

/**
 * Frees memory allocated for external references in a symbol
 *
 * @param sym (in) Symbol containing the references to free
 */
void free_ext_references(symbol *sym);

/**
 * Frees memory allocated for an entire symbol table
 *
 * @param table (in) Symbol table to free
 */
void free_symbol_table(symbolTable *table);

#endif
