#ifndef MICROJVM_ATTR_H
#define MICROJVM_ATTR_H

#include <endian.h>
#include <classfile/types.h>
#include <classfile/read.h>
#include <stdint.h>
#include <stdio.h>

// Society hates my decimal numbers!

#define ATTR_UNKNOWN                            0
#define CONSTANT_VALUE                          1
#define CODE                                    2
#define STACK_MAP_TABLE                         3
#define EXCEPTIONS                              4
#define INNER_CLASSES                           5
#define ENCLOSING_METHOD                        6
#define SYNTHETIC                               7
#define SIGNATURE                               8
#define SOURCE_FILE                             9
#define SOURCE_DEBUG_EXTENSION                  10
#define LINE_NUMBER_TABLE                       11
#define LOCAL_VARIABLE_TABLE                    12
#define LOCAL_VARIABLE_TYPE_TABLE               13
#define DEPRECATED                              14
#define RUNTIME_VISIBLE_ANNOTATIONS             15
#define RUNTIME_INVISIBLE_ANNOTATIONS           16
#define RUNTIME_VISIBLE_PARAMETER_ANNOTATIONS   17
#define RUNTIME_INVISIBLE_PARAMETER_ANNOTATIONS 18
#define RUNTIME_VISIBLE_TYPE_ANNOTATIONS        19
#define RUNTIME_INVISIBLE_TYPE_ANNOTATIONS      20
#define ANNOTATION_DEFAULT                      21
#define BOOTSTRAP_METHODS                       22
#define METHOD_PARAMETERS                       23
#define MODULE                                  24
#define MODULE_PACKAGES                         25
#define MODULE_MAIN_CLASS                       26
#define NEST_HOST                               27
#define NEST_MEMBERS                            28
#define RECORD                                  29
#define PERMITTED_SUBCLASSES                    30

int get_attr(struct _utf8_info *info);
void *read_attr_data(FILE *stream, class_file cf, attribute_info *info);

#endif // MICROJVM_ATTR_H
