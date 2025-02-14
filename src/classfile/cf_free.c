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

void FreeClassFile(ClassFile *cf)
{
    free_cp(cf->constant_pool, cf->contant_pool_size);
    free(cf->name);
    if (cf->has_superclass)
        free(cf->super_name);
    for (size_t i = 0; i < cf->interface_count; i++) {
        free(cf->interfaces[i]);
    }
    free(cf->interfaces);
    for (size_t i = 0; i < cf->field_count; i++) {
        free(cf->fields[i].name);
        free(cf->fields[i].descriptor);
        free(cf->fields[i].value);
        free(cf->fields[i].attributes);
    }
    free(cf->fields);
    for (size_t i = 0; i < cf->method_count; i++) {
        free(cf->methods[i].name);
        free(cf->methods[i].descriptor);
        free_attr_info(cf->methods[i].attributes, cf->methods[i].attribute_count);
    }
    free(cf->methods);
    free_attr_info(cf->attributes, cf->attribute_count);
    free(cf);
}
