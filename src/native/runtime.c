#include <classfile/attr.h>
#include <classfile/read.h>
#include <native/runtime.h>
#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>
#include <util.h>

extern uint64_t pc;

attribute_info *get_attr(attribute_info *attrs, size_t size, int id)
{
    for (size_t i = 0; i < size; i++) {
        if (attrs[i].synth_attr_type == id)
            return &attrs[i];
    }
    return NULL;
}

int entry(class_file *cf)
{
    method_info *main_method = get_main(cf);

    if (main_method == NULL) {
        fprintf(stderr, "\x1b[37;41;1mError\x1b[0m: No suitable exection entry point found\n");
        free_classfile(cf);
        return 1;
    }

    code c = get_attr(main_method->attributes, main_method->attributes_count, CODE)->data.code_attribute;
    pc = (uintptr_t) c.code;

    free_classfile(cf);
    return 0;
}
