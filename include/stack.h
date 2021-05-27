#ifndef STACK_H
#define STACK_H

#define STACK_EMPTY -1

typedef struct _Node
{
    unsigned short value;
    struct _Node *next;
} Node;

typedef Node *Stack;

int push(Stack *head, unsigned short value);
short pop(Stack *head);

#endif