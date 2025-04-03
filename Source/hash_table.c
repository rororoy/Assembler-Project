#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/hash_table.h"
#include "../Headers/linked_list.h"
#include "../Headers/utils.h"
#include "../Headers/error.h"

hashTable *make_hash_table(int size){
  int i;

  hashTable *ht = malloc(sizeof(hashTable));
  check_malloc(ht);

  ht->size = size;
  ht->count = 0;
  ht->bucket = calloc(ht->size, sizeof(hashBucket));

  /* Initialize all buckets to NULL */
  for(i = 0; i < ht->size; i++){
    ht->bucket[i].type = BUCKET_MACRO; /* Default type for backward compatibility */
    ht->bucket[i].is_taken = 0;
    ht->bucket[i].macro_name = NULL;
    ht->bucket[i].code_nodes = NULL;
    ht->bucket[i].length = 0;
    ht->bucket[i].label_name = NULL;
    ht->bucket[i].command_index = 0;
    ht->bucket[i].word_number = 0;
  }

  return ht;
}

/* dbj2 hashing function */
unsigned int hash_function(char *name){
  unsigned int long hash = DBJ2_INITIAL_HASH;
  int c;

  while((c = *name++)){
    hash = ((hash << 5) + hash) +c; /* hash * 33 + c */
  }
  return (unsigned int)hash;
}

hashBucket *insert_entry(hashTable *ht, char *name){
  double load_factor;
  int index;
  int original_index;
  if(ht == NULL || name == NULL){ /* If one of the params is empty */
    print_error("Missing argument", "(insert entry)", 0); /* Recieved an empty parameter */
    return NULL;
  }

  load_factor = (double)(ht->count + 1) / ht->size;
  if(load_factor > LOAD_FACTOR_THERSHOLD){
    hashTable *new_ht = resize_table(ht);

    if(new_ht == NULL){ /* If got new table as NULL */
      return NULL;
    }

    /* Update hash table refrence */
    ht->bucket = new_ht->bucket;
    ht->size = new_ht->size;
    ht->count = new_ht->count;

    free(new_ht);
  }
  index = hash_function(name) % ht->size;
  original_index = index;

  /* Linear probing to find next empty bucket */
  while(ht->bucket[index].is_taken){
    if(ht->bucket[index].type == BUCKET_MACRO &&
       strcmp(ht->bucket[index].macro_name, name) == 0){ /* Macro exists */
      return &(ht->bucket[index]); /* Macro already exists return it*/
    }
    index = (index+1)%ht->size;
    if(index == original_index){ /* TODO CHECK IF NEED THIS COS NO WAY THIS IS FULL */
      return NULL;
    }
  }

  /* Macro isn't logged and found empty bucket */
  ht->bucket[index].type = BUCKET_MACRO;
  ht->bucket[index].macro_name = strdup(name);
  if(ht->bucket[index].macro_name == NULL){
    /* Failed to copy macro name */
    return NULL;
  }

  ht->bucket[index].code_nodes = NULL;
  ht->bucket[index].is_taken = 1;
  ht->bucket[index].length = 1;
  ht->count++;

  return &(ht->bucket[index]);
}

hashBucket *search_table(hashTable *ht, char *name){
  int index;
  int original_index;
  if(ht == NULL || name == NULL){ /* If one of the params is empty */
    print_error("Missing argument", "(search table)", 0); /* Recieved an empty parameter */
    return NULL;
  }

  index = hash_function(name) % ht->size;
  original_index = index;

  /* Linear probing to find next empty bucket */
  while(ht->bucket[index].is_taken){
    if(ht->bucket[index].type == BUCKET_MACRO &&
       strcmp(ht->bucket[index].macro_name, name) == 0){ /* Macro exists */
      return &(ht->bucket[index]); /* Macro found return it*/
    }
    index = (index+1) % ht->size;
    if(index == original_index){
       /* Didn't find the entry */
      return NULL;
    }
  }
  return NULL; /* empty */
}

hashTable *resize_table(hashTable *old_ht){
  int i;
  int new_size = old_ht->size * 2;
  hashBucket *new_bucket;
  hashTable *new_ht;

  if(old_ht == NULL){
    return NULL;
  }

  new_ht = make_hash_table(new_size);

  if(new_ht == NULL){
    print_error("Create hashtable", "(resize)", 0);
    return NULL;
  }

  /* Loop for each bucket in the old table */
  for(i = 0; i < old_ht->size; i++){
    if(old_ht->bucket[i].is_taken){
      if(old_ht->bucket[i].type == BUCKET_MACRO){
        /* Handle macro buckets */
        char *old_name = old_ht->bucket[i].macro_name;
        node *current = old_ht->bucket[i].code_nodes;

        /* Try to insert a copied entry into a bucket in the new table */
        if((new_bucket = insert_entry(new_ht, old_name)) != NULL){
          /* Transfer and copy the old linked list that the bucket points to */
          while(current != NULL){
            /* For each node - copy it and append to the new list */
            add_node(&(new_bucket->code_nodes), current->data);
            current = current->next;
          }
        }else{
          /* Encountered problem in inserting entry */
          print_error("Insert hash", "(resize)", 0); /* Error inserting entry */
          return NULL;
        }
      } else if(old_ht->bucket[i].type == BUCKET_PENDING_LABEL){
        /* Handle pending label buckets */
        char *label_name = old_ht->bucket[i].label_name;
        int cmd_idx = old_ht->bucket[i].command_index;
        int word_num = old_ht->bucket[i].word_number;
        int addr = old_ht->bucket[i].addr;

        if(insert_pending_label(new_ht, label_name, cmd_idx, word_num, addr) == NULL){
          /* Encountered problem in inserting pending label */
          print_error("Insert pending label", "(resize)", 0);
          return NULL;
        }
      }
    }
  }

  return new_ht;
}

