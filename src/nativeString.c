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

static Value UTF8LenNative(int argCount, Value* args) {
	if (argCount >= 1) {
		C_STR stringPtr = NULL;
		uint32_t length = 0;

		if (IS_STRING(args[0])) {
			ObjString* utf8_string = AS_STRING(args[0]);
			stringPtr = &utf8_string->chars[0];
			length = utf8_string->length;
		}

		if (stringPtr != NULL) {
			uint32_t char_count = 0;
			//get utf8 length
			for (uint32_t i = 0; i < length; char_count++) {
				uint8_t c = stringPtr[i];

				if ((c & 0x80) == 0) {       // 1 (0xxxxxxx)
					i += 1;
				}
				else if ((c & 0xE0) == 0xC0) { // 2 (110xxxxx)
					i += 2;
				}
				else if ((c & 0xF0) == 0xE0) { // 3 (1110xxxx)
					i += 3;
				}
				else if ((c & 0xF8) == 0xF0) { // 4 (11110xxx)
					i += 4;
				}
				else {
					return NAN_VAL;//invalid utf8
				}
			}
			return NUMBER_VAL((double)char_count);
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

static Value utf8AtNative(int argCount, Value* args) {
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

			for (uint32_t i = 0, char_count = 0, start; i < length; char_count++) {
				uint8_t c = stringPtr[i];
				start = i;

				if ((c & 0x80) == 0) {       // 1 (0xxxxxxx)
					i += 1;
				}
				else if ((c & 0xE0) == 0xC0) { // 2 (110xxxxx)
					i += 2;
				}
				else if ((c & 0xF0) == 0xE0) { // 3 (1110xxxx)
					i += 3;
				}
				else if ((c & 0xF8) == 0xF0) { // 4 (11110xxx)
					i += 4;
				}
				else {
					return NAN_VAL;//invalid utf8
				}

				if (char_count == index) {
					return OBJ_VAL(copyString(stringPtr + start, i - start, false));
				}
			}
		}
	}

	return NIL_VAL;
}



COLD_FUNCTION
void importNative_string() {
	defineNative_string("length", lengthNative);
	defineNative_string("charAt", charAtNative);
	defineNative_string("utf8Len", UTF8LenNative);
	defineNative_string("utf8At", utf8AtNative);
}