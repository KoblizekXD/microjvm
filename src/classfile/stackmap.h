#ifndef MICROJVM_STACKMAP_H
#define MICROJVM_STACKMAP_H

// Defines structures relevant to the StackMap attribute

#include <stdint.h>
#define ITEM_Top                0
#define ITEM_Integer            1
#define ITEM_Float              2
#define ITEM_Double             3
#define ITEM_Long               4
#define ITEM_Null               5
#define ITEM_UninitializedThis  6
#define ITEM_Object             7
#define ITEM_Uninitialized      8

typedef struct {
    uint8_t tag; // ITEM_Top
} top_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Integer
} integer_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Float
} float_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Double
} double_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Long
} long_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Null
} null_variable_info;

typedef struct {
    uint8_t tag; // ITEM_UninitializedThis
} uninitialized_this_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Object
    uint16_t class_index; // Index into constant pool for class reference
} object_variable_info;

typedef struct {
    uint8_t tag; // ITEM_Uninitialized
    uint16_t offset; // Bytecode offset where this variable is created
} uninitialized_variable_info;

typedef union {
    top_variable_info top;
    integer_variable_info integer;
    float_variable_info floating;
    double_variable_info double_;
    long_variable_info long_;
    null_variable_info null_;
    uninitialized_this_variable_info uninitialized_this;
    object_variable_info object;
    uninitialized_variable_info uninitialized;
} verification_type_info;

// tags in frames have predefined values:
// 0   - 63     = SAME_FRAME
// 64  - 127    = SAME_LOCALS_1_STACK_ITEM
// 247          = SAME_LOCALS_1_STACK_ITEM_EXTENDED
// 248 - 250    = CHOP
// 251          = SAME_FRAME_EXTENDED
// 252 - 254    = APPEND
// 255          = FULL_FRAME

typedef struct {
    uint8_t tag; // SAME_FRAME
} same_frame;

typedef struct {
    uint8_t tag; // SAME_LOCALS_1_STACK_ITEM_FRAME
    verification_type_info stack;
} same_locals_1_stack_item_frame;

typedef struct {
    uint8_t tag; // SAME_LOCALS_1_STACK_ITEM_FRAME_EXTENDED
    uint16_t offset_delta; // Delta to jump to next bytecode instruction
    uint8_t stack_item_type; // Type of the single stack item
} same_locals_1_stack_item_frame_extended;

typedef struct {
    uint8_t tag; // CHOP_FRAME
    uint8_t locals_to_remove; // Number of locals removed
} chop_frame;

typedef struct {
    uint8_t tag; // SAME_FRAME_EXTENDED
    uint16_t offset_delta; // Delta to jump to next bytecode instruction
} same_frame_extended;

typedef struct {
    uint8_t tag; // APPEND_FRAME
    uint8_t num_locals; // Number of new locals added
    uint8_t num_stack_items; // Number of stack items added
    uint8_t *locals; // Array of new local types
    uint8_t *stack_items; // Array of new stack items
} append_frame;

typedef struct {
    uint8_t tag; // FULL_FRAME
    uint16_t offset_delta; // Delta to next bytecode instruction
    uint16_t num_locals; // Number of locals
    uint8_t *locals; // Array of locals
    uint16_t num_stack_items; // Number of stack items
    uint8_t *stack_items; // Array of stack item types
} full_frame;

typedef union {
    same_frame same_frame;
    same_locals_1_stack_item_frame same_locals_1_stack_item_frame;
    same_locals_1_stack_item_frame_extended same_locals_1_stack_item_frame_extended;
    chop_frame chop_frame;
    same_frame_extended same_frame_extended;
    append_frame append_frame;
    full_frame full_frame;
} stack_map_frame;

#endif // MICROJVM_STACKMAP_H
