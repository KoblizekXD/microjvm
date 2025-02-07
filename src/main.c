#include "util.h"
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE* f = fopen("build/Test.class", "rb"); 

    class_file *cf = read_classfile(f);

    dump(stdout, *cf);
    free_classfile(cf);
    fclose(f);
    return 0;
}
