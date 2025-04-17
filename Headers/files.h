#ifndef FILES_H
#define FILES_H

#include <stdio.h>
#include "./global.h"
#include "./hash_table.h"
#include "./linked_list.h"
#include "./translate.h"

/**
 * Cleans up output files for a given filename
 *
 * @param filename The base filename without extension
 */
void clean_output_files(char *filename);

/**
 * Generates the object (.ob) file from the translation table
 *
 * @param file The output file pointer
 * @param translation_table The translation table containing the assembled code
 * @param IC The instruction counter
 * @param DC The data counter
 * @return 1 if successful, 0 if an error occurred
 */
int generate_ob_file(FILE *file, transTable *translation_table, int IC, int DC);

/**
 * Generates the externals (.ext) file listing all external references
 *
 * @param filename The base filename
 * @param symbol_table The symbol table containing all defined symbols
 * @param pending_labels The hash table of pending labels
 */
void generate_externals_file(char *filename, symbolTable *symbol_table, hashTable *pending_labels);

/**
 * Generates the entries (.ent) file listing all entry symbols
 *
 * @param file The output file pointer
 * @param symbol_table The symbol table containing all defined symbols
 * @return 1 if successful, 0 if an error occurred
 */
int generate_ent_file(FILE *file, symbolTable *symbol_table);

/**
 * Creates the output files (.ob, .ext, .ent) for the assembled program
 *
 * @param filename Base filename for output files
 * @param translation_table The translation table containing the assembled code
 * @param symbol_table The symbol table for resolving labels
 * @param pending_labels The hash table of pending labels
 * @param IC The instruction counter
 * @param DC The data counter
 * @return 1 if successful, 0 if an error occurred
 */
int create_output_files(char *filename, transTable *translation_table,
                      symbolTable *symbol_table, hashTable *pending_labels, int IC, int DC);

#endif
