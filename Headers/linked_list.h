
typedef struct node{
  char *name;
  char *data;
  int line;
  struct node *next;
} node;

node *make_node(char *name, char *data, int line){
  node *new_node = malloc(sizeof(node));
  check_malloc(new_node);

  new_node->name = name;
  new_node->data = data;
  new_node->line = line;
  new_node->next = NULL;

  return node;
}
