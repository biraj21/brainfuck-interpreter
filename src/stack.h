#ifndef STACK_H
#define STACK_H

#define STACK_INIT { .top = NULL }
#define STACK_EMPTY -1

typedef struct _Node {
    unsigned short value;
    struct _Node *next;
} Node;

typedef struct {
    Node *top;
} Stack;

int push(Stack *self, unsigned short value);
short pop(Stack *self);

#endif
