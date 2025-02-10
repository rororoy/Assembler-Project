#ifndef TRANSLATE_H
#define TRANSLATE_H

#include "./global.h"

typedef struct{
  /* Highest bits first */
  unsigned opcode     : 6; /* bits 18...23 */
  unsigned src_mode   : 2; /* bits 16...17  */
  unsigned src_reg    : 3; /* bits 13...15 */
  unsigned dst_mode   : 2; /* bits 11...12 */
  unsigned dst_reg    : 3; /* bits 8....10 */
  unsigned funct      : 5; /* bits 7...3  */
  signed   a          : 1; /* bits 2 */
  signed   r          : 1; /* bits 1 */
  signed   e          : 1; /* bits 0 */
} instructionWord;

/* Define an enum for the supported assembly commands */
typedef enum {LBL_CODE, LBL_DATA} labelType;

/* Command symanyics */
typedef struct{
  commands name;
  int funct;
  int op_code;
} commandSem;

/* Entry in the symbol table */
typedef struct{
  char *name;
  int address;
  labelType type;
} symbol;

typedef struct{
  int address;
  char *source_code;
  instructionWord binary[3];
} transTable;

extern char *allowed_commands[];

extern commandSem command_table[];

commandSem *command_lookup(char *cmd_name);

#endif
