#include "util.h"
#include <classfile/types.h>
#include <vm.h>
#include <native/stack.h>
#include <native/runtime.h>
#include <isa.h>
#include <stdint.h>

extern uint8_t *pc;

Field *find_field(ClassFile* cf, struct _utf8_info name)
{
    for (size_t i = 0; i < cf->field_count; i++) {
        if (utf8eq(name, cf->fields[i].name))
            return &cf->fields[i];
    }
    return NULL;
}

// cf       = Class File instance corresponding to the currently executing code attribute
// c        = Code attribute
// op_stack = Operand stack
void parse_instruction(vm_t *vm, ClassFile *cf, stack_frame *frame)
{
    uint8_t* addr = (uint8_t*) pc++;
    switch (*addr) {
        case GETSTATIC: {
            uint8_t indexbyte1 = *pc++;
            uint16_t index = (indexbyte1 << 8) | *pc++;
            cp_info fref = cf->constant_pool[index - 1];
            struct _utf8_info target_type = cf->constant_pool[cf->constant_pool[fref.info.field_method_ifmethod_ref_info.class_index - 1].info.class_info.name_index - 1].info.utf8_info;
            struct _utf8_info field_name = cf->constant_pool[cf->constant_pool[fref.info.field_method_ifmethod_ref_info.class_index - 1].info.name_and_type_info.name_index - 1].info.utf8_info;

            ClassFile *cf;
            Field *field;
            for (size_t i = 0; i < vm->loaded_classes_count; i++) {
                cf = vm->cfs[i];
                if (utf8eq(target_type, cf->name)) {
                    field = find_field(cf, field_name);
                    frame->operand_stack->values[frame->operand_stack->top + 1] = (uintptr_t) field->value;
                    frame->operand_stack->top++;
                    break;
                }
            }
            break;
        }    
        case LDC: {
            uint8_t index = *pc++;
            
        }
    }
}
