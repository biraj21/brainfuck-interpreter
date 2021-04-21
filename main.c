#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MAXMEM 30000 // number of memory cells (1 byte each)
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

unsigned char memory[MAXMEM]; // memory cells (1 byte each)
short ptr = 0;                // currently pointed memory cell
char *code;
Stack loop_indices = NULL; // stores indices of '[' (opening of a loop)
bool skip = false;         // whether to skip the code or not (loop)
int opened = 0;            // number of '[' (opened loops) while skipping

int main(int argc, const char *argv[])
{
    if (argc < 2)
    {
        printf("Usage: bf <file>\n");
        return 1;
    }

    // opens the brainfuck file
    FILE *fptr = fopen(argv[1], "r");
    if (fptr == NULL)
    {
        printf("Fatal error: Couldn't open file '%s'\n", argv[1]);
        return 1;
    }

    int i;
    // initializes each memory cell with 0
    for (i = 0; i < MAXMEM; ++i)
        memory[i] = 0;

    long allocated = 100;
    code = (char *)malloc(allocated);
    if (code == NULL)
    {
        printf("Error: Unable to allocate memory\n");
        return 1;
    }

    // copying code from file to the variable 'code'
    int c;
    for (i = 0; (c = fgetc(fptr)) != EOF;)
    {
        // only copy the brainfuck operators
        if (c == '+' || c == '-' || c == '>' || c == '<' || c == '.' || c == ',' || c == '[' || c == ']')
        {
            // allocate more memory for the variable 'code' if required
            if (i == allocated - 1)
            {
                code = (char *)realloc(code, allocated += 50);
                if (code == NULL)
                {
                    printf("Error: Unable to allocate memory\n");
                    return 1;
                }
            }

            code[i++] = c;
        }
    }
    code[i] = '\0';

    // closes the brainfuck file that was opened
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
                printf("Pointer is already pointing at the 'last' memory cell.\n");

                free(code);
                clear_stack(&loop_indices);

                return 0;
            }

            ++ptr;
        }
        else if (c == '<')
        {
            if (ptr == 0)
            {
                printf("Runtime Error: Cell index out of range (<0)\n");
                printf("Pointer is already pointing at the 'first' memory cell.\n");

                free(code);
                clear_stack(&loop_indices);

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

    free(code);
    if (loop_indices != NULL)
    {
        printf("Warning: Missing ']'\n");
        clear_stack(&loop_indices);
    }

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

    return value;
}

void clear_stack(Stack *s)
{
    while (pop(s) != STACK_EMPTY)
        ;
}