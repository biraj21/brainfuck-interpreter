CC = cc
FLAGS = -pedantic -Wall -Werror
SRC = ./src
OBJ = ./obj
BIN = ./bf
# HEADER_FILES = $(wildcard $(SRC)/*.h)
SRC_FILES = $(wildcard $(SRC)/*)
OBJ_FILES = $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c))

$(OBJ)/main.o: $(SRC_FILES)
	$(CC) $(FLAGS) -c -o $@ $(SRC)/main.c

$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(FLAGS) -c -o $@ $<

$(BIN): $(OBJ_FILES)
	$(CC) $(FLAGS) -o $@ $(OBJ_FILES)

clean:
	rm -f $(BIN) $(OBJ)/*.o sample
