#include <classfile/types.h>
#include <native/stack.h>
#include <native/runtime.h>
#include <isa.h>
#include <stdint.h>

extern uint8_t *pc;

// cf       = Class File instance corresponding to the currently executing code attribute
// c        = Code attribute
// op_stack = Operand stack
void parse_instruction(class_file *cf, code c, stack_t *op_stack)
{
    uint8_t* addr = (uint8_t*) pc++;
    switch (*addr) {
        case GETSTATIC: {
            uint8_t indexbyte1 = *pc++;
            uint16_t index = (indexbyte1 << 8) | *pc++;
            cp_info cp_info = cf->constant_pool[index - 1];
            
            break;
        }     
    }
}
