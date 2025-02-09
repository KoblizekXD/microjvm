#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <util.h>
#include <classfile/types.h>
#include <endian.h>

int streq(const char *s1, uint8_t *s2, size_t s2_len)
{
    size_t s1_len = strlen(s1);
    if (s1_len != s2_len) return 0;

    for (size_t i = 0; i < s1_len; i++) {
        if (s1[i] != s2[i]) return 0;
    }
    return 1;
}

void dump(FILE *stream, class_file cf)
{
    fprintf(stream, "Java Class File version %d.%d\n", cf.major_version, cf.minor_version);
    uint8_t* data = cf.constant_pool[cf.constant_pool[cf.this_class - 1].info.class_info.name_index - 1].info.utf8_info.bytes;
    uint16_t len = cf.constant_pool[cf.constant_pool[cf.this_class - 1].info.class_info.name_index - 1].info.utf8_info.length;
    fprintf(stream, "Class %.*s\n", len, data);
    fprintf(stream, "\tPublic: %d\n", (cf.access_flags & ACC_PUBLIC) != 0);
}

method_info* get_main(class_file *cf)
{
    for (size_t i = 0; i < cf->methods_count; i++) {
        struct _utf8_info name_data = cf->constant_pool[cf->methods[i].name_index - 1].info.utf8_info;
        struct _utf8_info desc_data = cf->constant_pool[cf->methods[i].descriptor_index - 1].info.utf8_info;

        if (streq("main", name_data.bytes, name_data.length) && streq("([Ljava/lang/String;)V", desc_data.bytes, desc_data.length))
            return &cf->methods[i];
    }
    return NULL;
}
