#include "classfile/read.h"
#include <classfile/types.h>
#include <stdlib.h>
#include <stddef.h>
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
    vm->threads_count = 0;
    vm->threads = NULL;
    return vm;
}

void load_class(vm_t *vm, class_file *cf)
{
    vm->loaded_classes_count++;
    vm->cfs = realloc(vm->cfs, vm->loaded_classes_count * sizeof(class_file*));
    vm->cfs[vm->loaded_classes_count - 1] = cf;
}

void load_classes(vm_t *vm, class_file **classes, size_t count)
{
    vm->loaded_classes_count += count;
    vm->cfs = realloc(vm->cfs, vm->loaded_classes_count);
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
        free_classfile(vm->cfs[i]);
    }
    free(vm->cfs);
}
