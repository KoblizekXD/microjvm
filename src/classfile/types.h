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

#define ACC_PUBLIC	     0x0001
#define ACC_PRIVATE      0x0002
#define ACC_PROTECTED    0x0004
#define ACC_STATIC       0x0008
#define ACC_FINAL	     0x0010
#define ACC_SUPER	     0x0020
#define ACC_SYNCHRONIZED 0x0020
#define ACC_BRIDGE       0x0040
#define ACC_VOLATILE     0x0040
#define ACC_TRANSIENT    0x0080
#define ACC_VARARGS      0x0080
#define ACC_NATIVE       0x0100
#define ACC_INTERFACE	 0x0200
#define ACC_ABSTRACT	 0x0400
#define ACC_STRICT       0x0800
#define ACC_SYNTHETIC	 0x1000
#define ACC_ANNOTATION	 0x2000
#define ACC_ENUM	     0x4000
#define ACC_MODULE	     0x8000

typedef struct _attribute_info {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint8_t* info;
    union {
        struct {
            uint16_t constantvalue_index;
        } constant_value;
        struct {
            uint16_t max_stack;
            uint16_t max_locals;
            uint32_t code_length;
            uint8_t* code;
            uint16_t exception_table_length;
            struct {
                uint16_t start_pc;
                uint16_t end_pc;
                uint16_t handler_pc;
                uint16_t catch_type;
            } *exception_table;
            uint16_t attributes_count;
            struct _attribute_info *attributes;
        } code_attribute;
        struct {
            uint16_t entries;
            // Todo
        } stackmap_table;
        struct {
            uint16_t num_bootstrap_methods;
            struct {
                uint16_t bootstrap_method_ref;
                uint16_t num_bootstrap_args;
                uint16_t *bootstrap_arguments;
            } *bootstrap_methods;
        } bootstrap_methods;
        struct {
            uint16_t host_class_index;
        } nest_host;
        struct {
            uint16_t number_of_classes;
            uint16_t *classes;
        } nest_members;
        struct {
            uint16_t number_of_classes;
            uint16_t *classes;
        } permitted_subclasses;
    } data;
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
