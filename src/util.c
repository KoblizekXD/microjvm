#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <util.h>
#include <classfile/types.h>
#include <endian.h>
#include <stdarg.h>

// #define VM_DEBUG

int streq(const char *s1, uint8_t *s2, size_t s2_len)
{
    size_t s1_len = strlen(s1);
    if (s1_len != s2_len) return 0;

    for (size_t i = 0; i < s1_len; i++) {
        if (s1[i] != s2[i]) return 0;
    }
    return 1;
}

int utf8eq(struct _utf8_info utf8str, const char *cstr)
{
    size_t cstr_len = strlen(cstr);
    if (utf8str.length != cstr_len) return 0;
    for (size_t i = 0; i < cstr_len; i++) {
        if (utf8str.bytes[i] != cstr[i]) return 0;
    }
    return 1;
}

int starts_with(const char *str, const char *prefix) 
{
    if (!str || !prefix) return 0;
    size_t prefix_len = strlen(prefix);
    return strncmp(str, prefix, prefix_len) == 0;
}

Method* get_main(ClassFile *cf)
{
    for (size_t i = 0; i < cf->method_count; i++) {
        if (strcmp("main", cf->methods[i].name) == 0 && strcmp("([Ljava/lang/String;)V", cf->methods[i].descriptor) == 0)
            return &cf->methods[i];
    } 
    return NULL;
}

void debug_fprintf(FILE *stream, const char *format, ...)
{
#ifdef VM_DEBUG
    fprintf(stream, "\x1b[90m[VMDEBUG] ");
    va_list args;
    va_start(args, format);
    vfprintf(stream, format, args);
    va_end(args);
    fprintf(stream, "\x1b[0m\n");
#endif
}

void errprintf(const char *format, ...)
{
    fprintf(stderr, "\x1b[37;41m[VMERROR]\x1b[0m ");
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    printf("\n");
}

int ends_with(const char *str, const char *suffix)
{
    if (!str || !suffix) {
        return 0;
    }
    
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    
    if (suffix_len > str_len) {
        return 0;
    }
    
    return strcmp(str + (str_len - suffix_len), suffix) == 0;
}

const char *PRIMITIVE_DESC_TYPES = "BCDFIJSZ";

size_t get_arg_count(const char *descriptor)
{
    size_t count = 0;
    size_t length = strlen(descriptor);

    for (size_t i = 0; i < length; i++) {
        char c = descriptor[i];

        if (strchr(PRIMITIVE_DESC_TYPES, c)) {
            count++;
        } else if (c == 'L') {
            i++;
            while (descriptor[i] != ';') {
                i++;
            }
            count++;
        } else if (c == '[') {
            while (descriptor[i] == '[') {
                i++;
            }
            if (descriptor[i] == 'L') {
                while (descriptor[i] != ';') {
                    i++;
                }
            }
            count++;
            i--;
        }
    }

    return count;
}

char *alloc_string(struct _utf8_info utf8)
{
    char* str = malloc(utf8.length + 1);
    if (str == NULL) return NULL;
    str[utf8.length] = '\0';
    return str;
}
