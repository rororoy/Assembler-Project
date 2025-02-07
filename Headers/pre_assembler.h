
int pre_assembler(char *filename);

int check_for_macro(char *line);

int process_macro_definition(FILE *in, hashBucket *ht_bucket, int *line_count);
