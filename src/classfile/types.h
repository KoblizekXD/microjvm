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

// Structures related to class file attributes

struct _attribute_info;

struct attr_constant_value;
struct attr_code;
struct attr_stackmap_table;
struct attr_bootstrap_methods;
struct attr_nest_host;
struct attr_nest_members;
struct attr_permitted_subclasses;

typedef struct attr_constant_value {
    uint16_t constant_value_index;
} constant_value;

typedef struct attr_code {
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t* code;
    uint16_t exception_table_length;
    struct _exc_table {
        uint16_t start_pc;
        uint16_t end_pc;
        uint16_t handler_pc;
        uint16_t catch_type;
    } *exception_table;
    uint16_t attributes_count;
    struct _attribute_info *attributes;
} code;

typedef struct attr_stackmap_table {
    uint16_t number_of_entries;
    struct _stackmap_frame {

    } *entries;
} stackmap_table;

typedef struct attr_bootstrap_methods {
    uint16_t num_bootstrap_methods;
    struct _bootstrap_methods {
        uint16_t bootstrap_method_ref;
        uint16_t num_bootstrap_args;
        uint16_t *bootstrap_arguments;
    } *bootstrap_methods;
} bootstrap_methods;

typedef struct attr_nest_host {
    uint16_t host_class_index;
} nest_host;

typedef struct attr_nest_members {
    uint16_t number_of_classes;
    uint16_t *classes;
} nest_members;

typedef struct attr_permitted_subclasses {
    uint16_t number_of_classes;
    uint16_t *classes;
} permitted_subclasses;

typedef struct _attribute_info {
    uint16_t attribute_name_index;
    uint32_t attribute_length;
    uint16_t synth_attr_type; // Synthetically added, so identifying the attribute is easier. Not actually present in the bytecode
    union {
        constant_value constant_value;
        code code_attribute;
        bootstrap_methods bootstrap_methods;
        stackmap_table stackmap_table;
        nest_host nest_host;
        nest_members nest_members;
        permitted_subclasses permitted_subclasses;
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
            uint16_t string_index;
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
        struct _utf8_info {
            uint16_t length;
            uint8_t *bytes;
        } utf8_info;
        struct _method_handle_info {
            uint8_t reference_kind;
            uint16_t reference_index;
        } mh_info;
        struct _method_type_info {
            uint16_t descriptor_index;
        } mt_info;
        struct {
            uint16_t bootstrap_method_attr_index;
            uint16_t name_and_type_index;
        } dyn_invoke_dyn_info;
        struct {
            uint16_t name_index;
        } module_package_info;
    } info;
} cp_info;

typedef cp_info *ConstantPool;

typedef struct {
    uint16_t access_flags;
    char *descriptor;
    char *name;
    void *value;
    uint16_t attribute_count;
    attribute_info *attributes;
} Field;

#define NOCODE -1

typedef struct {
    uint16_t access_flags;
    char *descriptor;
    char *name;
    uint16_t max_stack;
    uint16_t max_locals;
    uint32_t code_length;
    uint8_t *code;
    void *native_bind;
    uint16_t attribute_count;
    attribute_info *attributes;
} Method;

typedef struct {
    uint32_t magic;
    uint16_t minor_version;
    uint16_t major_version;
    uint16_t access_flags;
    size_t contant_pool_size;
    ConstantPool constant_pool;
    char *name;
    uint8_t has_superclass;
    char *super_name;
    size_t interface_count;
    char **interfaces;
    uint16_t field_count;
    Field *fields;
    uint16_t method_count;
    Method *methods;
    size_t attribute_count;
    attribute_info *attributes;
} ClassFile;

#endif // MICROJVM_TYPES_H
