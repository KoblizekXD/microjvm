#ifndef MICROJVM_VM_H
#define MICROJVM_VM_H

#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int no_default_lib;
    size_t classpath_len;
    char **classpath;
    char *main;
} vm_options;

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
    vm_options *opts;
    size_t loaded_classes_count;
    ClassFile **cfs;

    size_t threads_count;
    vm_thread *threads;
    vm_thread *thread_current;
} vm_t;

vm_t*           create_vm(vm_options *opts);
void            load_class(vm_t *vm, ClassFile *cf);
void            load_classes(vm_t *vm, ClassFile **classes, size_t count);
void            destroy_vm(vm_t *vm);
stack_frame*    push_frame(vm_thread *thread, Method *method);
ClassFile*      find_class(vm_t *vm, const char *name);
ClassFile*      LoadClass(vm_t *vm, const char *name, int initialize);
ClassFile*      LoadClassFromFile(vm_t *vm, const char *path, int initialize);

#endif // MICROJVM_VM_H
