#ifndef MICROJVM_NATIVE_STACK_H
#define MICROJVM_NATIVE_STACK_H

#include <stddef.h>
#include <stdint.h>

// Default 1 MiB stack
#define STACK_DEFAULT_SIZE (1024*1024)

typedef struct _native_stack {
    uint8_t *stack_data;
    size_t  capacity;
    size_t  off;
} stack_t;

stack_t *create(size_t size);
void destroy(stack_t *stack);
void stack_push(stack_t *stack, void *value, size_t size);
void stack_pop(stack_t *stack, void *dest, size_t size);

#endif // MICROJVM_NATIVE_STACK_H
