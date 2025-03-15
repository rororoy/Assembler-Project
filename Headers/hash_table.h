#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

#define HASH_TABLE_INITIAL_SIZE 101 /* Initial prime num size of the hash table */
#define LOAD_FACTOR_THERSHOLD 0.7 /* Threshould for resizing table */
#define  DBJ2_INITIAL_HASH 5381

typedef struct {
  char *macro_name;
  int length;
  int is_taken;
  node *code_nodes;
} hashBucket;

typedef struct {
  hashBucket *bucket;
  int count;
  int size;
}hashTable;


hashTable *make_hash_table(int size);

unsigned int hash_function(char *name);

hashBucket *insert_entry(hashTable *ht, char *name);

hashTable *resize_table(hashTable *old_ht);

hashBucket *get_entry(hashTable *ht, char *name);

void free_hash_table(hashTable *ht);

hashBucket *search_table(hashTable *ht, char *name);

#endif
