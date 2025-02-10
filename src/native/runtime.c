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
extern void parse_instruction(class_file *cf, code c, stack_t *op_stack);
extern class_file **read_jmod(const char *java_home, const char *jmod_name);

attribute_info *get_attr(attribute_info *attrs, size_t size, int id)
{
    for (size_t i = 0; i < size; i++) {
        if (attrs[i].synth_attr_type == id)
            return &attrs[i];
    }
    return NULL;
}

void bytecode_exec(class_file *cf, code c)
{
    debug_fprintf(stdout, "Entering bytecode execution: stack=%d, locals=%d", c.max_stack, c.max_locals); 
    pc = c.code;
    stack_t *op_stack = create(c.max_stack);

    while (pc < c.code + c.code_length) {
        parse_instruction(cf, c, op_stack);
    }

    destroy(op_stack);
}

int entry(class_file *cf)
{
    read_jmod(getenv("JAVA_HOME"), "java.base");
    method_info *main_method = get_main(cf);

    if (main_method == NULL) {
        fprintf(stderr, "\x1b[37;41;1mError\x1b[0m: No suitable exection entry point found\n");
        free_classfile(cf);
        return 1;
    }

    code c = get_attr(main_method->attributes, main_method->attributes_count, CODE)->data.code_attribute;

    bytecode_exec(cf, c);

    free_classfile(cf);
    return 0;
}
