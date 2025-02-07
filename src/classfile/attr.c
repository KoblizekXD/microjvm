#include <classfile/attr.h>

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
        
    } else if (is_attr(cf, *info, "NestHost")) {
        read_16(info->data.nest_host.host_class_index);
    } else if (is_attr(cf, *info, "NestMembers")) {
        read_16(info->data.nest_members.number_of_classes);
        fread(info->data.nest_members.classes, sizeof(uint16_t), info->data.nest_members.number_of_classes, stream); 
    } else if (is_attr(cf, *info, "PermittedSubclasses")) {
        read_16(info->data.permitted_subclasses.number_of_classes);
        fread(info->data.permitted_subclasses.classes, sizeof(uint16_t), info->data.permitted_subclasses.number_of_classes, stream); 
    } else {
        fprintf(stderr, "Error: Cannot determine attribute %.*s\n", cf.constant_pool[info->attribute_name_index - 1].info.utf8_info.length,
                cf.constant_pool[info->attribute_name_index - 1].info.utf8_info.bytes);
    }

    return 1;
}
