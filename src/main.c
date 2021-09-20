#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

#define MAXMEM 30000 // number of memory cells

void free_memory(void);

unsigned char memory[MAXMEM]; // memory cells (1 byte each)
unsigned short ptr = 0;       // currently pointed memory cell
char *code = NULL;            // for storing brainfuck code
Stack jumps = STACK_INIT;     // stores indices of '[' (opened loops)

int main(int argc, const char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s FILE\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r");
    if (fp == NULL) {
        perror("bf: Fatal Error");
        return 1;
    }

    size_t alloced = 128;
    code = malloc(alloced);
    if (code == NULL) {
        fclose(fp);
        perror("bf: Error");
        return 1;
    }

    atexit(free_memory);

    int len = 0;
    for (int c; (c = fgetc(fp)) != EOF;) {
        if (c == '>' || c == '<' || c == '+' || c == '-' || c == '[' || c == ']' || c == ',' || c == '.') {
             code[len] = c;

            if (++len == alloced - 1) {
                alloced *= 1.5;
                char *new_buf = realloc(code, alloced);
                if (new_buf == NULL) {
                    fclose(fp);
                    perror("bf: Error");
                    return 1;
                }

                code = new_buf;
            }
        }
    }

    code[len] = '\0';
    fclose(fp);

    // initializing each memory cell with 0
    for (int i = 0; i < MAXMEM; ++i)
        memory[i] = 0;

    /*** Interpreter ***/
    for (int i = 0; code[i]!= '\0';) {
        char c = code[i];
        if (c == '>') {
            if (ptr == MAXMEM - 1) {
                fprintf(stderr, "bf: Runtime Error: Cell index out of range\n"
                                "Pointer is already at the last memory cell (index %d).\n", MAXMEM - 1);
                return 1;
            }

            ++ptr;
        } else if (c == '<') {
            if (ptr == 0) {
                fputs("bf: Runtime Error: Cell index out of range\n"
                     "Pointer is already at the first memory cell (index 0).\n", stderr);
                return 1;
            }

            --ptr;
        } else if (c == '+')
            ++memory[ptr];
        else if (c == '-')
            --memory[ptr];
        else if (c == '[') {
            if (memory[ptr] == 0) {
                ++i;
                for (int loop_depth = 1; loop_depth > 0 && code[i] != '\0'; ++i) {
                    if (code[i] == '[')
                        ++loop_depth;
                    else if (code[i] == ']')
                        --loop_depth;
                }

                continue;
            } else
                push(&jumps, i + 1);
        } else if (c == ']' ) {
            if (jumps.top == NULL) {
                fputs("bf: Runtime Error: Unnecessary ']' for unopened loop\n"
                      "No matching '[' found for current ']'\n", stderr);
                return 1;
            }

            if (memory[ptr] == 0)
                pop(&jumps);
            else {
                i = jumps.top->value;
                continue;
            }
        } else if (c == ',')
            memory[ptr] = getchar();
        else if (c == '.')
            putchar(memory[ptr]);

        ++i;
    }

    if (jumps.top != NULL)
        puts("bf: Warning: Unclosed loop\nMissing a ']'.");

    return 0;
}

void free_memory(void) {
    free(code);
    while (pop(&jumps) != STACK_EMPTY)
        ;
}
