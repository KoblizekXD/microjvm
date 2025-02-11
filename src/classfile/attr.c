#include <util.h>
#include <classfile/read.h>
#include <classfile/types.h>
#include <classfile/attr.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>

int utf8eq(struct _utf8_info utf8str, const char *cstr)
{
    size_t cstr_len = strlen(cstr);
    if (utf8str.length != cstr_len) return 0;
    for (size_t i = 0; i < cstr_len; i++) {
        if (utf8str.bytes[i] != cstr[i]) return 0;
    }
    return 1;
}

int get_attr(struct _utf8_info *name)
{
    if (!name || !name->bytes) return ATTR_UNKNOWN;

    switch (name->bytes[0]) {
        case 'A':
            if (name->length == 16 && memcmp(name->bytes, "AnnotationDefault", 16) == 0) return ANNOTATION_DEFAULT;
            break;
        case 'B':
            if (name->length == 16 && memcmp(name->bytes, "BootstrapMethods", 16) == 0) return BOOTSTRAP_METHODS;
            break;
        case 'C':
            if (name->length == 4 && memcmp(name->bytes, "Code", 4) == 0) return CODE;
            break;
        case 'D':
            if (name->length == 10 && memcmp(name->bytes, "Deprecated", 10) == 0) return DEPRECATED;
            break;
        case 'E':
            if (name->length == 15 && memcmp(name->bytes, "EnclosingMethod", 15) == 0) return ENCLOSING_METHOD;
            if (name->length == 10 && memcmp(name->bytes, "Exceptions", 10) == 0) return EXCEPTIONS;
            break;
        case 'I':
            if (name->length == 12 && memcmp(name->bytes, "InnerClasses", 12) == 0) return INNER_CLASSES;
            break;
        case 'L':
            if (name->length == 14 && memcmp(name->bytes, "LineNumberTable", 14) == 0) return LINE_NUMBER_TABLE;
            if (name->length == 18 && memcmp(name->bytes, "LocalVariableTable", 18) == 0) return LOCAL_VARIABLE_TABLE;
            if (name->length == 22 && memcmp(name->bytes, "LocalVariableTypeTable", 22) == 0) return LOCAL_VARIABLE_TYPE_TABLE;
            break;
        case 'M':
            if (name->length == 16 && memcmp(name->bytes, "MethodParameters", 16) == 0) return METHOD_PARAMETERS;
            if (name->length == 6 && memcmp(name->bytes, "Module", 6) == 0) return MODULE;
            if (name->length == 16 && memcmp(name->bytes, "ModuleMainClass", 16) == 0) return MODULE_MAIN_CLASS;
            if (name->length == 14 && memcmp(name->bytes, "ModulePackages", 14) == 0) return MODULE_PACKAGES;
            break;
        case 'N':
            if (name->length == 8 && memcmp(name->bytes, "NestHost", 8) == 0) return NEST_HOST;
            if (name->length == 11 && memcmp(name->bytes, "NestMembers", 11) == 0) return NEST_MEMBERS;
            break;
        case 'P':
            if (name->length == 19 && memcmp(name->bytes, "PermittedSubclasses", 19) == 0) return PERMITTED_SUBCLASSES;
            break;
        case 'R':
            if (name->length == 6 && memcmp(name->bytes, "Record", 6) == 0) return RECORD;
            if (name->length == 26 && memcmp(name->bytes, "RuntimeInvisibleAnnotations", 26) == 0) return RUNTIME_INVISIBLE_ANNOTATIONS;
            if (name->length == 32 && memcmp(name->bytes, "RuntimeInvisibleParameterAnnotations", 32) == 0) return RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS;
            if (name->length == 30 && memcmp(name->bytes, "RuntimeInvisibleTypeAnnotations", 30) == 0) return RUNTIME_INVISIBLE_TYPE_ANNOTATIONS;
            if (name->length == 24 && memcmp(name->bytes, "RuntimeVisibleAnnotations", 24) == 0) return RUNTIME_VISIBLE_ANNOTATIONS;
            if (name->length == 30 && memcmp(name->bytes, "RuntimeVisibleParameterAnnotations", 30) == 0) return RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS;
            if (name->length == 28 && memcmp(name->bytes, "RuntimeVisibleTypeAnnotations", 28) == 0) return RUNTIME_VISIBLE_TYPE_ANNOTATIONS;
            break;
        case 'S':
            if (name->length == 9 && memcmp(name->bytes, "Signature", 9) == 0) return SIGNATURE;
            if (name->length == 9 && memcmp(name->bytes, "Synthetic", 9) == 0) return SYNTHETIC;
            if (name->length == 21 && memcmp(name->bytes, "SourceDebugExtension", 21) == 0) return SOURCE_DEBUG_EXTENSION;
            if (name->length == 10 && memcmp(name->bytes, "SourceFile", 10) == 0) return SOURCE_FILE;
            if (name->length == 13 && memcmp(name->bytes, "StackMapTable", 13) == 0) return STACK_MAP_TABLE;
            break;
        case 'V':
            if (name->length == 13 && memcmp(name->bytes, "ConstantValue", 13) == 0) return CONSTANT_VALUE;
            break; 
    }
    return ATTR_UNKNOWN;
}

