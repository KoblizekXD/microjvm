#include <stddef.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdlib.h>

#define free_if_not_null(OBJECT) if (OBJECT != NULL) free(OBJECT);

static void free_attr_info(attribute_info *attr, size_t count)
{
    if (attr == NULL) return;
    for (size_t i = 0; i < count; i++) {
        free_if_not_null(attr[i].info);
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
    free_cp(cf->constant_pool, cf->constant_pool_count);
    free(cf->interfaces);
    free_fields(cf->fields, cf->fields_count);
    // TODO: free field_info, method_info
    free_attr_info(cf->attributes, cf->attributes_count);
    free(cf);
}
