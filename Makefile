path = ./src
flags = -Wall -Werror
build:
	gcc $(flags) $(path)/main.c $(path)/stack.c -o bf

clean:
	rm -fr bf