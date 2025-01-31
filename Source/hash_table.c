#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../Headers/hash_table.h"
#include "../Headers/linked_list.h"
#include "../Headers/utils.h"

hashTable *make_hash_table(int size){
  int i;

  hashTable *ht = malloc(sizeof(hashTable));
  check_malloc(ht);

  ht->size = size;
  ht->count = 0;
  ht->bucket = calloc(ht->size, sizeof(hashBucket));

  /* Initialize all buckets to NULL */
  for(i = 0; i < ht->size; i++){
    ht->bucket[i].macro_name = NULL;
    ht->bucket[i].code_nodes = NULL;
    ht->bucket[i].length = 0;
    ht->bucket[i].is_taken = 0;
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
    fprintf(stderr, "Error invalid arguments for macro:%s\n", name);
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
  index = hash_function(name);
  original_index = index;

  /* Linear probing to find next empty bucket */
  while(ht->bucket[index].is_taken){
    if(strcmp(ht->bucket[index].macro_name, name) == 0){ /* Macro exists */
      return &(ht->bucket[index]); /* Macro already exists return it*/
    }
    index = (index+1)%ht->size;
    if(index == original_index){
      fprintf(stderr, "ERROR HASH TABLE IS FULL");
      return NULL;
    }
  }

  /* Macro isn't logged and found empty bucket */
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
    fprintf(stderr, "ERROR WHEN CREATING NEW RESIZED HASH TABLE\n");
    return NULL;
  }

  /* Loop for each bucket in the old table */
  for(i = 0; i < old_ht->size; i++){
    if(old_ht->bucket[i].is_taken){
      char *old_name = old_ht->bucket[i].macro_name;
      node *current = old_ht->bucket[i].code_nodes;

      /* Try to insert a copied entry into a bucket in the new table */
      if((new_bucket = insert_entry(new_ht, old_name)) != NULL){
        /* Transfer and copy the old linked list that the bucket points to */
        while(current != NULL){
          /* For each node - copy it and append to the new list */
          add_node(&(new_ht->bucket[i].code_nodes), current->data);
          current = current->next;
        }
      }else{
        /* Encountered problem in inserting entry */
        return NULL;
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

  index  = hash_function(name);
  original_index = index;

  /* Linear probing to find correct bucket */
  while(ht->bucket[index].is_taken){
    if(strcmp(ht->bucket[index].macro_name, name) == 0){ /* Macro exists */
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
      free(ht->bucket[i].macro_name);
      free_list(ht->bucket[i].code_nodes);
    }
  }

  free(ht->bucket);
  free(ht);
}
