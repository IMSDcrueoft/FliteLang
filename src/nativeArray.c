/*
* MIT License
* Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
* See LICENSE file in the root directory for full license text.
*/
#include "nativeBuiltin.h"
#include "vm.h"
#include "object.h"
//Array

static Value lengthNative(int argCount, Value* args)
{
	if (argCount >= 1 && IS_ARRAY(args[0])) {
		return NUMBER_VAL(AS_ARRAY(args[0])->length);
	}
	else {
		return NAN_VAL;
	}
}

static Value pushNative(int argCount, Value* args) {
	if (argCount >= 1 && IS_ARRAY(args[0])) {
		ObjArray* array = AS_ARRAY(args[0]);

		if (argCount >= 2) {
			uint64_t newSize = (uint64_t)array->length + argCount - 1;

			if (newSize > ARRAYLIKE_MAX) {
				fprintf(stderr, "Array size overflow\n");
				exit(1);
			}

			newSize = (newSize + 7) & ~7; //align to 8

			//check size
			if (newSize > array->capacity) {
				if (array->capacity < 64) {
					newSize = max(newSize, array->capacity * 2);
				}
				else {
					newSize = max(newSize, (array->capacity * 3) >> 1);
				}

				reserveArray(array, newSize);
			}

			//no type check,so it's faster
			for (uint32_t i = 1; i < argCount; ++i) {
					array->elements[array->length] = args[i];
					array->length++;
				}
		}

		return NUMBER_VAL(array->length);
	}
	else {
		return NAN_VAL;
	}
}

static Value popNative(int argCount, Value* args) {
	if (argCount >= 1 && IS_ARRAY(args[0])) {
		ObjArray* array = AS_ARRAY(args[0]);

		if (array->length > 0) {
			Value value = array->elements[array->length - 1];
			array->length--;
			return value;
		}
		else { //default return nil
			return NIL_VAL;
		}
	}
	else {
		return NIL_VAL;
	}
}

//resize the array,user don't care reserve ,this is not cpp vector
static Value resizeNative(int argCount, Value* args) {
	if (argCount >= 2 && IS_ARRAY(args[0]) && IS_NUMBER(args[1])) {
		ObjArray* array = AS_ARRAY(args[0]);

		uint64_t length = 0;
		double size = AS_NUMBER(args[1]);

		//no error
		if (size > 0 && size <= ARRAYLIKE_MAX) {
			length = (uint64_t)size;

			if (length > array->length) {
				reserveArray(array, length);

				//no type check,so it's faster
				while (array->length < length) {
						array->elements[array->length] = NIL_VAL;
						array->length++;
					}
			}
			else {
				array->length = length;//and do nothing
			}

			return BOOL_VAL(true);
		}
		else {
			fprintf(stderr, "Array size overflow\n");
			exit(1);
		}
	}

	return BOOL_VAL(false);
}

static Value ArrayNative(int argCount, Value* args) {
	uint32_t length = 0;

	if (argCount >= 1 && IS_NUMBER(args[0])) {
		double size = AS_NUMBER(args[0]);

		//no error
		if (size > 0 && size <= ARRAYLIKE_MAX) {
			length = (uint32_t)size;
		}
		else {
			fprintf(stderr, "Array size overflow\n");
			exit(1);
		}
	}

	ObjArray* array = newArray();
	stack_push(OBJ_VAL(array));
	reserveArray(array, length);

	while (array->length < length)
	{
		array->elements[array->length] = NIL_VAL;
		array->length++;
	}

	return OBJ_VAL(array);
}

COLD_FUNCTION
void importNative_array() {
	//array
	defineNative_array("resize", resizeNative);
	defineNative_array("length", lengthNative);
	defineNative_array("pop", popNative);
	defineNative_array("push", pushNative);

	defineNative_array("Array", ArrayNative);
}