#include <stdio.h>
#include <util.h>
#include <classfile/types.h>
#include <endian.h>

void dump(FILE *stream, class_file cf)
{
    fprintf(stream, "Java Class File version %d.%d\n", cf.major_version, cf.minor_version);
    uint8_t* data = cf.constant_pool[cf.constant_pool[cf.this_class - 1].info.class_info.name_index - 1].info.utf8_info.bytes;
    uint16_t len = cf.constant_pool[cf.constant_pool[cf.this_class - 1].info.class_info.name_index - 1].info.utf8_info.length;
    fprintf(stream, "Class %.*s\n", len, data);
    fprintf(stream, "\tPublic: %d\n", (cf.access_flags & ACC_PUBLIC) != 0);
}
