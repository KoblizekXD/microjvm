#ifndef MICROJVM_READ_H
#define MICROJVM_READ_H

#include <stdio.h>
#include <classfile/types.h>

#define read_32(SAVE_TO) result = fread(&temp_32, sizeof(uint32_t), 1, stream); \
                                  if (result != 1) return NULL; \
                                  SAVE_TO = be32toh(temp_32)

#define read_16(SAVE_TO) result = fread(&temp_16, sizeof(uint16_t), 1, stream); \
                                  if (result != 1) return NULL; \
                                  SAVE_TO = be16toh(temp_16)

/**
 * Attempts to read a standard JVM class file object from the given stream.
 * The class file is read in big endian format, but the `class_file` structure
 * stores them in the native host endinness. The passing stream is not closed automatically.
 *
 * If the reading fails, the function will return NULL, with corresponding errno set.
 * Otherwise, a dynamically allocated pointer will be returned(don't forget to free it).
 */
ClassFile  *ReadClassFileFromStream(FILE *stream);

/**
 * Frees given class file.
 * No error will occur if operation fails.
 */
void FreeClassFile(ClassFile *cf);

/**
 * Reads n attribute entries from stream and dynamically allocates them. 
 */
attribute_info *ReadAttributes(FILE *stream, size_t count, cp_info *pool);

#endif // MICROJVM_READ_H
