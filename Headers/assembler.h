#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <stdio.h>
#include <stdlib.h>
#include "./pre_assembler.h"
#include "./error.h"
#include "./utils.h"
#include "./first_stage.h"
#include "./linked_list.h"
#include "./hash_table.h"
#include "./global.h"
#include "./files.h"

/**
 * Main entry point for the assembler program
 *
 * Processes each assembly source file provided on the command line through:
 * 1. Pre-assembler stage: Expands macros and processes directives (.am file)
 * 2. First pass: Converts assembly code to machine code and builds symbol table
 * 3. Second pass: Resolves all undefined labels and generates output files
 *
 * @param argc (in) Number of command-line arguments
 * @param argv (in) Array of command-line arguments, where argv[1] through argv[argc-1]
 *                  are the names of assembly source files without extension
 * @return 0 if the program completes successfully, non-zero otherwise
 */
int main(int argc, char *argv[]);

#endif
