#include <classfile/read.h>
#include <classfile/types.h>
#include <stdint.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <vm.h>

vm_thread *create_thread()
{
    vm_thread *th = (vm_thread*) malloc(sizeof(vm_thread));
    th->frame_size = 0;
    th->frames = NULL;
    return th;
}

vm_t *create_vm()
{
    vm_t *vm = (vm_t*) malloc(sizeof(vm_t));
    vm->cfs = NULL;
    vm->loaded_classes_count = 0;
    vm->threads_count = 1;
    vm->threads = calloc(1, 1 * sizeof(vm_thread));
    vm->thread_current = vm->threads;
    return vm;
}

void load_class(vm_t *vm, ClassFile *cf)
{
    vm->loaded_classes_count++;
    vm->cfs = realloc(vm->cfs, vm->loaded_classes_count * sizeof(ClassFile*));
    vm->cfs[vm->loaded_classes_count - 1] = cf;
}

stack_frame* push_frame(vm_thread *thread, Method *method)
{
    thread->frame_size++;
    thread->frames = realloc(thread->frames, sizeof(stack_frame) * thread->frame_size);
    thread->frames[thread->frame_size - 1] = (stack_frame) {
        .local_vars = (int*) malloc(sizeof(int) * method->max_locals),
        .local_vars_count = 0,
        .operand_stack = (operand_stack*) malloc(sizeof(operand_stack))
    };
    thread->frames[thread->frame_size - 1].operand_stack->capacity = method->max_stack;
    thread->frames[thread->frame_size - 1].operand_stack->top = 0;
    thread->frames[thread->frame_size - 1].operand_stack->values = malloc(sizeof(uint64_t) * method->max_stack);

    return &thread->frames[thread->frame_size - 1];
}

void load_classes(vm_t *vm, ClassFile **classes, size_t count)
{
    vm->loaded_classes_count += count;
    vm->cfs = realloc(vm->cfs, vm->loaded_classes_count * sizeof(ClassFile*));
    for (size_t i = 0; i < count; i++) {
        vm->cfs[vm->loaded_classes_count - count + i] = classes[i];
    }
}

void destroy_vm(vm_t *vm)
{
    for (size_t i = 0; i < vm->threads_count; i++) {
        for (size_t j = 0; j < vm->threads[i].frame_size; j++) {
            free(vm->threads[i].frames[j].operand_stack->values);
            free(vm->threads[i].frames[j].operand_stack);
            free(vm->threads[i].frames[j].local_vars);
        }
        free(vm->threads[i].frames);
    }
    free(vm->threads);
    for (size_t i = 0; i < vm->loaded_classes_count; i++) {
        FreeClassFile(vm->cfs[i]);
    }
    free(vm->cfs);
    free(vm);
}

ClassFile* find_class(vm_t *vm, const char *name)
{
    for (size_t i = 0; i < vm->loaded_classes_count; i++) {
        if (strcmp(vm->cfs[i]->name, name) == 0) return vm->cfs[i];
    }
    return NULL;
}
