#ifndef MICROJVM_VM_H
#define MICROJVM_VM_H

#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int capacity;
    int top;
    uint64_t *values;
} operand_stack;

typedef struct {
    uint32_t local_vars_count;
    int *local_vars;
    operand_stack *operand_stack;
} stack_frame;

typedef struct {
    size_t frame_size;
    stack_frame *frames;
} vm_thread;

typedef struct {
    size_t loaded_classes_count;
    class_file **cfs;
    size_t threads_count;
    vm_thread *threads;
} vm_t;

typedef struct {
    int no_default_lib;
    int classpath_len;
    char **classpath;
    char *main;
} vm_options;

vm_t *create_vm();
void  load_class(vm_t *vm, class_file *cf);
void  load_classes(vm_t *vm, class_file **classes, size_t count);
void  destroy_vm(vm_t *vm);

#endif // MICROJVM_VM_H
