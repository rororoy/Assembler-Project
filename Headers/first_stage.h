#include "./translate.h"
#include "./linked_list.h"
#include "./hash_table.h"

int first_pass(char *filename);

void process_assembly_command(hashTable *pending_labels, transTable *my_table, int *tablepointer, char **tokens, int IC,
                               int operand_src_type, int operand_dst_type, int command_start, symbolTable *symbol_table);

int handle_undefined_label(hashTable *pending_labels, char *label_name, int current_command_index, int word_position);
