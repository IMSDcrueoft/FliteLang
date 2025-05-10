/*
* MIT License
* Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
* See LICENSE file in the root directory for full license text.
*/
#include "nativeBuiltin.h"
#include "vm.h"
#include "object.h"
#include "gc.h"
//System

//force do gc
static Value gcNative(int argCount, Value* args) {
	garbageCollect();
	return NIL_VAL;
}

static Value totalBytesNative(int argCount, Value* args) {
	return NUMBER_VAL((double)(vm.bytesAllocated_no_gc + vm.bytesAllocated));
}

//Print all the parameters
static Value logNative(int argCount, Value* args) {
	for (int i = 0; i < argCount;) {
		printValue_sys(args[i]);

		if (++i < argCount) {
			printf(" ");
		}
		else {
			printf("\n");
		}
	}
	//no '\n'
	return NIL_VAL;
}

#undef KiB16
#undef GiB1

COLD_FUNCTION
void importNative_system() {
	defineNative_system("gc", gcNative);
	defineNative_system("total", totalBytesNative);
	defineNative_system("log", logNative);
}