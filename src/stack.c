#include "stack.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

bool stack_is_empty(Stack *self) { return self->top == NULL; }

int stack_push(Stack *self, unsigned short value) {
  if (self == NULL) {
    fputs("stack_push(): argument 'self' cannot be NULL\n", stderr);
    exit(EXIT_FAILURE);
  }

  Node *new_node = malloc(sizeof(Node));
  if (new_node == NULL) {
    return -1;
  }

  new_node->value = value;
  new_node->next = self->top;
  self->top = new_node;
  return 0;
}

unsigned short stack_pop(Stack *self) {
  if (self == NULL) {
    fputs("stack_pop(): argument 'self' cannot be NULL\n", stderr);
    exit(EXIT_FAILURE);
  }

  if (stack_is_empty(self)) {
    fputs("stack_pop(): stack is empty\n", stderr);
    exit(EXIT_FAILURE);
  }

  unsigned short value = self->top->value;
  Node *tmp = self->top;
  self->top = tmp->next;
  free(tmp);
  return value;
}
