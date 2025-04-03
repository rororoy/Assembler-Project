#include "./translate.h"
#include "./linked_list.h"
#include "./hash_table.h"

int first_pass(char *filename, hashTable *macro_table);


/*
 * Main function to process assembly commands
 *
 * Parameters:
 * pending_labels - Hash table of labels referenced but not yet defined
 * translation_table - Translation table for machine code
 * tablepointer - Pointer to the next available position in the translation table
 * tokens - Array of tokenized command strings
 * IC - Current instruction counter
 * operand_src_type - Source operand addressing mode (0=immediate, 1=direct, 2=relative, 3=register)
 * operand_dst_type - Destination operand addressing mode (0=immediate, 1=direct, 2=relative, 3=register)
 * command_start - Index in tokens where the command starts (to handle labels)
 * symbol_table - Table of all symbols (labels) and their addresses
 */
void process_assembly_command(hashTable *pending_labels, transTable *translation_table, int *tablepointer,
                             char **tokens, int IC, int operand_src_type, int operand_dst_type,
                             int command_start, symbolTable *symbol_table);

/*
 * Helper function to calculate correct word position based on command structure
 *
 * Parameters:
 * is_source - Flag indicating if this is a source operand (1) or destination operand (0)
 * cmnd - Command semantics information
 * operand_src_type - Source operand addressing mode
 *
 * Returns:
 * int - The correct word position (0-based index)
 */
int calculate_word_position(int is_source, commandSem *cmnd, int operand_src_type);

/*
 * Helper function to process directive commands (.data, .string, .extern, .entry)
 *
 * Parameters:
 * pending_labels - Hash table of labels referenced but not yet defined
 * translation_table - Translation table for machine code
 * tablepointer - Pointer to the next available position in the translation table
 * tokens - Array of tokenized command strings
 * IC - Current instruction counter
 * command_start - Index in tokens where the command starts
 * symbol_table - Table of all symbols (labels) and their addresses
 * source_line - Original source line as a string
 */
void process_directive(hashTable *pending_labels, transTable *translation_table, int *tablepointer,
                             char **tokens, int IC, int command_start, symbolTable *symbol_table,
                             char *source_line);

/*
 * Helper function to process immediate addressing mode (#value)
 *
 * Parameters:
 * translation_table - Translation table for machine code
 * tablepointer - Current position in the translation table
 * IC - Current instruction counter
 * tokens - Array of tokenized command strings
 * command_start - Index in tokens where the command starts
 * is_source - Flag indicating if this is a source operand (1) or destination operand (0)
 * source_line - Original source line as a string
 */
void process_immediate_addressing(transTable *translation_table, int tablepointer, int IC,
                                        char **tokens, int command_start, int is_source,
                                        char *source_line);

/*
 * Helper function to process direct addressing mode (label)
 *
 * Parameters:
 * pending_labels - Hash table of labels referenced but not yet defined
 * translation_table - Translation table for machine code
 * tablepointer - Current position in the translation table
 * IC - Current instruction counter
 * tokens - Array of tokenized command strings
 * command_start - Index in tokens where the command starts
 * is_source - Flag indicating if this is a source operand (1) or destination operand (0)
 * symbol_table - Table of all symbols (labels) and their addresses
 * cmnd - Command semantics information
 * source_line - Original source line as a string
 * operand_src_type - Source operand addressing mode
 */
void process_direct_addressing(hashTable *pending_labels, transTable *translation_table,
                                     int tablepointer, int IC, char **tokens, int command_start,
                                     int is_source, symbolTable *symbol_table, commandSem *cmnd,
                                     char *source_line, int operand_src_type);

/*
 * Helper function to process relative addressing mode (&label)
 *
 * Parameters:
 * pending_labels - Hash table of labels referenced but not yet defined
 * translation_table - Translation table for machine code
 * tablepointer - Current position in the translation table
 * IC - Current instruction counter
 * tokens - Array of tokenized command strings
 * command_start - Index in tokens where the command starts
 * is_source - Flag indicating if this is a source operand (1) or destination operand (0)
 * symbol_table - Table of all symbols (labels) and their addresses
 * cmnd - Command semantics information
 * source_line - Original source line as a string
 * operand_src_type - Source operand addressing mode
 */
void process_relative_addressing(hashTable *pending_labels, transTable *translation_table,
                                       int tablepointer, int IC, char **tokens, int command_start,
                                       int is_source, symbolTable *symbol_table, commandSem *cmnd,
                                       char *source_line, int operand_src_type);


int handle_undefined_label(hashTable *pending_labels, char *label_name, int current_command_index, int word_position, int IC);
