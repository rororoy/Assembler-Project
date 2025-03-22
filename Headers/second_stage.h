#include "../Headers/global.h"
#include "../Headers/hash_table.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"

/**
 * Performs the second pass of the assembler, resolving all pending labels.
 *
 * @param filename The name of the source file.
 * @param pending_labels Hash table of pending labels to be resolved.
 * @param translation_table The translation table containing assembled code.
 * @param symbol_table The symbol table containing all defined symbols.
 * @return 1 if successful, 0 if an error occurred.
 */
int second_pass(char *filename, hashTable *pending_labels, transTable *translation_table, symbolTable *symbol_table);

/**
 * Resolves a word in the translation table based on symbol information.
 *
 * @param pending_entry The pending label entry from the hash table.
 * @param translation_table The translation table.
 * @param symbol_entry The symbol information from the symbol table.
 * @param node_ptr Pointer to the word node to be resolved.
 * @param IS_RELATIVE_LABEL Flag indicating if this is a relative addressing mode.
 * @return 1 if successful, 0 if an error occurred.
 */
int resolve_word(hashBucket *pending_entry, transTable *translation_table, symbol *symbol_entry, wordNode *node_ptr, int IS_RELATIVE_LABEL);

/**
 * Creates the output files (.ob, .ext, .ent) for the assembled program.
 *
 * @param filename Base filename for output files.
 * @param translation_table The translation table containing the assembled code.
 * @param symbol_table The symbol table for resolving labels.
 * @return 1 if successful, 0 if an error occurred.
 */
int create_output_files(char *filename, transTable *translation_table, symbolTable *symbol_table);

/**
 * Generates the object (.ob) file from the translation table.
 * The file format is: IC and DC on first line, followed by addresses and hex words.
 *
 * @param file The output file pointer.
 * @param translation_table The translation table containing the assembled code.
 * @return 1 if successful, 0 if an error occurred.
 */
int generate_ob_file(FILE *file, transTable *translation_table);

/**
 * Generates the externals (.ext) file listing all external references.
 *
 * @param file The output file pointer.
 * @param symbol_table The symbol table containing all defined symbols.
 * @return 1 if successful, 0 if an error occurred.
 */
int generate_ext_file(FILE *file, symbolTable *symbol_table);

/**
 * Generates the entries (.ent) file listing all entry symbols.
 *
 * @param file The output file pointer.
 * @param symbol_table The symbol table containing all defined symbols.
 * @return 1 if successful, 0 if an error occurred.
 */
int generate_ent_file(FILE *file, symbolTable *symbol_table);
