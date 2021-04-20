#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXCHARS 1000
#define MAXMEM 30000
#define STACK_EMPTY -1

// stack
typedef struct Node
{
    short value;
    struct Node *next;
} Node;

typedef Node *Stack;

bool push(Stack *s, short value);
short pop(Stack *s);
void clear_stack(Stack *s);

unsigned char memory[MAXMEM];
short ptr = 0;
char code[MAXCHARS];
Stack loop_indices = NULL;

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: bf <file>\n");
        return 0;
    }

    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL)
    {
        printf("Fatal error: Unable to open file %s.\n", argv[1]);
        return 0;
    }

    // initialize each memory cell with 0
    int i;
    for (i = 0; i < MAXMEM; ++i)
        memory[i] = 0;

    // copying code from file to variable 'code'
    int c;
    for (i = 0; i < MAXCHARS - 1 && (c = fgetc(fptr)) != EOF;)
    {
        // only copy the brainfuck operators
        if (c == '+' || c == '-' || c == '>' || c == '<' || c == '.' || c == ',' || c == '[' || c == ']')
        {
            code[i] = c;
            ++i;
        }
    }
    code[i] = '\0';

    for (i = 0; (c = code[i]) != '\0'; ++i)
    {
        if (c == '+')
            ++memory[ptr];
        else if (c == '-')
            --memory[ptr];
        else if (c == '>')
        {
            if (ptr == MAXMEM - 1)
            {
                printf("Runtime Error: Cell index out of range (>%d)\n", MAXMEM - 1);
                printf("Pointer is already pointing at the last memory cell.\n");

                clear_stack(&loop_indices);
                exit(0);
            }

            ++ptr;
        }
        else if (c == '<')
        {
            if (ptr == 0)
            {
                printf("Runtime Error: Cell index out of range (<0)\n");
                printf("Pointer is already pointing at the first memory cell.\n");

                clear_stack(&loop_indices);
                exit(0);
            }

            --ptr;
        }
        else if (c == ',')
            memory[ptr] = getchar();
        else if (c == '.')
            putchar(memory[ptr]);
        else if (c == '[')
            push(&loop_indices, i);
        else if (c == ']')
        {
            if (loop_indices == NULL)
            {
                printf("Runtime Error: No matching '[' found for current ']'\n");
                exit(0);
            }

            if (memory[ptr] == 0)
                pop(&loop_indices);
            else
                i = loop_indices->value;
        }
    }

    clear_stack(&loop_indices);
    fclose(fptr);
    return 0;
}

// functions for stack
bool push(Stack *s, short value)
{
    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
        return false;

    new_node->value = value;
    new_node->next = *s;

    *s = new_node;
    return true;
}

short pop(Stack *s)
{
    if (*s == NULL)
        return STACK_EMPTY;

    short value = (*s)->value;

    Node *tmp = *s;
    *s = (*s)->next;

    free(tmp);
}

void clear_stack(Stack *s)
{
    while (pop(s) != STACK_EMPTY)
        ;
}