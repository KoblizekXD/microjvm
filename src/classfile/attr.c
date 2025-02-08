#include <classfile/read.h>
#include <classfile/types.h>
#include <classfile/attr.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

int is_attr(class_file cf, attribute_info attr, const char *name)
{
    cp_info e = cf.constant_pool[attr.attribute_name_index - 1];
    if (e.info.utf8_info.length != strlen(name)) return 0;

    for (size_t i = 0; i < e.info.utf8_info.length; i++)
    {
        if (e.info.utf8_info.bytes[i] != name[i]) return 0;
    }

    return 1;
}

void *read_attr_data(FILE *stream, class_file cf, attribute_info *info)
{
    uint32_t temp_32;
    uint16_t temp_16;
    size_t result = 1;

    if (is_attr(cf, *info, "ConstantValue")) {
        info->synth_attr_type = CONSTANT_VALUE;
        read_16(info->data.constant_value.constant_value_index);
    } else if (is_attr(cf, *info, "Code")) {
        info->synth_attr_type = CODE;
        read_16(info->data.code_attribute.max_stack);
        read_16(info->data.code_attribute.max_locals);
        read_32(info->data.code_attribute.code_length);
        info->data.code_attribute.code = (uint8_t*) malloc(info->data.code_attribute.code_length);
        fread(info->data.code_attribute.code, sizeof(uint8_t), info->data.code_attribute.code_length, stream); 
        read_16(info->data.code_attribute.exception_table_length);
        info->data.code_attribute.exception_table = (struct _exc_table*) malloc(sizeof(struct _exc_table) * info->data.code_attribute.exception_table_length);
        for (size_t i = 0; i < info->data.code_attribute.exception_table_length; i++) {
            read_16(info->data.code_attribute.exception_table[i].start_pc);
            read_16(info->data.code_attribute.exception_table[i].end_pc);
            read_16(info->data.code_attribute.exception_table[i].handler_pc);
            read_16(info->data.code_attribute.exception_table[i].catch_type);
        }
        read_16(info->data.code_attribute.attributes_count);
        info->data.code_attribute.attributes = read_attr(stream, info->data.code_attribute.attributes_count, cf);
    } else if (is_attr(cf, *info, "StackMapTable")) {
        info->synth_attr_type = STACK_MAP_TABLE;
        fseek(stream, info->attribute_length, SEEK_CUR);
    } else if (is_attr(cf, *info, "BootstrapMethods")) {
        info->synth_attr_type = BOOTSTRAP_METHODS;
        read_16(info->data.bootstrap_methods.num_bootstrap_methods);
        info->data.bootstrap_methods.bootstrap_methods = (struct _bootstrap_methods*) malloc(sizeof(struct _bootstrap_methods) * info->data.bootstrap_methods.num_bootstrap_methods);
        for (size_t i = 0; i < info->data.bootstrap_methods.num_bootstrap_methods; i++) {
            read_16(info->data.bootstrap_methods.bootstrap_methods[i].bootstrap_method_ref);
            read_16(info->data.bootstrap_methods.bootstrap_methods[i].num_bootstrap_args);
            info->data.bootstrap_methods.bootstrap_methods[i].bootstrap_arguments = (uint16_t*) malloc(sizeof(uint16_t) * info->data.bootstrap_methods.bootstrap_methods[i].num_bootstrap_args);
        }
    } else if (is_attr(cf, *info, "NestHost")) {
        info->synth_attr_type = NEST_HOST;
        read_16(info->data.nest_host.host_class_index);
    } else if (is_attr(cf, *info, "NestMembers")) {
        info->synth_attr_type = NEST_MEMBERS;
        read_16(info->data.nest_members.number_of_classes);
        fread(info->data.nest_members.classes, sizeof(uint16_t), info->data.nest_members.number_of_classes, stream); 
    } else if (is_attr(cf, *info, "PermittedSubclasses")) {
        info->synth_attr_type = PERMITTED_SUBCLASSES;
        read_16(info->data.permitted_subclasses.number_of_classes);
        fread(info->data.permitted_subclasses.classes, sizeof(uint16_t), info->data.permitted_subclasses.number_of_classes, stream); 
    } else {
        info->synth_attr_type = ATTR_UNKNOWN;
        fprintf(stderr, "Error: Cannot determine attribute %.*s\n", cf.constant_pool[info->attribute_name_index - 1].info.utf8_info.length,
                cf.constant_pool[info->attribute_name_index - 1].info.utf8_info.bytes);
        fseek(stream, info->attribute_length, SEEK_CUR);
    }

    return (void*) 1;
}
