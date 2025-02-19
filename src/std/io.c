#include <stdio.h>

void println(void *instance, void **args)
{
    puts((const char*) args[0]);
}
