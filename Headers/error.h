typedef struct{
  int code; /* The error code number */
  char *msg; /* The description of the error */
} error;

void print_error(int error_code);
