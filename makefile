# Compiler and flags
CC = gcc
CFLAGS = -ansi -Wall -pedantic

# Name of the final executable
TARGET = assembler

# Default rule
all: $(TARGET)

# Link object files to create the executable
$(TARGET): assembler.o pre_assembler.o error.o hash_table.o linked_list.o utils.o validate.o first_stage.o
	$(CC) $(CFLAGS) -o $(TARGET) assembler.o pre_assembler.o error.o hash_table.o linked_list.o utils.o validate.o first_stage.o

assembler.o: ./Source/assembler.c ./Headers/assembler.h ./Headers/pre_assembler.h ./Headers/utils.h ./Headers/error.h ./Headers/first_stage.h
	$(CC) $(CFLAGS) -c ./Source/assembler.c

# Compile mycomp.o
pre_assembler.o: ./Source/pre_assembler.c ./Headers/pre_assembler.h ./Headers/error.h ./Headers/hash_table.h ./Headers/linked_list.h ./Headers/utils.h ./Headers/global.h ./Headers/validate.h
	$(CC) $(CFLAGS) -c ./Source/pre_assembler.c

# Compile utils.o
error.o: ./Source/error.c ./Headers/error.h
	$(CC) $(CFLAGS) -c ./Source/error.c

validate.o: ./Source/validate.c ./Headers/validate.h ./Headers/global.h
	$(CC) $(CFLAGS) -c ./Source/validate.c

# Compile utils.o
hash_table.o: ./Source/hash_table.c ./Headers/hash_table.h ./Headers/linked_list.h ./Headers/utils.h ./Headers/error.h
	$(CC) $(CFLAGS) -c ./Source/hash_table.c

# Compile utils.o
linked_list.o: ./Source/linked_list.c ./Headers/linked_list.h ./Headers/utils.h ./Headers/error.h
	$(CC) $(CFLAGS) -c ./Source/linked_list.c

utils.o: ./Source/utils.c ./Headers/utils.h ./Headers/error.h ./Headers/global.h ./Headers/validate.h
	$(CC) $(CFLAGS) -c ./Source/utils.c

first_stage.o: ./Source/first_stage.c ./Headers/first_stage.h ./Headers/error.h ./Headers/utils.h ./Headers/global.h ./Headers/validate.h
	$(CC) $(CFLAGS) -c ./Source/first_stage.c

# Clean up generated files
clean:
	rm -f *.o $(TARGET)
