#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_LINE_LENGTH 80

#define NUM_ALLOWED_COMMANDS 20

#define NUM_RESERVED_WORDS 26

#define REGISTERS_AMOUNT 6

/*
  Define addressing mode bit flags:
    We represent the allowed addressing modes as a 4 bit binary number
    each bit representing a different mode and turning on if alloweed
*/
#define ADDR_IMMEDIATE  0x01  /* 0001 in binary */
#define ADDR_DIRECT     0x02  /* 0010 in binary */
#define ADDR_RELATIVE   0x04  /* 0100 in binary */
#define ADDR_REGISTER   0x08  /* 1000 in binary */
#define ADDR_ALL        0x0F  /* All modes allowed */
#define ADDR_NONE       0x00  /* No addressing modes allowed */

extern int LINE_NUMBER;

extern int ERROR_ENCOUNTERED;

extern char *RESERVED_WORDS[];

/* Define an enum for the supported assembly commands */
typedef enum {
    CMD_MOV,  /* mov */
    CMD_CMP,  /* cmp */
    CMD_ADD,  /* add */
    CMD_SUB,  /* sub */
    CMD_LEA,  /* lea */
    CMD_CLR,  /* clr */
    CMD_NOT,  /* not */
    CMD_INC,  /* inc */
    CMD_DEC,  /* dec */
    CMD_JMP,  /* jmp */
    CMD_BNE,  /* bne */
    CMD_JSR,  /* jsr */
    CMD_RED,  /* red */
    CMD_RTS,  /* rts */
    CMD_PRN,  /* prn */
    CMD_STOP, /* stop */
    CMD_EXTERN, /* extern */
    CMD_ENTRY, /* entry */
    CMD_DATA, /* data */
    CMD_STRING /* string */
} commands;

/* Command semantics */
typedef struct{
  commands name;
  int funct;
  int op_code;
  int type;
  int allowed_src_add_mode;
  int allowed_dest_add_mode;
} commandSem;

/* Global arrays moved from translate.c */
extern commandSem command_table[];
extern char *allowed_commands[];
extern char *registers[];

#endif
