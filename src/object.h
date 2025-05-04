/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#pragma once
#include "common.h"
#include "value.h"
#include "table.h"
#include "chunk.h"

typedef enum {
	//objects that don't gc
	OBJ_STRING,
	OBJ_NATIVE,
	OBJ_FUNCTION,

	//objects gc able
	OBJ_UPVALUE,
	OBJ_CLOSURE,
	OBJ_BOUND_METHOD,
	OBJ_CLASS,
	OBJ_INSTANCE,
	//array
	OBJ_ARRAY
} ObjType;

//@object.type()
typedef enum {
	TYPE_STRING_BOOL,
	TYPE_STRING_NIL,
	TYPE_STRING_NUMBER,
	TYPE_STRING_STRING,
	TYPE_STRING_FUNCTION,
	TYPE_STRING_NATIVE,
	TYPE_STRING_CLASS,
	TYPE_STRING_OBJECT,
	TYPE_STRING_ARRAY,

	TYPE_STRING_COUNT,
} TypeStringType;

#if DEBUG_LOG_GC
extern const C_STR objTypeInfo[];
#endif

struct Obj {
	uint8_t type;
	uint8_t isMarked;
	struct Obj* next;
};
#define OBJ_PTR_SET_NEXT(obj,nextPtr)	(obj->next = nextPtr)
#define OBJ_PTR_GET_NEXT(obj)			(obj->next)

static inline Obj stateLess_obj_header(ObjType objType) {
	return (Obj) { .next = NULL, .isMarked = 1, .type = objType };
}

typedef struct {
	Obj obj;
	uint16_t arity;
	uint16_t upvalueCount;
	uint32_t id;
	Chunk chunk;
	ObjString* name;
} ObjFunction;

typedef struct ObjUpvalue {
	Obj obj;
	Value closed; //closed value
	Value* location;
	struct ObjUpvalue* next;
} ObjUpvalue;

typedef struct {
	Obj obj;
	uint32_t upvalueCount;
	ObjUpvalue** upvalues;
	ObjFunction* function;
} ObjClosure;

typedef struct {
	Obj obj;
	Value receiver;
	ObjClosure* method;
} ObjBoundMethod;

//argCount and argValues
typedef Value(*NativeFn)(int argCount, Value* args);

typedef struct {
	Obj obj;
	NativeFn function;
} ObjNative;

typedef struct {
	Obj obj;
	ObjString* name;
	Value initializer;//inline cache
	Table methods;
} ObjClass;

typedef struct {
	Obj obj;
	ObjClass* klass;
	Table fields;
} ObjInstance;

#define INVALID_OBJ_STRING_SYMBOL UINT32_MAX
struct ObjString {
	Obj obj;
	uint32_t symbol; // used to boost global hash table
	uint32_t length; // the real length,not include '\0'
	uint64_t hash; // the hash
	char chars[]; // flexible array members FAM
};

//begin at 8 and align to 8, when < 64,mul 2, then *1.5 and align 8
//ARRAYLIKE_MAX + 7 & ~7 => ARRAYLIKE_MAX
#define ARRAYLIKE_MAX 0xfffffff8
typedef struct {
	Obj obj;
	uint32_t length;
	uint32_t capacity;
	Value* elements;
} ObjArray;

#define OBJ_GET_TYPE(obj)			((obj).type)
#define OBJ_SET_TYPE(obj,objType)	((obj).type = objType)
#define OBJ_PTR_GET_TYPE(obj)		((obj)->type)

#define OBJ_TYPE(value)				OBJ_PTR_GET_TYPE(AS_OBJ(value))
#define IS_CLOSURE(value)			isObjType(value, OBJ_CLOSURE)
#define IS_FUNCTION(value)			isObjType(value, OBJ_FUNCTION)
#define IS_NATIVE(value)			isObjType(value, OBJ_NATIVE)
#define IS_BOUND_METHOD(value)		isObjType(value, OBJ_BOUND_METHOD)
#define IS_CLASS(value)				isObjType(value, OBJ_CLASS)
#define IS_INSTANCE(value)			isObjType(value, OBJ_INSTANCE)
#define IS_STRING(value)			isObjType(value, OBJ_STRING)
#define IS_ARRAY(value)				isObjType(value, OBJ_ARRAY)

#define OBJ_IS_TYPE(array, arrayType)		(OBJ_GET_TYPE(array->obj) == arrayType)
#define ARRAY_IN_RANGE(array, index)		((index >= 0) && (index < array->length))

#define AS_CLOSURE(value)			((ObjClosure*)AS_OBJ(value))
#define AS_FUNCTION(value)			((ObjFunction*)AS_OBJ(value))
#define AS_BOUND_METHOD(value)		((ObjBoundMethod*)AS_OBJ(value))
#define AS_CLASS(value)				((ObjClass*)AS_OBJ(value))
#define AS_INSTANCE(value)			((ObjInstance*)AS_OBJ(value))
#define AS_NATIVE(value)			(((ObjNative*)AS_OBJ(value))->function)
#define AS_STRING(value)			((ObjString*)AS_OBJ(value))
#define AS_ARRAY(value)				((ObjArray*)AS_OBJ(value))

static inline bool isObjType(Value value, ObjType type) {
	return IS_OBJ(value) && AS_OBJ(value)->type == type;
}

ObjString* copyString(C_STR chars, uint32_t length, bool escapeChars);
ObjString* connectString(ObjString* strA, ObjString* strB);

void printObject(Value value, bool isExpand);

Entry* getStringEntryInPool(ObjString* string);
NumberEntry* getNumberEntryInPool(Value* value);

ObjUpvalue* newUpvalue(Value* slot);
ObjFunction* newFunction();
ObjClosure* newClosure(ObjFunction* function);
ObjBoundMethod* newBoundMethod(Value receiver, ObjClosure* method);
ObjNative* newNative(NativeFn function);
ObjClass* newClass(ObjString* name);
ObjInstance* newInstance(ObjClass* klass);
ObjArray* newArray();

void reserveArray(ObjArray* array, uint64_t size);