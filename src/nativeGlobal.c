/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#include "nativeBuiltin.h"
#include "vm.h"
#include <time.h>

//place your function here

//return second
static Value clockNative(int argCount, Value* args)
{
	return NUMBER_VAL((double)clock() / CLOCKS_PER_SEC);
}

COLD_FUNCTION
void importNative_global()
{
	//nothing here yet.
	defineNative_global("clock", clockNative);
}
