#ifndef MICROJVM_UTIL_H
#define MICROJVM_UTIL_H

#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void dump(FILE *stream, class_file cf);
int streq(const char *s1, uint8_t *s2, size_t s2_len);
method_info *get_main(class_file *cf);

#endif
