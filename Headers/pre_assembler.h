#include "../Headers/hash_table.h"

int pre_assembler(char *filename);

int check_for_macro(char *line);

void exit_function(FILE *in_file, FILE *temp_file, hashTable *ht);

int process_macro_definition(FILE *file, hashBucket *ht_bucket, int *line_count);
