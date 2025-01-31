#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node{
  char *data;
  struct node *next;
} node;

node *make_node(char *data);

void add_node(node **head, char *data);

void print_list(node *head);

int write_list_to_file(node *head, char *filename);

void free_list(node *head);

#endif
