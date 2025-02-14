#include <classfile/types.h>
#include <vm.h>
#include <native/stack.h>
#include <native/runtime.h>
#include <isa.h>
#include <stdint.h>

extern uint8_t *pc;

ClassFile *resolve_class(struct _utf8_info utf_data)
{
    
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
            break;
        }     
    }
}
