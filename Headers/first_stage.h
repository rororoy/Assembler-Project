#include "./translate.h"
#include "./linked_list.h"
#include "./hash_table.h"

int first_pass(char *filename);

int handle_undefined_label(hashTable *pending_labels, char *label_name, int current_command_index, int word_position);

/**
 * Process an assembly command and update the translation table
 * @param pending_labels Hash table of pending label references
 * @param my_table Translation table for code generation
 * @param tablepointer Pointer to current index in translation table
 * @param tokens Array of tokens from the assembly line
 * @param IC Current instruction counter
 * @param operand_src_type Addressing type of source operand
 * @param operand_dst_type Addressing type of destination operand
 * @param command_start Index of command token in tokens array
 * @param symbol_table Symbol table for label lookups
 */
void process_assembly_command(hashTable *pending_labels, transTable *my_table,
                             int *tablepointer, char **tokens, int IC,
                             int operand_src_type, int operand_dst_type,
                             int command_start, symbolTable *symbol_table);

/**
 * Process directive commands (.data, .string, etc.)
 * @param my_table Translation table for code generation
 * @param tablepointer Pointer to current index in translation table
 * @param tokens Array of tokens from the assembly line
 * @param command_start Index of directive token in tokens array
 * @param IC Current instruction counter
 * @param source_line Complete source line as a string
 */
void process_directive(transTable *my_table, int *tablepointer, char **tokens,
                       int command_start, int IC, char *source_line);

/**
 * Process an instruction (non-directive) command
 * @param pending_labels Hash table of pending label references
 * @param my_table Translation table for code generation
 * @param tablepointer Pointer to current index in translation table
 * @param tokens Array of tokens from the assembly line
 * @param IC Current instruction counter
 * @param operand_src_type Addressing type of source operand
 * @param operand_dst_type Addressing type of destination operand
 * @param command_start Index of command token in tokens array
 * @param cmnd Command semantics structure
 * @param source_line Complete source line as a string
 * @param symbol_table Symbol table for label lookups
 */
void process_instruction(hashTable *pending_labels, transTable *my_table,
                         int *tablepointer, char **tokens, int IC,
                         int operand_src_type, int operand_dst_type,
                         int command_start, commandSem *cmnd,
                         char *source_line, symbolTable *symbol_table);

/**
 * Process source operand and add extra words as needed
 * @param pending_labels Hash table of pending label references
 * @param my_table Translation table for code generation
 * @param tablepointer Current index in translation table
 * @param tokens Array of tokens from the assembly line
 * @param command_start Index of command token in tokens array
 * @param IC Current instruction counter
 * @param operand_type Addressing type of the operand
 * @param source_line Complete source line as a string
 * @param symbol_table Symbol table for label lookups
 */
void process_source_operand(hashTable *pending_labels, transTable *my_table,
                           int tablepointer, char **tokens, int command_start,
                           int IC, int operand_type, char *source_line,
                           symbolTable *symbol_table);

/**
 * Process destination operand and add extra words as needed
 * @param pending_labels Hash table of pending label references
 * @param my_table Translation table for code generation
 * @param tablepointer Current index in translation table
 * @param tokens Array of tokens from the assembly line
 * @param command_start Index of command token in tokens array
 * @param IC Current instruction counter
 * @param operand_type Addressing type of the operand
 * @param source_line Complete source line as a string
 * @param symbol_table Symbol table for label lookups
 * @param command_type Type of command (affects operand positioning)
 */
void process_destination_operand(hashTable *pending_labels, transTable *my_table,
                                int tablepointer, char **tokens, int command_start,
                                int IC, int operand_type, char *source_line,
                                symbolTable *symbol_table, int command_type);

/**
 * Handle label references and update translation table
 * @param pending_labels Hash table of pending label references
 * @param my_table Translation table for code generation
 * @param tablepointer Current index in translation table
 * @param IC Current instruction counter
 * @param source_line Complete source line as a string
 * @param label Label text to process
 * @param is_relative Flag indicating if this is a relative addressing mode
 * @param word_place Position of the word in the instruction
 * @param symbol_table Symbol table for label lookups
 */
void handle_label_reference(hashTable *pending_labels, transTable *my_table,
                           int tablepointer, int IC, char *source_line,
                           char *label, int is_relative, int word_place,
                           symbolTable *symbol_table);

/**
 * Extract register number from a register operand token
 * @param reg_token Token containing register reference (format: "rN")
 * @return Register number (0-7) or 0 if invalid
 */
int get_register_number(char *reg_token);

/**
 * Log debug information about command insertion
 * @param tablepointer Current index in translation table
 * @param IC Current instruction counter
 * @param opcode Operation code
 * @param src_type Source operand addressing type
 * @param src_reg Source register number (if applicable)
 * @param dst_type Destination operand addressing type
 * @param dst_reg Destination register number (if applicable)
 * @param funct Function code
 */
void debug_command_insert(int tablepointer, int IC, int opcode,
                         int src_type, int src_reg, int dst_type, int dst_reg, int funct);
