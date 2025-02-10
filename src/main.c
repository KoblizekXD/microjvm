#include "util.h"
#include <native/runtime.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// The program counter register. Must be set and modified during method execution.
uint8_t *pc = 0x0;

void print_help();
extern class_file **read_jmod(const char *java_home, const char *jmod_name);

int main(int argc, char **argv)
{
    if (argc == 2 && strcmp(argv[1], "--help") == 0) {
        print_help();
        exit(0);
    } else if (argc == 2 && strcmp(argv[1], "--version") == 0) {
        printf("MicroJVM v1.0 pre-release - https://github.com/KoblizekXD/microjvm\n\n");
        exit(0);
    }

    printf("\x1b[36;49mMicroJVM Virtual Machine v1.0\x1b[0m\n\n");

    char *home = getenv("JAVA_HOME");
    if (home == NULL && (argc == 1 || strcmp(argv[1], "--no-default-lib"))) {
        errprintf("JAVA_HOME must be set in order to correctly reinterpret Java's runtime classes.\n\tIf you wish to continue anyways, use flag --no-default-lib");
        exit(1);
    }

    if (argc != 2 || strcmp(argv[1], "--no-default-lib") != 0)
        read_jmod(getenv("JAVA_HOME"), "java.base");

    FILE* f = fopen("build/Test.class", "rb"); 

    class_file *cf = read_classfile(f);
    fclose(f);

    return entry(cf);
}

void print_help()
{
    printf("Usage: microjvm [flags...] classpath...\n\nFlags:\n");
    printf("\t--version\n\t\tPrints version related information.\n");
    printf("\t--help\n\t\tPrints this.\n");
    printf("\t--no-default-lib\n\t\tOmits lookup for default JVM installation. This can be useful if you want to use custom runtime classes.\n");
}
