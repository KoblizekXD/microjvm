#include "util.h"
#include <stddef.h>
#include <stdint.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <classfile/attr.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <endian.h>
#include <unistd.h>

#define NONNULL_ASSIGN(ASSIGN_TO, WHAT) if (WHAT == NULL) return NULL; \
                                                  ASSIGN_TO = WHAT;
#define read_8(SAVE_TO) if (fread(&SAVE_TO, sizeof(uint8_t), 1, stream) != 1)

cp_info *read_cp(FILE *stream, size_t entries /* - 1 */);
attribute_info *read_attr(FILE *stream, size_t entries, class_file cf);
field_info *read_fields(FILE *stream, size_t entries, class_file cf);
method_info *read_methods(FILE *stream, size_t entries, class_file cf);

class_file *read_classfile(FILE *stream)
{
    if (stream == NULL) 
    {
        errno = EINVAL;
        return NULL;
    }

    uint32_t temp_32;
    uint32_t temp_16;
    size_t result = 1;

    class_file* cf = (class_file*) malloc(sizeof(class_file));
    read_32(cf->magic);
    read_16(cf->minor_version);
    read_16(cf->major_version);
    read_16(cf->constant_pool_count);
    cf->constant_pool = read_cp(stream, cf->constant_pool_count - 1);
    if (cf->constant_pool == NULL) {
        errprintf("Failed to read constant pool");
        return NULL;
    }
    read_16(cf->access_flags);
    read_16(cf->this_class);
    read_16(cf->super_class);
    read_16(cf->interfaces_count);
    cf->interfaces = (uint16_t*) malloc(sizeof(uint16_t) * cf->interfaces_count);
    fread(cf->interfaces, sizeof(uint16_t), cf->interfaces_count, stream);
    for (size_t i = 0; i < cf->interfaces_count; i++) {
        cf->interfaces[i] = be16toh(cf->interfaces[i]);
    }
    read_16(cf->fields_count);
    cf->fields = read_fields(stream, cf->fields_count, *cf);
    read_16(cf->methods_count);
    cf->methods = read_methods(stream, cf->methods_count, *cf);
    read_16(cf->attributes_count);
    cf->attributes = read_attr(stream, cf->attributes_count, *cf);

    return cf;
}

cp_info *read_cp(FILE *stream, size_t entries /* - 1 */)
{
    cp_info *info = (cp_info*) malloc(entries * sizeof(cp_info));

    uint16_t temp_16;
    uint32_t temp_32;
    size_t result;

    for (size_t i = 0; i < entries; i++) {
        uint8_t tag;
        read_8(tag) {
            return NULL;
        }
        info[i].tag = tag;
        // When adding new constants, don't forget to add them to cf_free too!!
        switch (tag) {
            case CONSTANT_Class:
                read_16(info[i].info.class_info.name_index);
                break;
            case CONSTANT_Fieldref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_Methodref:
                read_16(info[i].info.field_method_ifmethod_ref_info.class_index);
                read_16(info[i].info.field_method_ifmethod_ref_info.name_and_type_index);
                break;
            case CONSTANT_String:
                read_16(info[i].info.string_info.string_index);
                break;
            case CONSTANT_Integer:
            case CONSTANT_Float:
                read_32(info[i].info.int_float_info.bytes);
                break;
            case CONSTANT_Long:
            case CONSTANT_Double:
                read_32(info[i].info.long_double_info.high_bytes);
                read_32(info[i].info.long_double_info.low_bytes);
                i++;
                break;
            case CONSTANT_NameAndType:
                read_16(info[i].info.name_and_type_info.name_index);
                read_16(info[i].info.name_and_type_info.descriptor_index);
                break;
            case CONSTANT_Utf8:
                read_16(info[i].info.utf8_info.length);
                info[i].info.utf8_info.bytes = (uint8_t*) malloc(info[i].info.utf8_info.length * sizeof(uint8_t));
                result = fread(info[i].info.utf8_info.bytes, sizeof(uint8_t), info[i].info.utf8_info.length, stream);
                if (result != info[i].info.utf8_info.length)
                    return NULL;
                break;
            case CONSTANT_MethodHandle:
                read_8(info[i].info.mh_info.reference_kind) return NULL;
                read_16(info[i].info.mh_info.reference_index);
                break;
            case CONSTANT_MethodType:
                read_16(info[i].info.mt_info.descriptor_index);
                break;
            case CONSTANT_Dynamic:
            case CONSTANT_InvokeDynamic:
                read_16(info[i].info.dyn_invoke_dyn_info.bootstrap_method_attr_index);
                read_16(info[i].info.dyn_invoke_dyn_info.name_and_type_index);
                break;
            case CONSTANT_Module:
            case CONSTANT_Package:
                read_16(info[i].info.module_package_info.name_index);
                break;
            default:
                fprintf(stderr, "Invalid constant pool tag: %d\n", tag);
                return NULL;
        }
    }
    return info;
}

attribute_info *read_attr(FILE *stream, size_t entries, class_file cf)
{
    uint16_t temp_16;
    uint32_t temp_32;
    size_t result;
    attribute_info *attr = (attribute_info*) malloc(sizeof(attribute_info) * entries);
    for (size_t i = 0; i < entries; i++) {
        read_16(attr[i].attribute_name_index);
        read_32(attr[i].attribute_length);
        read_attr_data(stream, cf, &attr[i]);
        // attr[i].info = (uint8_t*) malloc(attr[i].attribute_length * sizeof(uint8_t));
        // fread(attr[i].info, sizeof(uint8_t), attr[i].attribute_length, stream);
    }
    return attr;
}

field_info *read_fields(FILE *stream, size_t entries, class_file cf)
{
    uint16_t temp_16;
    size_t result;
    field_info *fields = (field_info*) malloc(sizeof(field_info) * entries);

    for (size_t i = 0; i < entries; i++) {
        read_16(fields[i].access_flags);
        read_16(fields[i].name_index);
        read_16(fields[i].descriptor_index);
        read_16(fields[i].attributes_count);
        fields[i].attributes = read_attr(stream, fields[i].attributes_count, cf);
    }

    return fields;
}

method_info *read_methods(FILE *stream, size_t entries, class_file cf)
{
    uint16_t temp_16;
    size_t result;
    method_info *methods = (method_info*) malloc(sizeof(method_info) * entries);

    for (size_t i = 0; i < entries; i++) {
        read_16(methods[i].access_flags);
        read_16(methods[i].name_index);
        read_16(methods[i].descriptor_index);
        read_16(methods[i].attributes_count);
        methods[i].attributes = read_attr(stream, methods[i].attributes_count, cf);
    }

    return methods;
}

