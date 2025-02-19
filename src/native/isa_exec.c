#include <util.h>
#include <classfile/types.h>
#include <vm.h>
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

void parse_instruction(vm_t *vm, ClassFile *cf, stack_frame *frame)
{
    uint8_t* addr = (uint8_t*) pc++;
    switch (*addr) {
        case GETSTATIC: {
            uint8_t indexbyte1 = *pc++;
            uint16_t index = (indexbyte1 << 8) | *pc++;
            struct member_ref fref = cf->constant_pool[index - 1].info.field_method_ifmethod_ref_info;
            struct _utf8_info target_type = cf->constant_pool[cf->constant_pool[fref.class_index - 1].info.class_info.name_index - 1].info.utf8_info;
            struct _utf8_info target_name = cf->constant_pool[cf->constant_pool[fref.name_and_type_index - 1].info.name_and_type_info.name_index - 1].info.utf8_info;
            ClassFile* cf = LoadClassUtf8(vm, target_type, 1);
            Field* f = GetFieldUtf8(cf, target_name); 
            break;
        }
        case LDC: {
            uint8_t index = *pc++;
            break;
        }
        case INVOKESTATIC: {
            uint8_t indexbyte1 = *pc++;
            uint16_t index = (indexbyte1 << 8) | *pc++;
            struct member_ref mref = cf->constant_pool[index - 1].info.field_method_ifmethod_ref_info;
            struct name_type_info nt_info = cf->constant_pool[mref.name_and_type_index - 1].info.name_and_type_info;
            struct _utf8_info target_name = cf->constant_pool[cf->constant_pool[mref.class_index - 1].info.class_info.name_index - 1].info.utf8_info;
            ClassFile* target_cf = LoadClassUtf8(vm, target_name, 1);

            Method* method = GetMethodUtf8(target_cf, cf->constant_pool[nt_info.name_index - 1].info.utf8_info);
            stack_frame* new_frame = push_frame(vm->thread_current, method);
            size_t arg_count = get_arg_count(method->descriptor);
            for (size_t i = 0; i < arg_count; i++) {
                new_frame->local_vars[i] = frame->operand_stack->values[frame->operand_stack->top];
                frame->operand_stack->top--;
                new_frame->local_vars_count++;
            }
            ExecuteBytecode(vm, cf, method, frame);
            pop_frame(vm->thread_current);
            break;
        }
    }
}
