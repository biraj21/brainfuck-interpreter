#ifndef STACK_H
#define STACK_H

#define STACK_EMPTY -1

typedef struct Node
{
    unsigned short value;
    struct Node *next;
} Node;

typedef Node *Stack;

bool push(Stack *head, unsigned short value);
short pop(Stack *head);
void clear_stack(Stack *head);

#endif