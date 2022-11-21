#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "transpiler.h"

#define MAXMEM 30000 // number of memory cells

typedef enum {
    INTERPRET,
    TRANSPILE
} Action;

void free_memory(void);
void interpret(void);
void read_from_file(FILE *fp, char **str, size_t *capacity_ptr);

unsigned char memory[MAXMEM] = {0}; // memory cells (1 byte each)
unsigned short ptr = 0;             // currently pointed memory cell
char *code = NULL;                  // for storing brainfuck code
Stack jumps = STACK_INIT;           // stores indices of '[' (opened loops)

int main(int argc, const char *argv[]) {
    Action action = INTERPRET;
    FILE *fp =  NULL;
    if (argc > 1) {
        if (strcmp(argv[1], "--transpile") == 0) {
            action = TRANSPILE;
        } else if (strncmp(argv[1], "--", 2) == 0) {
            fprintf(stderr, "bf: Error: Invalid option %s\n", argv[1]);
            return EXIT_FAILURE;
        }

        bool file_given = false;
        if (action == TRANSPILE && argc > 2) {
            fp = fopen(argv[2], "r");
            file_given = true;
        } else if (action == INTERPRET && argc > 1) {
            fp = fopen(argv[1], "r");
            file_given = true;
        }

        if (file_given && fp == NULL) {
            perror("bf: Fatal Error");
            return EXIT_FAILURE;
        }
    }

    size_t capacity = 128;
    code = malloc(capacity);
    if (code == NULL) {
        fclose(fp);
        perror("bf: Error");
        return EXIT_FAILURE;
    }

    atexit(free_memory);

    if (fp != NULL) {
        read_from_file(fp, &code, &capacity);
        fclose(fp);
    } else {
        read_from_file(stdin, &code, &capacity);
        puts("\n----- read -----");
        freopen("/dev/tty", "r", stdin); // reopening stdin in case the Brainfuck program needs user input
    }

    if (action == TRANSPILE) {
        transpile(code, stdout);
    } else {
        interpret();
    }

    return EXIT_SUCCESS;
}

void free_memory(void) {
    free(code);
    while (!stack_is_empty(&jumps)) {
        stack_pop(&jumps);
    }
}

void interpret(void) {
    for (size_t i = 0; code[i]!= '\0';) {
        char c = code[i];
        if (c == '>') {
            if (ptr == MAXMEM - 1) {
                fprintf(stderr, "bf: Runtime Error: Cell index out of range. Pointer is already at the last memory cell (index %d).\n", MAXMEM - 1);
                exit(EXIT_FAILURE);
            }

            ++ptr;
        } else if (c == '<') {
            if (ptr == 0) {
                fputs("bf: Runtime Error: Cell index out of range. Pointer is already at the first memory cell (index 0).\n", stderr);
                exit(EXIT_FAILURE);
            }

            --ptr;
        } else if (c == '+') {
            ++memory[ptr];
        } else if (c == '-') {
            --memory[ptr];
        } else if (c == '[') {
            if (memory[ptr] == 0) {
                ++i;
                int loop_depth = 1; 
                for (; loop_depth > 0 && code[i] != '\0'; ++i) {
                    if (code[i] == '[') {
                        ++loop_depth;
                    } else if (code[i] == ']') {
                        --loop_depth;
                    }
                }

                if (loop_depth > 0) {
                    puts("bf: Warning: Unclosed loop. Missing a ']'.");
                }

                continue;
            } else {
                stack_push(&jumps, i + 1);
            }
        } else if (c == ']' ) {
            if (jumps.top == NULL) {
                fputs("bf: Runtime Error: Unnecessary ']'. No matching '[' found for current ']'\n", stderr);
                exit(EXIT_FAILURE);
            }

            if (memory[ptr] == 0) {
                stack_pop(&jumps);
            } else {
                i = jumps.top->value;
                continue;
            }
        } else if (c == ',') {
            memory[ptr] = getchar();
        } else if (c == '.') {
            putchar(memory[ptr]);
        }

        ++i;
    }

    if (!stack_is_empty(&jumps)) {
        puts("bf: Warning: Unclosed loop. Missing a ']'.");
    }
}

void read_from_file(FILE *fp, char **str_ptr, size_t *capacity_ptr) {
    char *str = *str_ptr;
    size_t capacity = *capacity_ptr;

    size_t len = 0;
    int c;
    while ((c = fgetc(fp)) != EOF) {
        if (c == '>' || c == '<' || c == '+' || c == '-' || c == '[' || c == ']' || c == ',' || c == '.') {
            str[len] = c;

            if (++len == capacity - 1) {
                capacity *= 1.5;
                char *new_buf = realloc(str, capacity);
                if (new_buf == NULL) {
                    if (fp != stdin) {
                        fclose(fp);
                    }

                    perror("bf: Error");
                    exit(EXIT_FAILURE);
                }

                str = new_buf;
            }
        }
    }

    str[len] = '\0';
    *str_ptr = str;
    *capacity_ptr = capacity;
}