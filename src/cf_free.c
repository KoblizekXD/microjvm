#include <types.h>
#include <read.h>
#include <stdlib.h>

#define free_if_not_null(OBJECT) if (OBJECT != NULL) free(OBJECT);

void free_classfile(class_file *cf)
{
    free(cf);
}
