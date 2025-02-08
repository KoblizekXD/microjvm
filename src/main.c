#include "util.h"
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdio.h>

int main(int argc, char **argv)
{
    FILE* f = fopen("build/Test.class", "rb"); 

    class_file *cf = read_classfile(f);
    fclose(f);

    dump(stdout, *cf);
    free_classfile(cf); 
    return 0;
}
