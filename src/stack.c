#include <stdlib.h>
#include "../include/stack.h"

int push(Stack *head, unsigned short value)
{
    Stack new_node = malloc(sizeof(Node));
    if (new_node == NULL)
        return -1;

    new_node->value = value;
    new_node->next = *head;
    *head = new_node;

    return 0;
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