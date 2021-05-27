CC = gcc
SRC = ./src
FLAGS = -Wall -Werror
BIN = bf

$(BIN): $(SRC)/main.c $(SRC)/stack.c
	$(CC) $(FLAGS) -o $@ $^

clean:
	rm -f $(BIN)