# ===============================
# Makefile for MyShell Project
# ===============================

# Compiler and Flags
CC = gcc
CFLAGS = -Iinclude -Wall

# Directories
SRC_DIR = src
BIN_DIR = bin
OBJ_DIR = obj

# Files
SRC_FILES = $(SRC_DIR)/main.c $(SRC_DIR)/shell.c $(SRC_DIR)/execute.c
OBJ_FILES = $(OBJ_DIR)/main.o $(OBJ_DIR)/shell.o $(OBJ_DIR)/execute.o
TARGET = $(BIN_DIR)/myshell

# Default target
all: $(TARGET)

# Link object files to create the final binary
$(TARGET): $(OBJ_FILES)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJ_FILES) -o $(TARGET)
	@echo "✅ Build complete. Run ./bin/myshell"

# Compile each .c file into .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean build files
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)
	@echo "🧹 Cleaned build files."

# Run the shell
run: all
	./bin/myshell

# Phony targets (not real files)
.PHONY: all clean run
