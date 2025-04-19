#ifndef VALIDATE_H
#define VALIDATE_H

#include "../Headers/global.h"
#include "../Headers/translate.h"

/**
 * This structure holds the addressing modes for the source and destination operands
 * of an assembly instruction.
 */
typedef struct{
  int destination_op; /**< Addressing mode of the destination operand */
  int source_op;      /**< Addressing mode of the source operand */
} addressModes;

/**
 *  Validates if an assembly line is within the maximum length limit
 *
 * Checks if the given line from the assembly file falls within the 80 character length limit.
 *
 * @param line (in) Pointer to the line to be validated
 * @return 1 if the line length is valid, 0 if too long
 */
int valid_length_line(char *line);

/**
 *  Checks if a string is a reserved word in the assembly language
 *
 * Validates whether the given string is one of the predefined reserved words
 * that cannot be used as labels or other identifiers.
 *
 * @param str (in) String to check
 * @return 1 if the string is a reserved word, 0 otherwise
 */
int is_saved_word(char *str);

/**
 *  Validates if a string can be used as a label
 *
 * Checks if the given string follows the rules for a valid label:
 * - Starts with a letter
 * - Contains only alphanumeric characters
 * - Maximum length of 32 characters
 * - Is not a reserved word
 *
 * @param tok (in) Token to validate as a label
 * @return 1 if the string is a valid label, 0 otherwise
 */
int valid_label(char *tok);

/**
 *  Validates if a command is valid and retrieves its addressing modes
 *
 * Checks if the specified command is valid, and if its operands are correctly formed.
 * Determines the addressing modes for the operands and stores them in the operands parameter.
 *
 * @param command_start (in) Index of the command in the tokens array
 * @param tokens (in) Array of tokens from the parsed assembly line
 * @param operands (out) Pointer to structure where addressing modes will be stored
 * @return The number of operands if command is valid, 0 if invalid
 */
int is_valid_command(int command_start, char *tokens[MAX_LINE_LENGTH], addressModes *operands);

/**
 *  Validates operands for a given command
 *
 * Checks if the number and types of operands match what is expected for the command.
 * Determines the addressing modes for each operand and verifies they are valid.
 *
 * @param command_start (in) Index of the command in the tokens array
 * @param tokens (in) Array of tokens from the parsed assembly line
 * @param correct_operands (in) Expected number of operands for this command
 * @param operands_adress (out) Pointer to structure where addressing modes will be stored
 * @param command_type (in) Type of command (1=two operands, 2=one operand, 3=no operands, 4=directive)
 * @return 1 if the operands are valid, 0 otherwise
 */
int check_operands(int command_start, char *tokens[MAX_LINE_LENGTH], int correct_operands, addressModes *operands_adress, int command_type);

/**
 *  Prints the contents of a translation table for debugging
 *
 * Displays the contents of the translation table, which contains the machine code
 * generated from the assembly code.
 *
 * @param table (in) Pointer to the translation table to print
 * @param size (in) Number of entries in the table
 */
void print_transTable(transTable *table, int size);

/**
 *  Checks if a word is a valid register name
 *
 * Determines if the given word matches one of the valid register names
 * defined in the assembly language (e.g., r0, r1, etc.).
 *
 * @param word (in) String to check
 * @return 1 if the word is a valid register name, 0 otherwise
 */
int is_reg(char *word);

#endif
