/**
 * Defines the scope of an error
 *
 * This enum differentiates between internal errors (faults in the program)
 * and external errors (user input errors).
 */
typedef enum {
    INTERNAL, /* Error is internal to the program (programming fault) */
    EXTERNAL  /* Error is external (user input error) */
} error_scope;

/**
 * Stores information about an error
 *
 * This structure contains all necessary information about an error,
 * including its scope, name, and description.
 */
typedef struct{
  error_scope context; /* The context of the error - if it was internal (fault in the program) or external (user) */
  char *name;         /* Short error name/identifier */
  char *description;  /* Detailed description of the error */
} error;

/**
 * Global array of predefined errors
 *
 * This array contains all predefined errors that can occur in the program.
 */
extern error errors[];

/**
 * Prints an error message to the user
 *
 * This function formats and displays an error message including the error name,
 * any additional information, and the line number where the error occurred.
 *
 * @param name (in) The name of the error
 * @param additional_arg (in) Additional information about the error, can be NULL
 * @param line_number (in) The line number where the error occurred
 */
void print_error(char *name, char *additional_arg, int line_number);
