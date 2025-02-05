typedef enum {
    INTERNAL,
    EXTERNAL
} error_scope;

typedef struct{
  error_scope context; /* The context of the error - if it was iternal (fault in the program) or external (user) */
  char *name;
  char *description;
} error;

extern error errors[];

void print_error(char *name, char *additional_arg, int line_number);
