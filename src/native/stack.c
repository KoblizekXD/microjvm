#include <native/stack.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

stack_t *create(size_t size)
{
    stack_t *stack = calloc(1, sizeof(stack_t));
    stack->stack_data = calloc(1, size);
    stack->capacity = size;
    stack->off = 0;
    return stack;
}

void destroy(stack_t *stack)
{
    free(stack->stack_data);
    free(stack);
}

void stack_push(stack_t *stack, void *value, size_t size)
{
    if (stack->off + size > stack->capacity) {
        fprintf(stderr, "Stack overflow\n");
        exit(1);
    }
    memcpy(stack->stack_data + stack->off, value, size);
    stack->off += size;
}

void stack_pop(stack_t *stack, void *dest, size_t size)
{
    if (stack->off < size) {
        fprintf(stderr, "Stack underflow\n");
        exit(1);
    }
    stack->off -= size;
    memcpy(dest, stack->stack_data + stack->off, size);
}
