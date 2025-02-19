#include <vm.h>
#include <classfile/attr.h>
#include <classfile/read.h>
#include <native/runtime.h>
#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <util.h>
#include <dlfcn.h>

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

void execute_native(vm_t *vm, ClassFile *cf, Method *method, stack_frame *frame)
{
    void (*handle)(void *instance, void **args) = (void (*)(void *instance, void **args)) dlsym(vm->sym_handle, method->name);
    
    if (!handle) {
        errprintf("Method symbol not found: %s", method->name);
        return;
    }
    size_t i = get_arg_count(method->descriptor);
    void **args = malloc(sizeof(void*) * i);
    for (size_t j = 0; j < i; j++) {
         _stack_value val = frame->operand_stack->stack_values[frame->operand_stack->top];
         if (val.type == OP_STACK_VALUE_CPOOL_REF) {
             cp_info info = cf->constant_pool[val.value - 1];
             switch (info.tag) {
                 case CONSTANT_String: {
                      info.info.string_info.string_index;
                      break;
                 }
             }
         }
         frame->operand_stack->top--;
    }
    handle(cf, args);
    free(args);
}

void bytecode_exec(vm_t *vm, ClassFile *cf, Method *method)
{
    debug_fprintf(stdout, "Entering bytecode execution: stack=%d, locals=%d", method->max_stack, method->max_locals); 
    pc = method->code;
    stack_frame *frame = push_frame(vm->thread_current, method);
    while (pc < method->code + method->code_length) {
        parse_instruction(vm, cf, frame);
    }
}

void ExecuteBytecode(vm_t *vm, ClassFile *cf, Method *method, stack_frame *frame)
{
    debug_fprintf(stdout, "Entering bytecode execution: stack=%d, locals=%d", method->max_stack, method->max_locals);
    if (method->access_flags & ACC_NATIVE) {
        execute_native(vm, cf, method, frame);
        return;
    }
    uint8_t* old_pc = pc;
    pc = method->code;
    while (pc < method->code + method->code_length) {
        parse_instruction(vm, cf, frame);
    }
    pc = old_pc;
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
