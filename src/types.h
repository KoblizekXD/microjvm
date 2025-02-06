#ifndef MICROJVM_TYPES_H
#define MICROJVM_TYPES_H

#include <stdint.h>
#include <stddef.h>

#define CONSTANT_Class	            7
#define CONSTANT_Fieldref	        9
#define CONSTANT_Methodref	        10
#define CONSTANT_InterfaceMethodref	11
#define CONSTANT_String	            8
#define CONSTANT_Integer	        3
#define CONSTANT_Float	            4
#define CONSTANT_Long	            5
#define CONSTANT_Double	            6
#define CONSTANT_NameAndType	    12
#define CONSTANT_Utf8	            1
#define CONSTANT_MethodHandle	    15
#define CONSTANT_MethodType	        16
#define CONSTANT_Dynamic	        17
#define CONSTANT_InvokeDynamic	    18
#define CONSTANT_Module	            19
#define CONSTANT_Package	        20

typedef struct {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t* info;
} attribute_info;

typedef struct {
    uint8_t tag;
    union {
        struct {
            uint16_t name_index;
        } class_info;
        struct {
            uint16_t class_index;
            uint16_t name_and_type_index;
        } field_method_ifmethod_ref_info;
        struct {
            uint32_t string_index;
        } string_info;
        struct {
            uint32_t bytes;
        } int_float_info;
        struct {
            uint32_t high_bytes;
            uint32_t low_bytes;
        } long_double_info;
        struct {
            uint16_t name_index;
            uint16_t descriptor_index;
        } name_and_type_info;
        struct {
            uint16_t length;
            uint8_t *bytes;
        } utf8_info;
    } info;
} cp_info;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info* attributes;
} field_info;

typedef struct {
    uint16_t access_flags;
    uint16_t name_index;
    uint16_t descriptor_index;
    uint16_t attributes_count;
    attribute_info* attributes;
} method_info;

typedef struct class_file {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t constant_pool_count;
    cp_info* constant_pool;
    uint16_t access_flags;
    uint16_t this_class;
    uint16_t super_class;
    uint16_t interfaces_count;
    uint16_t* interfaces;
    uint16_t fields_count;
    field_info* fields;
    uint16_t methods_count;
    method_info* methods;
    uint16_t attributes_count;
    attribute_info* attributes;
} class_file;

#endif // MICROJVM_TYPES_H
