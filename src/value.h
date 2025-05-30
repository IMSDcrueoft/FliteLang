/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#pragma once
#include "common.h"

typedef enum {
	VAL_BOOL,
	VAL_NIL, //nil should not be zero
	VAL_NUMBER,
	VAL_OBJ,
} ValueType;

typedef struct Obj Obj;
typedef struct ObjString ObjString;

//the dynamic value
typedef struct {
	ValueType type;
	union {
		bool boolean;
		double number;

		Obj* obj;

		//convert to binary
		uint64_t binary;
	} as;
} Value;

typedef struct {
	uint32_t capacity; //limit to 4G
	uint32_t count;    //limit to 4G
	Value* values;
} ValueArray;

#define SAME_VALUE_TYPE(a,b)	((a).type == (b).type)

#define IS_BOOL(value)    ((value).type == VAL_BOOL)
#define IS_NIL(value)     ((value).type == VAL_NIL)
#define NOT_NIL(value)	  ((value).type != VAL_NIL)
#define IS_NUMBER(value)  ((value).type == VAL_NUMBER)
#define IS_OBJ(value)     ((value).type == VAL_OBJ)

#define AS_BOOL(value)    ((value).as.boolean)
#define AS_NUMBER(value)  ((value).as.number)
#define AS_OBJ(value)     ((value).as.obj)
#define AS_BINARY(value)  ((value).as.binary)

#define BOOL_VAL(value)   ((Value){VAL_BOOL, {.boolean = value}})
#define NIL_VAL           ((Value){VAL_NIL, {.number = 0}})
#define NAN_VAL           ((Value){VAL_NUMBER, {.number = NAN}}) //use this to return a NaN
#define NUMBER_VAL(value) ((Value){VAL_NUMBER, {.number = value}})
#define OBJ_VAL(object)   ((Value){VAL_OBJ, {.obj = (Obj*)object}})

bool valuesEqual(Value a, Value b);

void printValue(Value value);
void printValue_sys(Value value);

void valueArray_init(ValueArray* array);
void valueArray_write(ValueArray* array, Value value);
void valueArray_writeAt(ValueArray* array, Value value, uint32_t index);
void valueArray_free(ValueArray* array);