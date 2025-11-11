CC = gcc
CFLAGS = -Iinclude -Wall

SRC = src/main.c src/shell.c src/execute.c src/history.c
OBJ = obj/main.o obj/shell.o obj/execute.o obj/history.o
TARGET = bin/myshell

all: $(TARGET)

# Build executable
$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET)

# Compile each source file into object file
obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/* $(TARGET)