hashBucket *get_entry(hashTable *ht, char *name){
  int index;
  int original_index;
  if(ht == NULL || name == NULL){
    /* Empty params provided */
    return NULL;
  }

  index = hash_function(name) % ht->size;
  original_index = index;

  /* Linear probing to find correct bucket */
  while(ht->bucket[index].is_taken){
    if(ht->bucket[index].type == BUCKET_MACRO &&
       strcmp(ht->bucket[index].macro_name, name) == 0){ /* Macro exists */
      return &(ht->bucket[index]); /* Macro found return it*/
    }
    index = (index+1)%ht->size;
    if(index == original_index){
      fprintf(stderr, "ERROR DIDNT FIND ENTRY");
      return NULL;
    }
  }
  return NULL; /* Didn't find the entry */
}

void free_hash_table(hashTable *ht){
  int i;
  if(ht == NULL){
    /* Empty hash table provided */
    return;
  }

  for(i = 0; i < ht->size; i++){
    if(ht->bucket[i].is_taken){
      if(ht->bucket[i].type == BUCKET_MACRO){
        /* Free macro bucket */
        free(ht->bucket[i].macro_name);
        free_list(ht->bucket[i].code_nodes);
      } else if(ht->bucket[i].type == BUCKET_PENDING_LABEL){
        /* Free pending label bucket */
        free(ht->bucket[i].label_name);
      }
    }
  }

  free(ht->bucket);
  free(ht);
}

/* New function for inserting pending labels */
hashBucket *insert_pending_label(hashTable *ht, char *label_name, int command_index, int word_number, int addr){
  double load_factor;
  int index;
  int original_index;

  if(ht == NULL || label_name == NULL){
    print_error("Missing argument", "(insert pending label)", 0);
    return NULL;
  }

  load_factor = (double)(ht->count + 1) / ht->size;
  if(load_factor > LOAD_FACTOR_THERSHOLD){
    hashTable *new_ht = resize_table(ht);

    if(new_ht == NULL){
      return NULL;
    }

    /* Update hash table reference */
    ht->bucket = new_ht->bucket;
    ht->size = new_ht->size;
    ht->count = new_ht->count;

    free(new_ht);
  }

  index = hash_function(label_name) % ht->size;
  original_index = index;

  /* Linear probing to find next empty bucket */
  while(ht->bucket[index].is_taken){
    index = (index+1) % ht->size;
    if(index == original_index){
      return NULL; /* Table is full (should not happen due to resize) */
    }
  }

  /* Initialize the bucket as a pending label */
  ht->bucket[index].type = BUCKET_PENDING_LABEL;
  ht->bucket[index].addr = addr;
  ht->bucket[index].is_taken = 1;
  ht->bucket[index].label_name = strdup(label_name);
  if(ht->bucket[index].label_name == NULL){
    return NULL; /* Failed to copy label name */
  }

  ht->bucket[index].command_index = command_index;
  ht->bucket[index].word_number = word_number;

  /* Set macro fields to NULL/0 for safety */
  ht->bucket[index].macro_name = NULL;
  ht->bucket[index].code_nodes = NULL;
  ht->bucket[index].length = 0;

  ht->count++;

  return &(ht->bucket[index]);
}

/* Function to resolve pending labels */
void resolve_pending_labels(hashTable *ht, char *label_name, int resolved_address,
                          void (*update_command)(int cmd_idx, int word_num, int address)){
  int i;

  if(ht == NULL || label_name == NULL || update_command == NULL){
    print_error("Missing argument", "(resolve pending labels)", 0);
    return;
  }

  for(i = 0; i < ht->size; i++){
    if(ht->bucket[i].is_taken &&
       ht->bucket[i].type == BUCKET_PENDING_LABEL &&
       strcmp(ht->bucket[i].label_name, label_name) == 0){

      /* Call the callback to update the command with the resolved address */
      update_command(
        ht->bucket[i].command_index,
        ht->bucket[i].word_number,
        resolved_address
      );

      /* Optionally, mark the pending label as processed */
      /* If you want to free the memory:
      free(ht->bucket[i].label_name);
      ht->bucket[i].is_taken = 0;
      ht->count--;
      */
    }
  }
}

/* Function to count pending labels */
int get_pending_labels_count(hashTable *ht){
  int i;
  int count = 0;

  if(ht == NULL){
    return 0;
  }

  for(i = 0; i < ht->size; i++){
    if(ht->bucket[i].is_taken && ht->bucket[i].type == BUCKET_PENDING_LABEL){
      count++;
    }
  }

  return count;
}

/* Function to print all pending labels in a hash table */
void print_pending_labels(hashTable *ht) {
    int i;
    int count = 0;
    if (ht == NULL) {
        printf("Error: NULL hash table provided to print_pending_labels\n");
        return;
    }
    printf("\n=== PENDING LABELS TABLE ===\n");
    printf("%-20s | %-15s | %-10s | %-10s\n", "LABEL", "COMMAND INDEX", "WORD POS", "ADDRESS");
    printf("-------------------------------------------------------------------\n");
    for (i = 0; i < ht->size; i++) {
        if (ht->bucket[i].is_taken && ht->bucket[i].type == BUCKET_PENDING_LABEL) {
            printf("%-20s | %-15d | %-10d | %-10d\n",
                   ht->bucket[i].label_name,
                   ht->bucket[i].command_index,
                   ht->bucket[i].word_number,
                   ht->bucket[i].addr);
            count++;
        }
    }
    printf("-------------------------------------------------------------------\n");
    printf("Total pending labels: %d\n", count);
    printf("==============================\n\n");
}
