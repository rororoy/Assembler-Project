#ifndef UTILS_H
#define UTILS_H

#include "../Headers/global.h"
#include "../Headers/translate.h"

/**
 * Checks if memory allocation was successful
 *
 * Utility function to verify the result of malloc, calloc, or realloc.
 *
 * @param ptr (in) Pointer returned by memory allocation function
 * @return 1 if memory allocation was successful, 0 otherwise
 */
int check_malloc(void *ptr);

/**
 * Checks if a line is empty or contains only whitespace
 *
 * @param line (in) Line to check
 * @return 1 if the line is empty, 0 otherwise
 */
int empty_line(char *line);

/**
 * Checks if a line is a comment line (starts with semicolon)
 *
 * @param line (in) Line to check
 * @return 1 if the line is a comment, 0 otherwise
 */
int is_comment_line(const char *line);

/**
 * Duplicates a string (implementation of standard strdup)
 *
 * Creates a new copy of a string, allocating memory for it.
 *
 * @param s (in) String to duplicate
 * @return Pointer to the newly allocated string, or NULL if memory allocation failed
 */
char *strdup(char *s);

/**
 * Appends an extension to a filename
 *
 * Creates a new string containing the filename with the specified extension.
 *
 * @param filename (in) Base filename
 * @param extension (in) Extension to append (including the dot)
 * @return Pointer to the new filename with extension, or NULL if memory allocation failed
 */
char* append_extension(char *filename, const char *extension);

/**
 * Extracts a substring between two pointers
 *
 * Creates a new string containing the characters between start and end.
 *
 * @param start (in) Pointer to the first character of the substring
 * @param end (in) Pointer to the character after the last character of the substring
 * @return Pointer to the new substring, or NULL if memory allocation failed
 */
char *get_substring(char *start, char *end);

/**
 * Tokenizes a line into an array of strings
 *
 * Splits a line at whitespace and commas, handling string literals appropriately.
 *
 * @param line (in) Line to tokenize
 * @param tokens (out) Array to store the tokens
 * @param macro_scan (in) Flag indicating if this is for macro scanning (affects handling of certain tokens)
 * @return Number of tokens found
 */
int tokanize_line(char *line, char *tokens[MAX_LINE_LENGTH], int macro_scan);

/**
 * Joins an array of tokens into a single string
 *
 * Creates a new string containing all tokens joined with spaces.
 *
 * @param tokens (in) Array of tokens to join
 * @return Pointer to the joined string, or NULL if memory allocation failed
 */
char *join_tokens(char **tokens);

/**
 * Gets the register number from a register token
 *
 * Extracts the numeric part from a register token (e.g., "r3" -> 3).
 *
 * @param reg_token (in) Register token (e.g., "r0", "r1", etc.)
 * @return Register number, or -1 if invalid
 */
int get_register_number(char *reg_token);

/**
 * Calculates the word position for an operand
 *
 * Determines which word in a multi-word instruction should contain the operand.
 *
 * @param is_source (in) Flag indicating if this is a source operand (1) or destination operand (0)
 * @param cmnd (in) Command semantics information
 * @param operand_src_type (in) Source operand addressing mode
 * @return The word position (0-based index)
 */
int calculate_word_position(int is_source, commandSem *cmnd, int operand_src_type);

/**
 * Converts a word to a hexadecimal string based on its type
 *
 * Formats a machine word as a hexadecimal string, with specific handling
 * based on whether it's a first word, data entry, etc.
 *
 * @param word_data (in) The word to convert
 * @param is_first_word (in) Flag indicating if this is the first word in a linked list
 * @param is_data_entry (in) Flag indicating if this is a data entry
 * @param hex_str (out) The output buffer for the hex string (at least 7 bytes)
 */
void word_to_hex_by_type(word word_data, int is_first_word, int is_data_entry, char *hex_str);

#endif
