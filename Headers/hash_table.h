#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "linked_list.h"

/* Using a prime number for better distribution of hash values */
#define HASH_TABLE_INITIAL_SIZE 101

/* When the table reaches this fill ratio, it will be resized */
#define LOAD_FACTOR_THERSHOLD 0.7

/* Initial const hash value for the DBJ2 hash function */
#define DBJ2_INITIAL_HASH 5381

/* Distinguishes between buckets storing macros and pending labels */
typedef enum {
  BUCKET_MACRO,          /* Bucket stores a macro definition */
  BUCKET_PENDING_LABEL   /* Bucket stores a pending label reference */
} bucketType;

/* Can store either macro definitions or pending label references */
typedef struct {
  /* Common fields */
  bucketType type;      /* Type of this bucket (macro or pending label) */
  int is_taken;         /*  Flag indicating if this bucket is used */

  /* Original fields - kept for backward compatibility */
  char *macro_name;     /* Name of the macro if this is a macro bucket */
  int length;           /*  Length of the macro code */
  node *code_nodes;     /*  Linked list of macro code lines */

  /* New fields for pending labels */
  char *label_name;     /* Name of the unresolved label */
  int command_index;    /* Index in the command table */
  int word_number;      /* Which word is missing (1, 2, or 3) */
  int addr;             /* Address where the label is referenced */
} hashBucket;

/* Contains an array of buckets and metadata about the table */
typedef struct {
  hashBucket *bucket;   /* Array of buckets */
  int count;            /*  Number of occupied buckets */
  int size;             /*  Total size of the hash table */
} hashTable;


/*********** Original hash table functions ***********/

/** Allocates memory for a new hash table and initializes it.
 *
 * @param size (in) Initial size of the hash table
 * @return Pointer to the new hash table, or NULL if memory allocation failed
 */
hashTable *make_hash_table(int size);

/**  Uses the DBJ2 hash algorithm to generate a hash value.
 *
 * @param name (in) String to hash
 * @return Hash value for the string
 */
unsigned int hash_function(char *name);

/**
 * Inserts a new entry - Creates a new bucket for a macro or other entry.
 *
 * @param ht (in) Pointer to the hash table
 * @param name (in) Name for the new entry
 * @return Pointer to the newly created bucket, or NULL if insertion failed
 */
hashBucket *insert_entry(hashTable *ht, char *name);

/**
 * Creates a new, larger hash table and transfers all entries from the old table.
 *
 * @param old_ht (in) Pointer to the hash table to resize
 * @return Pointer to the new, resized hash table
 */
hashTable *resize_table(hashTable *old_ht);

/**
 * Gets an entry from the hash table by name
 *
 * @param ht (in) Pointer to the hash table
 * @param name (in) Name of the entry to find
 * @return Pointer to the found bucket, or NULL if not found
 */
hashBucket *get_entry(hashTable *ht, char *name);

/**
 * Frees all memory allocated for a hash table
 *
 * @param ht (in) Pointer to the hash table to free
 */
void free_hash_table(hashTable *ht);

/**
 * Searches for an entry in the hash table by name
 *
 * @param ht (in) Pointer to the hash table
 * @param name (in) Name of the entry to find
 * @return Pointer to the found bucket, or NULL if not found
 */
hashBucket *search_table(hashTable *ht, char *name);

/*******   Functions for pending label operations *******/
/* This is the second type of hashtable used for storing labels that
need to be resolved by the end of the second stage */

/**
 * Used during assembly when a label reference is encountered before the label definition.
 *
 * @param ht (in) Pointer to the hash table
 * @param label_name (in) Name of the unresolved label
 * @param command_index (in) Index of the command in the command table
 * @param word_number (in) Which word number (1, 2, or 3) contains the reference
 * @param addr (in) Address where the label is referenced
 * @return Pointer to the newly created bucket, or NULL if insertion failed
 */
hashBucket *insert_pending_label(hashTable *ht, char *label_name, int command_index, int word_number, int addr);

/**
 * Called when a label definition is encountered to update all references to that label.
 *
 * @param ht (in) Pointer to the hash table
 * @param label_name (in) Name of the resolved label
 * @param resolved_address (in) Address of the label definition
 * @param update_command (in) Function pointer to update commands that reference the label
 */
void resolve_pending_labels(hashTable *ht, char *label_name, int resolved_address,
                           void (*update_command)(int cmd_idx, int word_num, int address));

/**
 * Counts the number of pending (unresolved) labels
 *
 * @param ht (in) Pointer to the hash table
 * @return Number of pending labels
 */
int get_pending_labels_count(hashTable *ht);


#endif
