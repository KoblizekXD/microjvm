#ifndef MICROJVM_ATTR_H
#define MICROJVM_ATTR_H

#include <endian.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

int is_attr(class_file cf, attribute_info attr, const char *name)
{
    cp_info e = cf.constant_pool[attr.attribute_name_index - 1];

    for (size_t i = 0; i < e.info.utf8_info.length; i++)
    {
        if (e.info.utf8_info.bytes[i] != name[i]) return 0;
    }

    return 1;
}

int read_attr_data(FILE *stream, class_file cf, attribute_info *info)
{
    uint32_t temp_32;
    uint32_t temp_16;
    size_t result = 1;

    if (is_attr(cf, *info, "ConstantValue")) {
        read_16(info->data.constant_value.constantvalue_index);
    } else if (is_attr(cf, *info, "Code")) {
        read_16(info->data.code_attribute.max_stack);
        read_16(info->data.code_attribute.max_locals);
        read_32(info->data.code_attribute.code_length);
        fread(info->data.code_attribute.code, sizeof(uint8_t), info->data.code_attribute.code_length, stream); 
    } else if (is_attr(cf, *info, "StackMapTable")) {

    } else if (is_attr(cf, *info, "BootstrapMethods")) {
        read_16(info->data.bootstrap_methods.num_bootstrap_methods);

    }

    return 1;
}

#endif // MICROJVM_ATTR_H
