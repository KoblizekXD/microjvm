#include <classfile/attr.h>
#include <stddef.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdio.h>
#include <stdlib.h>

#define free_if_not_null(OBJECT) if (OBJECT != NULL) free(OBJECT);

static void free_attr_info(attribute_info *attr, size_t count)
{
    if (attr == NULL) return;
    for (size_t i = 0; i < count; i++) {
        switch (attr->synth_attr_type) {
            case CODE:
                free(attr->data.code_attribute.code);
                free(attr->data.code_attribute.exception_table);
                free_attr_info(attr->data.code_attribute.attributes, attr->data.code_attribute.attributes_count);
                break;
            case NEST_MEMBERS:
                free(attr->data.nest_members.classes);
                break;
            default:
                printf("Unsure what to free because of unknown tag %d\n", attr->synth_attr_type);
                break;
        }
    }
    free(attr);
}

static void free_fields(field_info *fields, size_t count)
{
    if (fields == NULL) return;
    for (size_t i = 0; i < count; i++) {
        free_attr_info(fields[i].attributes, fields[i].attributes_count);
    }
    free(fields);
}

static void free_methods(method_info *methods, size_t count)
{
    if (methods == NULL) return;
    for (size_t i = 0; i < count; i++) {
        free_attr_info(methods[i].attributes, methods[i].attributes_count);
    }
    free(methods);
}

static void free_cp(cp_info *cp, size_t count)
{
    if (cp == NULL) return;
    for (size_t i = 0; i < count; i++) {
        if (cp[i].tag == CONSTANT_Utf8)
            free(cp[i].info.utf8_info.bytes);
    }
    free(cp);
}

void free_classfile(class_file *cf)
{
    free_cp(cf->constant_pool, cf->constant_pool_count - 1);
    free(cf->interfaces);
    free_fields(cf->fields, cf->fields_count);
    free_methods(cf->methods, cf->methods_count);
    free_attr_info(cf->attributes, cf->attributes_count);
    free(cf);
}
