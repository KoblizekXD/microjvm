#include "native/runtime.h"
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdint.h>
#include <stdio.h>

// The program counter register. Must be set and modified during method execution.
uint8_t *pc = 0x0;

int main(int argc, char **argv)
{
    printf("MicroJVM Virtual Machine v1.0\n\n");
    FILE* f = fopen("build/Test.class", "rb"); 

    class_file *cf = read_classfile(f);
    fclose(f);

    return entry(cf);
}
