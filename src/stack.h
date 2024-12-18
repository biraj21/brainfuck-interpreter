#ifndef STACK_H
#define STACK_H

#include <stdbool.h>

#define STACK_INIT {.top = NULL}

typedef struct _Node {
  unsigned short value;
  struct _Node *next;
} Node;

typedef struct {
  Node *top;
} Stack;

bool stack_is_empty(Stack *self);
int stack_push(Stack *self, unsigned short value);
unsigned short stack_pop(Stack *self);

#endif
