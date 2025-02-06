#include <stdint.h>
#include <types.h>
#include <read.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <endian.h>

#define read_32(SAVE_TO) result = fread(&temp_32, sizeof(uint32_t), 1, stream); \
                                  if (result != 1) return NULL; \
                                  SAVE_TO = be32toh(temp_32)

#define read_16(SAVE_TO) result = fread(&temp_16, sizeof(uint16_t), 1, stream); \
                                  if (result != 1) return NULL; \
                                  SAVE_TO = be16toh(temp_16)

#define NONNULL_ASSIGN(ASSIGN_TO, WHAT) if (WHAT == NULL) return NULL; \
                                                  ASSIGN_TO = WHAT;
#define read_8(SAVE_TO) if (fread(&SAVE_TO, sizeof(uint8_t), 1, stream) != 1)

cp_info *read_cp(FILE *stream, size_t entries /* - 1 */);

class_file *read_classfile(FILE *stream)
{
    if (stream == NULL) 
    {
        errno = EINVAL;
        return NULL;
    }

    uint32_t temp_32;
    uint32_t temp_16;
    size_t result = 1;

    class_file* cf = (class_file*) malloc(sizeof(class_file));

    read_32(cf->magic);
    read_16(cf->minor_version);
    read_16(cf->major_version);
    read_16(cf->constant_pool_count);
    cf->constant_pool = read_cp(stream, cf->constant_pool_count - 1);
    read_16(cf->access_flags);
    read_16(cf->this_class);
    read_16(cf->super_class);
    read_16(cf->interfaces_count);

    return cf;
}

cp_info *read_cp(FILE *stream, size_t entries /* - 1 */)
{
    cp_info *info = (cp_info*) malloc(entries * sizeof(cp_info));

    uint16_t temp_16;
    uint32_t temp_32;
    size_t result;

    for (size_t i = 0; i < entries; i++) {
        uint8_t tag;
        read_8(tag) {
            return NULL;
        }
        info[i].tag = tag;
        switch (tag) {
            case CONSTANT_Class:
                read_16(info[i].info.class_info.name_index);
                break;
            case CONSTANT_Fieldref:
            case CONSTANT_InterfaceMethodref:
            case CONSTANT_Methodref:
                read_16(info[i].info.field_method_ifmethod_ref_info.class_index);
                read_16(info[i].info.field_method_ifmethod_ref_info.name_and_type_index);
                break;
            case CONSTANT_String:
                read_16(info[i].info.string_info.string_index);
                break;
            case CONSTANT_Integer:
            case CONSTANT_Float:
                read_32(info[i].info.int_float_info.bytes);
                break;
            case CONSTANT_Long:
            case CONSTANT_Double:
                read_32(info[i].info.long_double_info.high_bytes);
                read_32(info[i].info.long_double_info.low_bytes);
                break;
            case CONSTANT_NameAndType:
                read_16(info[i].info.name_and_type_info.name_index);
                read_16(info[i].info.name_and_type_info.descriptor_index);
                break;
            case CONSTANT_Utf8:
                read_16(info[i].info.utf8_info.length);
                info[i].info.utf8_info.bytes = (uint8_t*) malloc(info[i].info.utf8_info.length * sizeof(uint8_t));
                result = fread(info[i].info.utf8_info.bytes, sizeof(uint8_t), info[i].info.utf8_info.length, stream);
                if (result != info[i].info.utf8_info.length)
                    return NULL;
                break;
            default:
                fprintf(stderr, "Invalid constant pool tag: %d\n", tag);
                return NULL;
        }
    }
    return info;
}
