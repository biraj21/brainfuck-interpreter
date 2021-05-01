#include <stdbool.h>
#include <stdlib.h>
#include "../include/stack.h"

bool push(Stack *head, unsigned short value)
{
    Stack new_node = malloc(sizeof(Node));
    if (new_node == NULL)
        return false;

    new_node->value = value;
    new_node->next = *head;
    *head = new_node;

    return true;
}

short pop(Stack *head)
{
    if (*head == NULL)
        return STACK_EMPTY;

    unsigned short value = (*head)->value;

    Stack tmp = *head;
    *head = tmp->next;
    free(tmp);

    return value;
}

void clear_stack(Stack *head)
{
    while (pop(head) != STACK_EMPTY)
        ;
}