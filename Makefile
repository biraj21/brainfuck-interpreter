CC := cc
FLAGS := -Wall -Werror -Wextra -Wpedantic
SRC := ./src
OBJ := ./obj
BIN := ./bf
HEADER_FILES := $(wildcard $(SRC)/*.h)
SRC_FILES := $(wildcard $(SRC)/*.[ch])
OBJ_FILES := $(patsubst $(SRC)/%.c, $(OBJ)/%.o, $(wildcard $(SRC)/*.c))

all: setup clean $(BIN)

# link all object files and create the binary
$(BIN): $(OBJ_FILES)
	$(CC) $(FLAGS) -o $@ $(OBJ_FILES)

# compile main.c if it, or any header file has changed
$(OBJ)/main.o: $(SRC)/main.c $(HEADER_FILES)
	$(CC) $(FLAGS) -c -o $@ $(SRC)/main.c

# compile any source file <file>.c if it, or <file>.h has changed
$(OBJ)/%.o: $(SRC)/%.c $(SRC)/%.h
	$(CC) $(FLAGS) -c -o $@ $<

setup:
	mkdir -p src obj

clean:
	rm -f $(BIN) $(OBJ)/*.o

debug: setup $(BIN)
	valgrind --log-file=valgrind.log $(BIN) $(FILE)
