CC = gcc
CFLAGS = -Iinclude -Wall
LDFLAGS = -lreadline

SRC = src/main.c src/shell.c src/io_redirection.c src/pipes.c
OBJ = obj/main.o obj/shell.o obj/io_redirection.o obj/pipes.o
TARGET = bin/myshell

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

obj/%.o: src/%.c
	mkdir -p obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj/*.o $(TARGET)
