#include "../Headers/hash_table.h"

/**
 * Performs the pre-assembly stage processing macros
 *
 * This function processes an assembly source file (.as) to:
 * 1. Identify and store macro definitions
 * 2. Replace macro calls with their expanded content
 * 3. Generate an expanded assembly file (.am)
 *
 * @param filename (in) The name of the source file without extension
 * @param macro_table (in) Hash table to store macro definitions
 * @return 1 if successful, 0 if an error occurred
 */
int pre_assembler(char *filename, hashTable *macro_table);

/**
 * Checks if a line contains a macro definition
 *
 * Determines if a given line starts a macro definition (mcro keyword)
 * or ends a macro definition (endmcro keyword).
 *
 * @param line (in) The line to check
 * @return 1 if the line contains "mcro", 2 if it contains "endmcro", 0 otherwise
 */
int check_for_macro(char *line);

/**
 * Properly closes files and handles cleanup on exit
 *
 * Closes the input and output files and performs any necessary cleanup
 * when pre-assembly processing is complete or encounters an error.
 *
 * @param in_file (in) Pointer to the input source file
 * @param am_file (in) Pointer to the output expanded assembly file
 * @param filename1 (in) Name of the input file
 * @param filename2 (in) Name of the output file
 */
void handle_exit(FILE *in_file, FILE *am_file, char *filename1, char *filename2);

/**
 * Processes a macro definition and stores it in the hash table
 *
 * Reads lines from the file until the end of the macro definition is reached,
 * and stores the macro's content in the provided hash bucket.
 *
 * @param file (in) Pointer to the source file
 * @param ht_bucket (out) Hash bucket where the macro definition will be stored
 * @return 1 if successful, 0 if an error occurred
 */
int process_macro_definition(FILE *file, hashBucket *ht_bucket);
