#include <stdio.h>
#include <string.h>
#include "../Headers/translate.h"
#include "../Headers/global.h"

commandSem command_table[] = {
    /*  CMD   funct op */
    {CMD_MOV, -1,  0},
    {CMD_CMP, -1,  1},
    {CMD_ADD,  1,  2},
    {CMD_SUB,  2,  2},
    {CMD_LEA, -1,  4},
    {CMD_CLR,  1,  5},
    {CMD_NOT,  2,  5},
    {CMD_INC,  3,  5},
    {CMD_DEC,  4,  5},
    {CMD_JMP,  1,  9},
    {CMD_BNE,  2,  9},
    {CMD_JSR,  3,  9},
    {CMD_RED, -1, 12},
    {CMD_RTS, -1, 13},
    {CMD_PRN, -1, 14},
    {CMD_STOP,-1, 15}
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
    "rts",
    "prn",
    "stop"
};

char *registers[] = {"r1", "r2", "r3", "r4", "r5", "r6"};


commandSem *command_lookup(char *cmd_name) {
  int i;
  for (i = 0; i < NUM_ALLOWED_COMMANDS; i++) {
    if (strcmp(cmd_name, allowed_commands[i]) == 0) {
      return &command_table[i]; /* Found match at index i */
    }
  }
  return NULL; /* Not found */
}
