#ifndef LINKED_LIST_H
#define LINKED_LIST_H

/* This structure represents a node in a linked list that stores string data */
typedef struct node{
  char *data;     /* String data stored in the node */
  struct node *next; /* Pointer to the next node in the list */
} node;

#include "./global.h"
#include "./translate.h"

/* This structure represents a node in a linked list that stores machine word data */
struct wordNode {
  word data;      /* Machine word data stored in the node */
  struct wordNode *next; /*  Pointer to the next node in the list */
};

/* Regular linked list */

/**
 * Creates a new node for a string linked list
 *
 * Allocates memory for a new node and initializes it with the given data.
 *
 * @param data (in) String to store in the node
 * @return Pointer to the newly created node, or NULL if memory allocation failed
 */
node *make_node(char *data);

/**
 * Adds a new node with the specified data to the end of a string linked list
 *
 * Creates a new node with the given data and appends it to the end of the list.
 * If the list is empty, the new node becomes the head of the list.
 *
 * @param head (in/out) Pointer to the head pointer of the list
 * @param data (in) String to store in the new node
 */
void add_node(node **head, char *data);

/**
 * Prints all nodes in a string linked list
 *
 * Debug function that outputs the contents of all nodes in the list.
 *
 * @param head (in) Pointer to the head of the list
 */
void print_list(node *head);

/**
 * Writes the contents of a string linked list to a file
 *
 * @param file (in) Pointer to an open file for writing
 * @param head (in) Pointer to the head of the list
 * @param filename (in) Name of the file (for error reporting)
 * @return 1 if successful, 0 if an error occurred
 */
int write_list_to_file(FILE *file, node *head, char *filename);

/**
 * Searches for a node with specific data in a string linked list
 *
 * @param head (in) Pointer to the head of the list
 * @param data (in) String to search for
 * @return Pointer to the node containing the data, or NULL if not found
 */
node *search_node(node *head, char *data);

/**
 * Frees all memory allocated for a string linked list
 *
 * @param head (in) Pointer to the head of the list
 */
void free_list(node *head);


/* Word type linked list */

/**
 * Creates a new node for a word linked list
 *
 * Allocates memory for a new node and initializes it with the given machine word data.
 *
 * @param data (in) Machine word to store in the node
 * @return Pointer to the newly created node, or NULL if memory allocation failed
 */
wordNode *make_word_node(word data);

/**
 * Adds a new node with the specified word data to the end of a word linked list
 *
 * Creates a new node with the given machine word and appends it to the end of the list.
 * If the list is empty, the new node becomes the head of the list.
 *
 * @param head (in/out) Pointer to the head pointer of the list
 * @param data (in) Machine word to store in the new node
 */
void add_word_node(wordNode **head, word data);

/**
 * Prints all nodes in a word linked list showing binary representation
 *
 * Debug function that outputs the contents of all machine words in the list.
 *
 * @param head (in) Pointer to the head of the list
 */
void print_word_list(wordNode *head);

/**
 * Gets the node at a specific index in a word linked list
 *
 * @param head (in) Pointer to the head of the list
 * @param index (in) Zero-based index of the node to retrieve
 * @return Pointer to the node at the specified index, or NULL if index is out of bounds
 */
wordNode *get_word_at_index(wordNode *head, int index);

/**
 * Frees all memory allocated for a word linked list
 *
 * @param head (in) Pointer to the head of the list
 */
void free_word_list(wordNode *head);

#endif
