#include <util.h>
#include <native/runtime.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <vm.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The program counter register. Must be set and modified during method execution.
uint8_t *pc = 0x0;

void print_help();
vm_options parse_options(int argc, char **argv);
extern size_t read_jmod(const char *java_home, const char *jmod_name, class_file ***entries);

int main(int argc, char **argv)
{
    vm_options opts = parse_options(argc, argv);

    printf("\x1b[36;49mMicroJVM Virtual Machine v1.0\x1b[0m\n\n");
    vm_t *vm = create_vm();

    char *home = getenv("JAVA_HOME");
    if (home == NULL || opts.no_default_lib) {
        errprintf("JAVA_HOME must be set in order to correctly reinterpret Java's runtime classes.\n\tIf you wish to continue anyways, use flag --no-default-lib");
        exit(1);
    }
    class_file **cf_entries = NULL;
    if (!opts.no_default_lib) {
        size_t entries = read_jmod(getenv("JAVA_HOME"), "java.base", &cf_entries);
        load_classes(vm, cf_entries, entries);
    }

    if (opts.classpath != NULL) {
        for (int i = 0; i < opts.classpath_len; i++) {
            FILE* f = fopen(opts.classpath[i], "rb");
            if (f == NULL) {
                perror("Failed to open classfile");
                continue;
            }
            class_file* cf = read_classfile(f);
            if (cf == NULL) {
                errprintf("Invalid classfile structure for %s\n", opts.classpath[i]);
                fclose(f);
            }
            load_class(vm, cf);
            fclose(f);
        }
    }

    int ret = 0;
    if (opts.classpath_len == 1)
        ret = entry(vm->cfs[vm->loaded_classes_count - 1]);
    else if (opts.main != NULL) {
        int found = 0;
        for (size_t i = 0; i < vm->loaded_classes_count; i++) {
            struct _utf8_info* utf8 = fqn_of(vm->cfs[i]);
            if (streq(opts.main, utf8->bytes, utf8->length)) {
                found = 1;
                ret = entry(vm->cfs[i]);
                break;
            }
        }
        if (found) {
            errprintf("Could not find main class %s", opts.main);
        }
    }

    destroy_vm(vm);
    free(cf_entries); 

    return ret;
}

void print_help()
{
    printf("Usage: microjvm [flags...] classpath...\n\nFlags:\n");
    printf("\t--version\n\t\tPrints version related information.\n");
    printf("\t--help\n\t\tPrints this.\n");
    printf("\t--no-default-lib\n\t\tOmits lookup for default JVM installation. This can be useful if you want to use custom runtime classes.\n");
    printf("\t--main <class>\n\t\tSets the default main class. This is unnecessary if classpath contains only 1 class.\n");
}

#define strequals(s1, s2) (strcmp(s1, s2) == 0)

vm_options parse_options(int argc, char **argv)
{
    vm_options opts;
    opts.no_default_lib = 0;
    opts.classpath = NULL;
    opts.main = NULL;

    if (argc == 1 || strequals(argv[1], "--help")) {
        print_help();
        exit(0);
    } else if (strequals(argv[1], "--version")) {
        printf("MicroJVM v1.0 pre-release - https://github.com/KoblizekXD/microjvm\n");
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        if (!starts_with(argv[i], "--")) {
            opts.classpath = argv + i;
            opts.classpath_len = argc - i;
            break;
        }
        if (strequals(argv[i], "--no-default-lib"))
            opts.no_default_lib = 1;
        else if (strequals(argv[i], "--main")) {
            opts.main = argv[i + 1];
            i++;
        }
    }
    return opts;
}