void *read_attr_data(FILE *stream, class_file cf, attribute_info *info)
{
    uint32_t temp_32;
    uint16_t temp_16;
    size_t result = 1;
    cp_info cp_entry = cf.constant_pool[info->attribute_name_index - 1];
    int attr_id = get_attr(&cp_entry.info.utf8_info);
    info->synth_attr_type = attr_id;

    if (attr_id == CONSTANT_VALUE) {
        read_16(info->data.constant_value.constant_value_index);
    } else if (attr_id == CODE) {
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
    } else if (attr_id == STACK_MAP_TABLE) {
        fseek(stream, info->attribute_length, SEEK_CUR);
    } else if (attr_id == BOOTSTRAP_METHODS) {
        read_16(info->data.bootstrap_methods.num_bootstrap_methods);
        info->data.bootstrap_methods.bootstrap_methods = (struct _bootstrap_methods*) malloc(sizeof(struct _bootstrap_methods) * info->data.bootstrap_methods.num_bootstrap_methods);
        for (size_t i = 0; i < info->data.bootstrap_methods.num_bootstrap_methods; i++) {
            read_16(info->data.bootstrap_methods.bootstrap_methods[i].bootstrap_method_ref);
            read_16(info->data.bootstrap_methods.bootstrap_methods[i].num_bootstrap_args);
            info->data.bootstrap_methods.bootstrap_methods[i].bootstrap_arguments = (uint16_t*) malloc(sizeof(uint16_t) * info->data.bootstrap_methods.bootstrap_methods[i].num_bootstrap_args);
            fread(info->data.bootstrap_methods.bootstrap_methods[i].bootstrap_arguments, sizeof(uint16_t), info->data.bootstrap_methods.bootstrap_methods[i].num_bootstrap_args, stream);
        }
    } else if (attr_id == NEST_HOST) {
        read_16(info->data.nest_host.host_class_index);
    } else if (attr_id == NEST_MEMBERS) {
        read_16(info->data.nest_members.number_of_classes);
        info->data.nest_members.classes = (uint16_t*) malloc(sizeof(uint16_t) * info->data.nest_members.number_of_classes);
        fread(info->data.nest_members.classes, sizeof(uint16_t), info->data.nest_members.number_of_classes, stream); 
    } else if (attr_id == PERMITTED_SUBCLASSES) {
        read_16(info->data.permitted_subclasses.number_of_classes);
        info->data.permitted_subclasses.classes = (uint16_t*) malloc(sizeof(uint16_t) * info->data.permitted_subclasses.number_of_classes);
        fread(info->data.permitted_subclasses.classes, sizeof(uint16_t), info->data.permitted_subclasses.number_of_classes, stream); 
    } else {
        debug_fprintf(stderr, "Error: Cannot determine attribute %.*s", cf.constant_pool[info->attribute_name_index - 1].info.utf8_info.length,
            cf.constant_pool[info->attribute_name_index - 1].info.utf8_info.bytes);
        fseek(stream, info->attribute_length, SEEK_CUR);
    }

    return (void*) 1;
}
