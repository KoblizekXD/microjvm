#include <util.h>
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
        switch (attr[i].synth_attr_type) {
            case CODE:
                free(attr[i].data.code_attribute.code);
                free(attr[i].data.code_attribute.exception_table);
                free_attr_info(attr[i].data.code_attribute.attributes, attr[i].data.code_attribute.attributes_count); 
                break;
            case NEST_MEMBERS:
                free(attr[i].data.nest_members.classes);
                break;
            case PERMITTED_SUBCLASSES:
                free(attr[i].data.permitted_subclasses.classes);
                break;
            case BOOTSTRAP_METHODS:
                for (size_t j = 0; j < attr[i].data.bootstrap_methods.num_bootstrap_methods; j++) {
                    free(attr[i].data.bootstrap_methods.bootstrap_methods[j].bootstrap_arguments);
                }
                free(attr[i].data.bootstrap_methods.bootstrap_methods);
                break;
            default:
                debug_fprintf(stderr, "Unsure what to free because of unknown tag %d", attr[i].synth_attr_type);
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

static void free_cp(cp_info *restrict cp, size_t count)
{
    if (!cp || count == 0) return;
    for (size_t i = 0; i < count; i++) {
        cp_info *entry = &cp[i];
        if (entry->tag == CONSTANT_Utf8)
            free(entry->info.utf8_info.bytes);
    }
    free(cp);
}

void free_classfile(class_file *cf)
{
    if (cf == NULL) {
        errprintf("Failed to free classfile, because passing classfile was NULL!");
        return;
    }
    free_cp(cf->constant_pool, cf->constant_pool_count - 1);
    free(cf->interfaces);
    free_fields(cf->fields, cf->fields_count);
    free_methods(cf->methods, cf->methods_count);
    free_attr_info(cf->attributes, cf->attributes_count);
    free(cf);
}
