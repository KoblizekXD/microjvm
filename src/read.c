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

    return cf;
}

cp_info *read_cp(FILE *stream, size_t entries /* - 1 */)
{

}
