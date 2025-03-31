#ifndef LINKED_LIST_H
#define LINKED_LIST_H

typedef struct node{
  char *data;
  struct node *next;
} node;

struct wordNode {
  word data;
  struct wordNode *next;
};

/* Regular linked list */

node *make_node(char *data);

void add_node(node **head, char *data);

void print_list(node *head);

int write_list_to_file(FILE *file, node *head, char *filename);

node *search_node(node *head, char *data);

void free_list(node *head);


/* Word type linked list */

/* Create a new word node */
wordNode *make_word_node(word data);

/* Add a word node to the end of the list */
void add_word_node(wordNode **head, word data);

/* Print the word list (showing binary representation) */
void print_word_list(wordNode *head);

/* Get word at specific index */
wordNode *get_word_at_index(wordNode *head, int index);

/* Free the entire word list */
void free_word_list(wordNode *head);

#endif
