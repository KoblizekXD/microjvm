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

int main(int argc, char **argv)
{
    vm_options opts = parse_options(argc, argv);

    printf("\x1b[36;49mMicroJVM Virtual Machine v1.0\x1b[0m\n\n");
    vm_t *vm = create_vm(&opts);

    char *home = getenv("JAVA_HOME");
    if (home == NULL && opts.no_default_lib == 0) {
        errprintf("JAVA_HOME must be set in order to correctly reinterpret Java's runtime classes.\n\tIf you wish to continue anyways, use flag --no-default-lib");
        exit(1);
    } else if (home != NULL && opts.no_default_lib == 0) {
        opts.classpath_len++;
        opts.classpath = realloc(opts.classpath, opts.classpath_len * sizeof(char*));
        char *base_jmod = malloc(sizeof(char) * (strlen(home) + 7 + 9 + 6));
        sprintf(base_jmod, "%s/jmods/java.base.jmod", home);
        opts.classpath[opts.classpath_len - 1] = base_jmod;
    }

    int ret = 0; 
    if ((opts.no_default_lib == 1 && opts.classpath_len == 1) || (opts.no_default_lib == 0 && opts.classpath_len - 1 == 1)) {
        ret = entry(vm, LoadClassFromFile(vm, opts.classpath[0], 1));
    } else if (opts.main != NULL) {
        ClassFile* cf = LoadClass(vm, opts.main, 1);
        if (cf == NULL) {
            errprintf("Could not find main class %s", opts.main);
        } else {
            ret = entry(vm, cf);
        }
    } else {
        errprintf("Please provide a valid entrypoint of the JVM using the --main flag!");
    }

    for (size_t i = 0; i < opts.classpath_len; i++) {
        free(opts.classpath[i]);
    }
    free(opts.classpath);
    destroy_vm(vm);

    return ret;
}

void print_help()
{
    printf("Usage: microjvm [flags...] classpath...\n\nFlags:\n");
    printf("\t--version\n\t\tPrints version related information.\n");
    printf("\t--help\n\t\tPrints this.\n");
    printf("\t--no-default-lib\n\t\tOmits lookup for default JVM installation. This can be useful if you want to use custom runtime classes. Requires JAVA_HOME variable set.\n");
    printf("\t--main <class>\n\t\tSets the default main class. This is unnecessary if classpath contains only 1 class(JVM doesn't count).\n");
    printf("\t--max-heap <size>\n\t\tSets the maximum size of the heap memory of the virtual machine in bytes. Attempting to exceed this value will result in crash. Defaults to 4096.\n");
    printf("\t--min-heap <size>\n\t\tSets the minimum size of the heap memory of the virtual machine in bytes. This value will be preallocated. Default to 1024(1KiB).\n");
}

#define strequals(s1, s2) (strcmp(s1, s2) == 0)

vm_options parse_options(int argc, char **argv)
{
    vm_options opts;
    opts.no_default_lib = 0;
    opts.classpath = NULL;
    opts.main = NULL;
    opts.heap_min = 1024;
    opts.heap_max = 4096;

    if (argc == 1 || strequals(argv[1], "--help")) {
        print_help();
        exit(0);
    } else if (strequals(argv[1], "--version")) {
        printf("MicroJVM v1.0 pre-release - https://github.com/KoblizekXD/microjvm\n");
        exit(0);
    }
    for (int i = 1; i < argc; i++) {
        if (!starts_with(argv[i], "--")) {
            opts.classpath_len = argc - i;
            opts.classpath = (char**) malloc(sizeof(char*) * opts.classpath_len);
            for (size_t j = i; j < i + opts.classpath_len; j++) {
                char* new = (char*) malloc(strlen(argv[j]) + 1);
                strcpy(new, argv[j]);
                opts.classpath[j - i] = new;
            }
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
