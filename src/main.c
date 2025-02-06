#include <types.h>
#include <read.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    FILE* f = fopen("build/Test.class", "rb"); 

    class_file *cf = read_classfile(f);

    printf("%d %d\n", cf->major_version, cf->minor_version); 

    free(cf);
    fclose(f);
    return 0;
}
