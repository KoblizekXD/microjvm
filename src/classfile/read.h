#ifndef MICROJVM_READ_H
#define MICROJVM_READ_H

#include <stdio.h>
#include <classfile/types.h>

/**
 * Attempts to read a standard JVM class file object from the given stream.
 * The class file is read in big endian format, but the `class_file` structure
 * stores them in the native host endinness. The passing stream is not closed automatically.
 *
 * If the reading fails, the function will return NULL, with corresponding errno set.
 * Otherwise, a dynamically allocated pointer will be returned(don't forget to free it).
 */
class_file *read_classfile(FILE *stream);

/**
 * Frees given class file.
 * No error will occur if operation fails.
 */
void free_classfile(class_file *cf);

#endif // MICROJVM_READ_H
