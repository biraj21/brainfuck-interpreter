#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "../include/stack.h"

#define MAXMEM 30000 // number of memory cells

void free_memory();

unsigned char memory[MAXMEM]; // memory cells (1 byte each)
unsigned short ptr = 0;       // currently pointed memory cell
char *code = NULL;            // brainfuck code
Stack loop_indices = NULL;    // stores the indices of '[' (opened loops)
bool skip = false;            // whether to skip the code or not (loop)
unsigned short opened = 0;    // number of '[' while skipping

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: %s <file>\n", argv[0]);
        return 1;
    }

    // opening brainfuck file
    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL)
    {
        perror("Fatal Error");
        return 1;
    }

    // initializing each memory cell with 0
    for (int i = 0; i < MAXMEM; ++i)
        memory[i] = 0;

    size_t allocated = 100;
    code = (char *)malloc(allocated);
    if (code == NULL)
    {
        printf("Error: Unable to allocate memory\n");
        return 1;
    }

    atexit(free_memory);

    // copying brainfuck code from the file to variable 'code'
    int c, i;
    for (i = 0; (c = fgetc(fptr)) != EOF;)
    {
        // copying only brainfuck operators
        if (c == '>' || c == '<' || c == '+' || c == '-' || c == ',' || c == '.' || c == '[' || c == ']')
        {
            // allocate more memory to variable 'code' if needed
            if (i == allocated - 1)
            {
                char *new_code = (char *)realloc(code, allocated += 50);
                if (new_code == NULL)
                {
                    printf("Error: Unable to allocate memory\n");
                    return 1;
                }
                code = new_code;
            }

            code[i++] = c;
        }
    }
    code[i] = '\0';

    // closing the open brainfuck file
    fclose(fptr);

    // interpreter
    for (i = 0; (c = code[i]) != '\0'; ++i)
    {
        if (skip)
        {
            if (c == '[')
                ++opened;
            else if (c == ']')
            {
                if (opened > 0)
                    --opened;
                else
                    skip = false;
            }

            continue;
        }

        if (c == '>')
        {
            if (ptr == MAXMEM - 1)
            {
                printf("Runtime Error: Cell index out of range (>%d)\n", MAXMEM - 1);
                printf("Pointer is already at the last memory cell.\n");
                return 0;
            }

            ++ptr;
        }
        else if (c == '<')
        {
            if (ptr == 0)
            {
                printf("Runtime Error: Cell index out of range (<0)\n");
                printf("Pointer is already at the first memory cell.\n");
                return 0;
            }

            --ptr;
        }
        else if (c == '+')
            ++memory[ptr];
        else if (c == '-')
            --memory[ptr];
        else if (c == ',')
            memory[ptr] = getchar();
        else if (c == '.')
            putchar(memory[ptr]);
        else if (c == '[')
        {
            if (memory[ptr] == 0)
                skip = true;
            else
                push(&loop_indices, i);
        }
        else if (c == ']')
        {
            if (loop_indices == NULL)
            {
                printf("Runtime Error: No matching '[' found for current ']'\n");
                return 0;
            }

            if (memory[ptr] == 0)
                pop(&loop_indices);
            else
                i = loop_indices->value;
        }
    }

    if (loop_indices != NULL)
        printf("Warning: Missing ']'\n");

    return 0;
}

void free_memory()
{
    free(code);
    while (pop(&loop_indices) != STACK_EMPTY)
        ;
}