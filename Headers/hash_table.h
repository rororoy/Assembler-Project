#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

#define HASH_TABLE_INITIAL_SIZE 101 /* Initial prime num size of the hash table */
#define LOAD_FACTOR_THERSHOLD 0.7 /* Threshould for resizing table */
#define DBJ2_INITIAL_HASH 5381

/* Define the bucket types for clear distinction */
typedef enum {
  BUCKET_MACRO,
  BUCKET_PENDING_LABEL
} bucketType;

/* Main bucket structure with backward compatibility */
typedef struct {
  /* Common fields */
  bucketType type;
  int is_taken;

  /* Original fields - kept for backward compatibility */
  char *macro_name;
  int length;
  node *code_nodes;

  /* New fields for pending labels */
  char *label_name;    /* Name of the unresolved label */
  int command_index;   /* Index in the command table */
  int word_number;     /* Which word is missing (1, 2, or 3) */
} hashBucket;

typedef struct {
  hashBucket *bucket;
  int count;
  int size;
} hashTable;


/* Original hash table functions */
hashTable *make_hash_table(int size);

unsigned int hash_function(char *name);

hashBucket *insert_entry(hashTable *ht, char *name);

hashTable *resize_table(hashTable *old_ht);

hashBucket *get_entry(hashTable *ht, char *name);

void free_hash_table(hashTable *ht);

hashBucket *search_table(hashTable *ht, char *name);

/* New functions for pending label operations */
hashBucket *insert_pending_label(hashTable *ht, char *label_name, int command_index, int word_number);

void resolve_pending_labels(hashTable *ht, char *label_name, int resolved_address,
                           void (*update_command)(int cmd_idx, int word_num, int address));

int get_pending_labels_count(hashTable *ht);

void print_pending_labels(hashTable *ht);

#endif /* HASH_TABLE_H */
