#ifndef MICROJVM_ATTR_H
#define MICROJVM_ATTR_H

#include <endian.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdint.h>
#include <stdio.h>

int is_attr(class_file cf, attribute_info attr, const char *name);
void *read_attr_data(FILE *stream, class_file cf, attribute_info *info);

#endif // MICROJVM_ATTR_H
