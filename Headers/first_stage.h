#ifndef FIRST_STAGE_H
#define FIRST_STAGE_H

#include "./global.h"
#include "./translate.h"
#include "./linked_list.h"
#include "./hash_table.h"

/**
 * Cleans up resources allocated during first_pass
 *
 * Frees memory and closes files that were used during the first pass of assembly.
 *
 * @param file (in) File pointer to close
 * @param am_filename (in) Filename to free
 * @param symbol_table (in) Symbol table to free
 * @param pending_labels (in) Pending labels hash table to free
 * @param translation_table (in) Translation table to free
 * @param tablepointer (in) Size of the translation table
 */
void cleanup_first_pass(FILE *file, char *am_filename,
                        symbolTable *symbol_table,
                        hashTable *pending_labels,
                        transTable *translation_table,
                        int tablepointer);

/**
 * @brief Performs the first pass of the assembly process
 *
 * This function processes an expanded assembly file (.am) to:
 * 1. Process all labels and commands
 * 2. Create a symbol table for all defined labels
 * 3. Generate machine code for all instructions
 * 4. Record any undefined labels for resolution in the second pass
 *
 * @param filename (in) The name of the source file without extension
 * @param macro_table (in) Hash table containing macro definitions from pre-assembly
 * @return 1 if successful, 0 if an error occurred
 */
int first_pass(char *filename, hashTable *macro_table);


/**
 * Processes an assembly command and generates the corresponding machine code
 *
 * Handles the translation of assembly commands to machine code words and
 * stores them in the translation table.
 *
 * @param pending_labels (in/out) Hash table of labels referenced but not yet defined
 * @param translation_table (out) Translation table for storing machine code
 * @param tablepointer (in/out) Pointer to the next available position in the translation table
 * @param tokens (in) Array of tokenized command strings
 * @param IC (in) Current instruction counter
 * @param operand_src_type (in) Source operand addressing mode (0=immediate, 1=direct, 2=relative, 3=register)
 * @param operand_dst_type (in) Destination operand addressing mode (0=immediate, 1=direct, 2=relative, 3=register)
 * @param command_start (in) Index in tokens where the command starts (to handle labels)
 * @param symbol_table (in) Table of all symbols (labels) and their addresses
 */
void process_assembly_command(hashTable *pending_labels, transTable *translation_table, int *tablepointer,
                             char **tokens, int IC, int operand_src_type, int operand_dst_type,
                             int command_start, symbolTable *symbol_table);

/**
 * Calculates the correct word position based on command structure
 *
 * Determines which word in a multi-word instruction should contain a particular operand
 * based on the command type and addressing modes.
 *
 * @param is_source (in) Flag indicating if this is a source operand (1) or destination operand (0)
 * @param cmnd (in) Command semantics information
 * @param operand_src_type (in) Source operand addressing mode
 * @return The correct word position (0-based index)
 */
int calculate_word_position(int is_source, commandSem *cmnd, int operand_src_type);

/**
 * Processes directive commands (.data, .string, .extern, .entry)
 *
 * Handles special directives that define data, strings, or specify external/entry points.
 *
 * @param pending_labels (in/out) Hash table of labels referenced but not yet defined
 * @param translation_table (out) Translation table for machine code
 * @param tablepointer (in/out) Pointer to the next available position in the translation table
 * @param tokens (in) Array of tokenized command strings
 * @param IC (in) Current instruction counter
 * @param command_start (in) Index in tokens where the command starts
 * @param symbol_table (in/out) Table of all symbols (labels) and their addresses
 * @param source_line (in) Original source line as a string
 */
void process_directive(hashTable *pending_labels, transTable *translation_table, int *tablepointer,
                             char **tokens, int IC, int command_start, symbolTable *symbol_table,
                             char *source_line);

/**
 * Processes immediate addressing mode (#value)
 *
 * Handles operands specified as immediate values (e.g., #123) and generates
 * the appropriate machine code.
 *
 * @param translation_table (out) Translation table for machine code
 * @param tablepointer (in) Current position in the translation table
 * @param IC (in) Current instruction counter
 * @param tokens (in) Array of tokenized command strings
 * @param command_start (in) Index in tokens where the command starts
 * @param is_source (in) Flag indicating if this is a source operand (1) or destination operand (0)
 * @param source_line (in) Original source line as a string
 */
void process_immediate_addressing(transTable *translation_table, int tablepointer, int IC,
                                        char **tokens, int command_start, int is_source,
                                        char *source_line);

/**
 * Processes direct addressing mode (label)
 *
 * Handles operands specified as label references and generates the appropriate machine code.
 * If the label is not yet defined, it's added to the pending labels table.
 *
 * @param pending_labels (in/out) Hash table of labels referenced but not yet defined
 * @param translation_table (out) Translation table for machine code
 * @param tablepointer (in) Current position in the translation table
 * @param IC (in) Current instruction counter
 * @param tokens (in) Array of tokenized command strings
 * @param command_start (in) Index in tokens where the command starts
 * @param is_source (in) Flag indicating if this is a source operand (1) or destination operand (0)
 * @param symbol_table (in) Table of all symbols (labels) and their addresses
 * @param cmnd (in) Command semantics information
 * @param source_line (in) Original source line as a string
 * @param operand_src_type (in) Source operand addressing mode
 */
void process_direct_addressing(hashTable *pending_labels, transTable *translation_table,
                                     int tablepointer, int IC, char **tokens, int command_start,
                                     int is_source, symbolTable *symbol_table, commandSem *cmnd,
                                     char *source_line, int operand_src_type);

/**
 * Processes relative addressing mode (&label)
 *
 * Handles operands specified as relative label references (e.g., &LOOP) and
 * generates the appropriate machine code. If the label is not yet defined,
 * it's added to the pending labels table.
 *
 * @param pending_labels (in/out) Hash table of labels referenced but not yet defined
 * @param translation_table (out) Translation table for machine code
 * @param tablepointer (in) Current position in the translation table
 * @param IC (in) Current instruction counter
 * @param tokens (in) Array of tokenized command strings
 * @param command_start (in) Index in tokens where the command starts
 * @param is_source (in) Flag indicating if this is a source operand (1) or destination operand (0)
 * @param symbol_table (in) Table of all symbols (labels) and their addresses
 * @param cmnd (in) Command semantics information
 * @param source_line (in) Original source line as a string
 * @param operand_src_type (in) Source operand addressing mode
 */
void process_relative_addressing(hashTable *pending_labels, transTable *translation_table,
                                       int tablepointer, int IC, char **tokens, int command_start,
                                       int is_source, symbolTable *symbol_table, commandSem *cmnd,
                                       char *source_line, int operand_src_type);

/**
 * Handles undefined label references
 *
 * Adds an undefined label to the pending labels table to be resolved in the second pass.
 *
 * @param pending_labels (in/out) Hash table of labels referenced but not yet defined
 * @param label_name (in) Name of the undefined label
 * @param current_command_index (in) Index of the current command in the translation table
 * @param word_position (in) Which word in the command needs the label's address
 * @param IC (in) Current instruction counter
 * @return 1 if successful, 0 if an error occurred
 */
int handle_undefined_label(hashTable *pending_labels, char *label_name, int current_command_index, int word_position, int IC);

#endif
