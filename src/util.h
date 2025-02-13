#ifndef MICROJVM_UTIL_H
#define MICROJVM_UTIL_H

#include <classfile/types.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

void dump(FILE *stream, class_file cf);
int streq(const char *s1, uint8_t *s2, size_t s2_len);
int utf8eq(struct _utf8_info utf8str, const char *cstr);
int starts_with(const char *str, const char *prefix);
method_info *get_main(class_file *cf);
void debug_fprintf(FILE *stream, const char *format, ...);
void errprintf(const char *format, ...);
struct _utf8_info *fqn_of(class_file *cf);

#endif
