#ifndef MICROJVM_RUNTIME_H
#define MICROJVM_RUNTIME_H

#include <vm.h>
#include <classfile/types.h>

int entry(vm_t *vm, ClassFile *cf);
// Frame has to be popped manually!
void ExecuteBytecode(vm_t *vm, ClassFile *cf, Method *method, stack_frame *frame);

#endif // MICROJVM_RUNTIME_H
