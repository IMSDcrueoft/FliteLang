/*
* MIT License
* Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
* See LICENSE file in the root directory for full license text.
*/
#include "nativeBuiltin.h"
#include "vm.h"

//const string limit,will truncate
#define INTERN_STRING_WARN (1024)

//String
static Value lengthNative(int argCount, Value* args)
{
	if (argCount >= 1) {
		if (IS_STRING(args[0])) {
			return NUMBER_VAL(AS_STRING(args[0])->length);
		}
	}
	return NAN_VAL;
}

static Value charAtNative(int argCount, Value* args) {
	if (argCount >= 2 && IS_NUMBER(args[1])) {
		C_STR stringPtr = NULL;
		uint32_t length = 0;

		if (IS_STRING(args[0])) {
			ObjString* string = AS_STRING(args[0]);
			stringPtr = &string->chars[0];
			length = string->length;
		}

		if (stringPtr != NULL) {
			double indexf = AS_NUMBER(args[1]);
			if (indexf < 0 || indexf >= length) return NIL_VAL;//out of range
			uint32_t index = (uint32_t)indexf;

			return OBJ_VAL(copyString(stringPtr + index, 1, false));
		}
	}

	return NIL_VAL;
}

COLD_FUNCTION
void importNative_string() {
	defineNative_string("length", lengthNative);
	defineNative_string("charAt", charAtNative);
}