#include "../Headers/global.h"

int LINE_NUMBER = 0;  /* Definition and initialization of the global variable */

int ERROR_ENCOUNTERED = 0;

char *RESERVED_WORDS[] = {
  "mov",
  "add",
  "cmp",
  "sub",
  "lea",
  "clr",
  "not",
  "inc",
  "dec",
  "jmp",
  "bne",
  "jsr",
  "red",
  "prn",
  "rts",
  "stop",
  ".string",
  "string",
  ".extern",
  "extern",
  ".data",
  "data",
  ".entry",
  "entry",
  "mcro",
  "mcroend",
  "r0",
  "r1",
  "r2",
  "r3",
  "r4",
  "r5",
  "r6",
  "r7"
};

/* Command table with addressing modes as bit flags */
commandSem command_table[] = {
    /* CMD     |funct|op|type|                    src_modes                   |                     dest_modes                */
    {CMD_MOV,  -1,   0, 1,     ADDR_DIRECT | ADDR_REGISTER,                      ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_CMP,  -1,   1, 1,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER,     ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_ADD,   1,   2, 1,     ADDR_DIRECT | ADDR_REGISTER,                      ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_SUB,   2,   2, 1,     ADDR_DIRECT | ADDR_REGISTER,                      ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_LEA,  -1,   4, 1,     ADDR_DIRECT,                                      ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_CLR,   1,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_NOT,   2,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_INC,   3,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_DEC,   4,   5, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_JMP,   1,   9, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_RELATIVE                  },
    {CMD_BNE,   2,   9, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_RELATIVE                  },
    {CMD_JSR,   3,   9, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_RELATIVE                  },
    {CMD_RED,  -1,  12, 2,     ADDR_NONE,                                        ADDR_DIRECT | ADDR_REGISTER                  },
    {CMD_PRN,  -1,  13, 2,     ADDR_NONE,                                        ADDR_IMMEDIATE | ADDR_DIRECT | ADDR_REGISTER },
    {CMD_RTS,  -1,  14, 3,     ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_STOP, -1,  15, 3,     ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_EXTERN, -1, -1, 4,    ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_ENTRY, -1, -1, 4,     ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_DATA, -1, -1, 4,      ADDR_NONE,                                        ADDR_NONE                                    },
    {CMD_STRING, -1, -1, 4,    ADDR_NONE,                                        ADDR_NONE                                    }
};

/* This array must match the order of the enum exactly. */
char *allowed_commands[] = {
    "mov",
    "cmp",
    "add",
    "sub",
    "lea",
    "clr",
    "not",
    "inc",
    "dec",
    "jmp",
    "bne",
    "jsr",
    "red",
    "prn",
    "rts",
    "stop",
    ".extern",
    ".entry",
    ".data",
    ".string"
};

char *registers[] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};
