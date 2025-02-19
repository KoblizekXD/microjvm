#include <archive.h>
#include <archive_entry.h>
#include <classfile/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vm.h>
#include <classfile/read.h>
#include <util.h>

extern void bytecode_exec(vm_t *vm, ClassFile *cf, Method *method);

void get_fqn(const char *path, char *output, size_t size)
{
    if (strncmp(path, "./", 2) == 0 || strncmp(path, ".\\", 2) == 0) {
        path += 2;
    }

    strncpy(output, path, size - 1);
    output[size - 1] = '\0';

    char *ext = strrchr(output, '.');
    if (ext && strcmp(ext, ".class") == 0) {
        *ext = '\0';
    }

    for (char *c = output; *c; ++c) {
        if (*c == '/' || *c == '\\') {
            *c = '.';
        }
    }
}

Method* _find_clinit(ClassFile* cf)
{
    for (size_t i = 0; i < cf->method_count; i++) {
        if (strcmp(cf->methods[i].name, "<clinit>") == 0)
            return &cf->methods[i];
    }
    return NULL;
}

ClassFile* LoadClassFromFile(vm_t *vm, const char *path, int initialize)
{
    FILE* f = fopen(path, "rb");
    if (f == NULL) return NULL;
    ClassFile* cf = ReadClassFileFromStream(f);
    fclose(f);
    if (cf == NULL) return NULL;
    if (initialize) {
        Method *clinit = _find_clinit(cf);
        if (clinit) bytecode_exec(vm, cf, clinit);
    }
    vm->loaded_classes_count++;
    vm->cfs = realloc(vm->cfs, vm->loaded_classes_count * sizeof(ClassFile*));
    vm->cfs[vm->loaded_classes_count - 1] = cf;
    return cf;
}

ClassFile* LoadClassUtf8(vm_t *vm, struct _utf8_info utf8_name, int initialize)
{
    char* data = (char*) malloc(utf8_name.length + 1);
    memcpy(data, utf8_name.bytes, utf8_name.length);
    data[utf8_name.length] = '\0';
    ClassFile* cf = LoadClass(vm, data, initialize);
    free(data);
    return cf;
}

ClassFile* LoadClass(vm_t *vm, const char *name, int initialize)
{
    for (size_t i = 0; i < vm->loaded_classes_count; i++) {
        if (strcmp(name, vm->cfs[i]->name) == 0) // Assume class has already been initialized
            return vm->cfs[i];
    }

    char **classpath = vm->opts->classpath;
    for (size_t i = 0; i < vm->opts->classpath_len; i++) {
        if (ends_with(classpath[i], ".class")) {
            FILE *stream = fopen(classpath[i], "rb");
            ClassFile* cf = ReadClassFileFromStream(stream);
            fclose(stream);
            if (strcmp(cf->name, name) == 0) {
                if (initialize) {
                    Method *clinit = _find_clinit(cf);
                    if (clinit) bytecode_exec(vm, cf, clinit);
                }
                vm->loaded_classes_count++;
                vm->cfs = realloc(vm->cfs, vm->loaded_classes_count * sizeof(ClassFile*));
                vm->cfs[vm->loaded_classes_count - 1] = cf;
                return cf;
            } else FreeClassFile(cf);
        } else if (ends_with(classpath[i], ".jmod")) {
            char *together = malloc(16 + strlen(name));
            if (together == NULL) continue;
            struct archive *a = archive_read_new();
            struct archive_entry *entry;
            archive_read_support_format_all(a);
            archive_read_support_filter_all(a);
            sprintf(together, "classes/%s.class", name);
            archive_read_open_filename(a, classpath[i], 10240);
            while (archive_read_next_header(a, &entry) == ARCHIVE_OK) {
                const char *pathname = archive_entry_pathname(entry);
                if (strcmp(pathname, together) == 0) {
                    size_t size = archive_entry_size(entry);
                    uint8_t *data = malloc(size);
                    archive_read_data(a, data, size);
                    FILE* stream = fmemopen(data, size, "rb");

                    if (stream != NULL) {
                        ClassFile* cf = ReadClassFileFromStream(stream);
                        archive_read_free(a);
                        fclose(stream);
                        free(data);
                        vm->loaded_classes_count++;
                        vm->cfs = realloc(vm->cfs, vm->loaded_classes_count * sizeof(ClassFile*));
                        vm->cfs[vm->loaded_classes_count - 1] = cf;
                        if (initialize) {
                            Method *clinit = _find_clinit(cf);
                            if (clinit) bytecode_exec(vm, cf, clinit);
                        } 
                        free(together);
                        return cf;
                    } else {
                        errprintf("Failed to open memory handle");
                        free(data);
                        archive_read_free(a);
                        free(together);
                        return NULL;
                    }
                } else {
                    archive_read_data_skip(a);
                }
            }
            archive_read_free(a);
            free(together);
        } else if (ends_with(classpath[i], ".jar")) {
            errprintf("Reading JAR files is not yet supported.");
        }
    }

    return NULL;
}
