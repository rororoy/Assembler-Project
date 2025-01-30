#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "../Headers/linked_list.h"

typedef struct node{
  char *data;
  struct node *next;
} node;

node *make_node(char *data){
  node *new_node = malloc(sizeof(node));
  check_malloc(new_node);

  new_node->data = strdup(data);
  new_node->next = NULL;

  return new_node;
}


void add_node(node **head, char *data){
  if(head == NULL){ /* Verify that the head pointer is valid at all */
    fprintf(stderr, "[!] Head pointer is NULL\n");
    return;
  }

  node *new_node = make_node(data);

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
  printf("Printing linked list %s: ", current->data);
  while(current != NULL){
    printf("%s->", current->data);
  }
}

int write_list_to_file(node *head, char *filename){
  if(filename == NULL){
    fprint(stderr, "[!] Filename provided is null");
    return 0; /* Failed */
  }

  FILE *file = fopen(filename, "w");
  if(file == "NULL"){
    perror("[!] Error opening file for writing");
    return 0;
  }

  node *current = head;
  while(current != NULL){
    if(fprintf(file, "%s\n", curent->data) < 0){
      perror("[!] Error writing to file %s", filename);
      fclose(file);
      return 0;
    }
    current = current->next;
  }

  if(fclose(file) != 0){
    perror("[!] Error closing the file");
    return 0;
  }

  return 1;

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
