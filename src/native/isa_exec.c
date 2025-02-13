#include "util.h"
#include <classfile/types.h>
#include <string.h>
#include <vm.h>
#include <native/stack.h>
#include <native/runtime.h>
#include <isa.h>
#include <stdint.h>

extern uint8_t *pc;

class_file *find_classfile(vm_t *vm, struct _utf8_info *utf8)
{
    for (size_t i = 0; i < vm->loaded_classes_count; i++) {
        class_file* potential = vm->cfs[i];
        struct _utf8_info pot_str = potential->constant_pool[potential->constant_pool[potential->this_class - 1].info.class_info.name_index - 1].info.utf8_info;
        if (pot_str.length != utf8->length) continue;
        if (memcmp(utf8->bytes, pot_str.bytes, utf8->length) == 0)
            return potential;
    }
    return NULL;
}

// cf       = Class File instance corresponding to the currently executing code attribute
// c        = Code attribute
// op_stack = Operand stack
void parse_instruction(vm_t *vm, class_file *cf, stack_frame *frame)
{
    uint8_t* addr = (uint8_t*) pc++;
    switch (*addr) {
        case GETSTATIC: {
            uint8_t indexbyte1 = *pc++;
            uint16_t index = (indexbyte1 << 8) | *pc++;
            cp_info cp_info = cf->constant_pool[index - 1];
            class_file* resolved = find_classfile(vm, &cf->constant_pool[cf->constant_pool[cp_info.info.field_method_ifmethod_ref_info.class_index - 1].info.class_info.name_index - 1]
                    .info.utf8_info);
            if (resolved == NULL) {
                errprintf("Java: ClassNotFound: %.*s",
                        cf->constant_pool[cf->constant_pool[cp_info.info.field_method_ifmethod_ref_info.class_index - 1].info.class_info.name_index - 1].info.utf8_info.length,
                        cf->constant_pool[cf->constant_pool[cp_info.info.field_method_ifmethod_ref_info.class_index - 1].info.class_info.name_index - 1].info.utf8_info.bytes);
            }
            
            break;
        }     
    }
}
