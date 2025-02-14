#include <vm.h>
#include <classfile/attr.h>
#include <classfile/read.h>
#include <native/runtime.h>
#include <native/stack.h>
#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <util.h>

extern uint8_t *pc;
extern void parse_instruction(vm_t *vm, ClassFile *cf, stack_frame *frame);

attribute_info *find_attribute(attribute_info *attrs, size_t size, int id)
{
    for (size_t i = 0; i < size; i++) {
        if (attrs[i].synth_attr_type == id)
            return &attrs[i];
    }
    return NULL;
}

void bytecode_exec(vm_t *vm, ClassFile *cf, Method *method) {
    debug_fprintf(stdout, "Entering bytecode execution: stack=%d, locals=%d", method->max_stack, method->max_locals); 
    pc = method->code;
    stack_frame *frame = push_frame(vm->thread_current, method);
    while (pc < method->code + method->code_length) {
        parse_instruction(vm, cf, frame);
    }
}

int entry(vm_t *vm, ClassFile *cf)
{
    Method* main_method = get_main(cf);

    if (main_method == NULL) {
        fprintf(stderr, "\x1b[37;41;1mError\x1b[0m: No suitable exection entry point found\n");
        destroy_vm(vm);
        return 1;
    }

    bytecode_exec(vm, cf, main_method);

    return 0;
}
