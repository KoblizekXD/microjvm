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
#include <string.h>
#include <unistd.h>

#define NONNULL_ASSIGN(ASSIGN_TO, WHAT) if (WHAT == NULL) return NULL; \
                                                  ASSIGN_TO = WHAT;
#define read_8(SAVE_TO) if (fread(&SAVE_TO, sizeof(uint8_t), 1, stream) != 1)

cp_info *read_cp(FILE *stream, size_t entries /* - 1 */);
attribute_info *read_attr(FILE *stream, size_t entries, class_file cf);
Field *ReadFields(FILE *stream, size_t entries, cp_info* cf);
Method *ReadMethods(FILE *stream, size_t entries, cp_info *cp);

char *to_string(struct _utf8_info utf8)
{
    char *str = (char*) malloc(utf8.length + 1);
    if (str == NULL) return NULL;

    memcpy(str, utf8.bytes, utf8.length);
    str[utf8.length] = '\0';

    return str;
}

ClassFile *ReadClassFileFromStream(FILE *stream)
{
    if (stream == NULL) 
    {
        errno = EINVAL;
        return NULL;
    }
    uint32_t temp_32;
    uint32_t temp_16;
    size_t result = 1;

    ClassFile *cf = (ClassFile*) malloc(sizeof(ClassFile));
    read_32(cf->magic);
    read_16(cf->minor_version);
    read_16(cf->major_version);
    uint16_t cpool_size;
    read_16(cpool_size);
    cpool_size--;
    cp_info *pool = read_cp(stream, cpool_size);
    if (pool == NULL) {
        errprintf("Failure in reading constant pool");
        return NULL;
    }
    read_16(cf->access_flags);
    uint16_t classIndex;
    read_16(classIndex);
    cf->name = to_string(pool[pool[classIndex - 1].info.class_info.name_index - 1].info.utf8_info);
    read_16(classIndex);
    if (classIndex == 0) cf->super_name = "java/lang/Object";
    cf->super_name = to_string(pool[pool[classIndex - 1].info.class_info.name_index - 1].info.utf8_info);
    read_16(cf->interface_count);
    cf->interfaces = malloc(sizeof(const char*) * cf->interface_count);

    for (size_t i = 0; i < cf->interface_count; i++) {
        read_16(classIndex);
        cf->interfaces[i] = to_string(pool[pool[classIndex - 1].info.class_info.name_index - 1].info.utf8_info);
    }
    read_16(cf->field_count);
    cf->fields = ReadFields(stream, cf->field_count, pool);
    read_16(cf->method_count);
    cf->methods = ReadMethods(stream, cf->method_count, pool);

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

attribute_info *ReadAttributes(FILE *stream, size_t count, cp_info *pool)
{
    uint16_t temp_16;
    uint32_t temp_32;
    size_t result;
    attribute_info *attr = (attribute_info*) malloc(sizeof(attribute_info) * count);
    for (size_t i = 0; i < count; i++) {
        read_16(attr[i].attribute_name_index);
        read_32(attr[i].attribute_length);
        read_attr_data(stream, pool, &attr[i]);
        // attr[i].info = (uint8_t*) malloc(attr[i].attribute_length * sizeof(uint8_t));
        // fread(attr[i].info, sizeof(uint8_t), attr[i].attribute_length, stream);
    }
    return attr;
}

Field *ReadFields(FILE *stream, size_t entries, cp_info *cp)
{
    uint16_t temp_16;
    size_t result;
    Field *fields = (Field*) malloc(sizeof(Field*) * entries);
    uint16_t index;
    for (size_t i = 0; i < entries; i++) {
        read_16(fields[i].access_flags);
        read_16(index);
        fields[i].name = to_string(cp[index - 1].info.utf8_info);
        read_16(index);
        fields[i].descriptor = to_string(cp[index - 1].info.utf8_info);
        fields[i].value = NULL;
        read_16(index);
        ReadAttributes(stream, index, cp);
    }
    return fields;
}

Method *ReadMethods(FILE *stream, size_t entries, cp_info *cp)
{
    uint16_t temp_16;
    size_t result;
    Method *methods = (Method*) malloc(sizeof(Method) * entries);

    for (size_t i = 0; i < entries; i++) {
        read_16(methods[i].access_flags);
        read_16(temp_16);
        methods[i].name = to_string(cp[temp_16 - 1].info.utf8_info);
        read_16(temp_16);
        methods[i].descriptor = to_string(cp[temp_16 - 1].info.utf8_info);
        read_16(temp_16);
        attribute_info *attrs = ReadAttributes(stream, temp_16, cp);
    }

    return methods;
}
