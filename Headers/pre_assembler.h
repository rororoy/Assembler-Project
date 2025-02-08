#include "../Headers/hash_table.h"

int pre_assembler(char *filename);

int check_for_macro(char *line);

void handle_exit(FILE *in_file, FILE *temp_file, hashTable *ht, char *filename1, char *filename2);

int process_macro_definition(FILE *file, hashBucket *ht_bucket, int *line_count);
