#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_LINE_LENGTH 80

#define NUM_ALLOWED_COMMANDS 20

#define NUM_RESERVED_WORDS 26

#define REGISTERS_AMOUNT 6

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

#endif
