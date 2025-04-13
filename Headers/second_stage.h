#include "../Headers/global.h"
#include "../Headers/hash_table.h"
#include "../Headers/translate.h"
#include "../Headers/linked_list.h"

/* Structure to hold external reference information */
typedef struct {
    char *label_name;
    int address;
} ExternalReference;

/**
 * Performs the second pass of the assembler, resolving all pending labels.
 *
 * @param filename The name of the source file.
 * @param pending_labels Hash table of pending labels to be resolved.
 * @param translation_table The translation table containing assembled code.
 * @param symbol_table The symbol table containing all defined symbols.
 * @return 1 if successful, 0 if an error occurred.
 */
int second_pass(char *filename, hashTable *pending_labels, transTable *translation_table, symbolTable *symbol_table, int IC, int DC);

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
