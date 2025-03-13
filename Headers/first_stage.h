#include "./translate.h"
#include "./linked_list.h"

int first_pass(char *filename);

void process_assembly_command(transTable *my_table, int *tablepointer, char **tokens, int IC,
                               int operand_src_type, int operand_dst_type);
