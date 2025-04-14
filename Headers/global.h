#ifndef GLOBAL_H
#define GLOBAL_H

/* Maximum allowed length of a line in the assembly source code */
#define MAX_LINE_LENGTH 80

/* Number of allowed commands in the assembly language */
#define NUM_ALLOWED_COMMANDS 20

/* Number of reserved words in the assembly language */
#define NUM_RESERVED_WORDS 34

/* Number of available registers */
#define REGISTERS_AMOUNT 8

/**
 * These bit flags represent different addressing modes:
 * - Each bit represents a different addressing mode
 * - Bits can be combined with bitwise OR to allow multiple modes
 */
#define ADDR_IMMEDIATE  0x01  /**< 0001 in binary - Immediate addressing mode (#) */
#define ADDR_DIRECT     0x02  /**< 0010 in binary - Direct addressing mode (label) */
#define ADDR_RELATIVE   0x04  /**< 0100 in binary - Relative addressing mode (&label) */
#define ADDR_REGISTER   0x08  /**< 1000 in binary - Register addressing mode (r0-r7) */
#define ADDR_ALL        0x0F  /**< All modes allowed (0001 | 0010 | 0100 | 1000) */
#define ADDR_NONE       0x00  /**< No addressing modes allowed */

/**
 * This global variable keeps track of the current line number during assembly,
 * which is useful for error reporting
 */
extern int LINE_NUMBER;

/* Set to non-zero value if any error has been encountered during the assembly process */
extern int ERROR_ENCOUNTERED;

/* Contains all keywords and reserved words that cannot be used as labels or identifiers */
extern char *RESERVED_WORDS[];

/* This enum defines symbolic names for all supported commands in the assembly language */
typedef enum {
    CMD_MOV,     /**< mov - Move operand to destination */
    CMD_CMP,     /**< cmp - Compare operands */
    CMD_ADD,     /**< add - Add source to destination */
    CMD_SUB,     /**< sub - Subtract source from destination */
    CMD_LEA,     /**< lea - Load effective address */
    CMD_CLR,     /**< clr - Clear destination */
    CMD_NOT,     /**< not - Logical NOT on destination */
    CMD_INC,     /**< inc - Increment destination */
    CMD_DEC,     /**< dec - Decrement destination */
    CMD_JMP,     /**< jmp - Jump to address */
    CMD_BNE,     /**< bne - Branch if not equal */
    CMD_JSR,     /**< jsr - Jump to subroutine */
    CMD_RED,     /**< red - Read value to destination */
    CMD_RTS,     /**< rts - Return from subroutine */
    CMD_PRN,     /**< prn - Print value */
    CMD_STOP,    /**< stop - Stop program execution */
    CMD_EXTERN,  /**< extern - Declare external label */
    CMD_ENTRY,   /**< entry - Declare entry point */
    CMD_DATA,    /**< data - Define data values */
    CMD_STRING   /**< string - Define string */
} commands;

/*
 * This structure contains all the necessary information about a command,
 * including its opcode, function code, type, and allowed addressing modes.
 */
typedef struct{
    commands name;               /**< Command name from the commands enum */
    int funct;                   /**< Function code (part of the binary encoding) */
    int op_code;                 /**< Operation code (part of the binary encoding) */
    int type;                    /**< Command type (1=two operands, 2=one operand, 3=no operands, 4=directive) */
    int allowed_src_add_mode;    /**< Bit mask of allowed source operand addressing modes */
    int allowed_dest_add_mode;   /**< Bit mask of allowed destination operand addressing modes */
} commandSem;

/* Table of all supported assembly commands and their semantics */
extern commandSem command_table[];

/* Array of command names as strings */
extern char *allowed_commands[];

/* Array of register names (r0-r7) */
extern char *registers[];

#endif
