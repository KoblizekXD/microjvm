#ifndef MICROJVM_RUNTIME_H
#define MICROJVM_RUNTIME_H

#include <vm.h>
#include <classfile/types.h>

int entry(vm_t *vm, class_file *cf);

#endif // MICROJVM_RUNTIME_H
