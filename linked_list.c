#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../Headers/linked_list.h"
#include "../Headers/utils.h"
#include "../Headers/error.h"

node *make_node(char *data){
  node *new_node = malloc(sizeof(node));
  check_malloc(new_node);

  new_node->data = strdup(data);
  new_node->next = NULL;

  return new_node;
}

void add_node(node **head, char *data){
  node *new_node;
  if(head == NULL){ /* Verify that the head pointer is valid at all */
    fprintf(stderr, "[!] Head pointer is NULL\n");
    return;
  }

  new_node = make_node(data);

  if(*head == NULL){ /* Check if head pointer is empty */
    *head = new_node;
  }else{
    /* Loop untill we reach the end - and append at the end */
    node *current = *head;
    while(current->next != NULL){
      current = current->next;
    }
    current->next = new_node;
  }
}

void print_list(node *head){
  node *current = head;

  printf("Printing linked list %s ", current->data);
  while(current != NULL){
    printf("%s->", current->data);
    current = current->next;
  }
  printf("\n");
}

int write_list_to_file(FILE *file, node *head, char *filename){
  node *current;
  if(filename == NULL){
    print_error("Missing argument", filename, 0);
    return 0; /* Failed */
  }

  current = head;
  while(current != NULL){
    if(fprintf(file, "%s", current->data) < 0){
      print_error("Failed writing", filename, 0);
      return 0;
    }
    current = current->next;
  }

  return 1;
}

node *search_node(node *head, char *data){
  node *current = head;
  while(current != NULL){
    if(strcmp(current->data, data) == 0){
      return current;
    }
    current = current->next;
  }
  return NULL;
}

void free_list(node *head){
  node *current = head;
  while(current != NULL){
    node *temp = current;
    current = current->next;
    free(temp->data);
    free(temp);
  }
}

/* Create a new word node */
wordNode *make_word_node(word data) {
  wordNode *new_node = malloc(sizeof(wordNode));
  check_malloc(new_node);
  new_node->data = data; /* Direct assignment of word struct */
  new_node->next = NULL;
  return new_node;
}

/* Add a word node to the end of the list */
void add_word_node(wordNode **head, word data) {
  wordNode *new_node;
  if(head == NULL) { /* Verify that the head pointer is valid at all */
    fprintf(stderr, "[!] Head pointer is NULL\n");
    return;
  }
  new_node = make_word_node(data);
  if(*head == NULL) { /* Check if head pointer is empty */
    *head = new_node;
  } else {
    /* Loop until we reach the end - and append at the end */
    wordNode *current = *head;
    while(current->next != NULL) {
      current = current->next;
    }
    current->next = new_node;
  }
}

/* Print the word list (showing binary representation) */
void print_word_list(wordNode *head) {
  wordNode *current = head;
  int count = 0;
  printf("Printing word linked list:\n");

  while(current != NULL) {
    /* Print binary representation of the word */
    printf("Word %d: ", count++);

    /* Check if this is an instruction word or extra word based on usage pattern */
    /* For simplicity, just print some key fields */
    printf("opcode=%u, src_mode=%u, dst_mode=%u | OR | value=%u, are=%u%u%u\n",
           current->data.instruction.opcode,
           current->data.instruction.src_mode,
           current->data.instruction.dst_mode,
           current->data.extra_word.value,
           current->data.extra_word.a,
           current->data.extra_word.r,
           current->data.extra_word.e);

    current = current->next;
  }
  printf("\n");
}

/* Free the entire word list */
void free_word_list(wordNode *head) {
  wordNode *current = head;
  while(current != NULL) {
    wordNode *temp = current;
    current = current->next;
    /* No need to free the word data as it's not a pointer */
    free(temp);
  }
}

/* Get the length of the word list */
int word_list_length(wordNode *head) {
  wordNode *current = head;
  int count = 0;

  while(current != NULL) {
    count++;
    current = current->next;
  }

  return count;
}

/* Get word at specific index */
wordNode *get_word_at_index(wordNode *head, int index) {
  wordNode *current = head;
  int count = 0;

  while(current != NULL && count < index) {
    current = current->next;
    count++;
  }

  return current; /* Returns NULL if index is out of bounds */
}
