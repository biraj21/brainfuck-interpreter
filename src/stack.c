#include <stdio.h>
#include <stdlib.h>
#include "stack.h"

int push(Stack *self, unsigned short value) {
    if (self == NULL) {
        fputs("push(): 'self' cannot be NULL\n", stderr);
        exit(1);
    }

    Node *new_node = malloc(sizeof(Node));
    if (new_node == NULL)
        return -1;

    new_node->value = value;
    new_node->next = self->top;
    self->top = new_node;
    return 0;
}

short pop(Stack *self) {
    if (self == NULL) {
        fputs("pop(): 'self' cannot be NULL\n", stderr);
        exit(1);
    }

    if (self->top == NULL)
        return STACK_EMPTY;

    unsigned short value = self->top->value;
    Node *tmp = self->top;
    self->top = tmp->next;
    free(tmp);
    return value;
}
