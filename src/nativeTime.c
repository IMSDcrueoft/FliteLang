/*
* MIT License
* Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
* See LICENSE file in the root directory for full license text.
*/
#include "nativeBuiltin.h"
#include "vm.h"
#include "timer.h"

//Time
//return milli second-
static Value milliNative(int argCount, Value* args)
{
	return NUMBER_VAL((double)get_milliseconds());
}

//return second
static Value secondNative(int argCount, Value* args)
{
	return NUMBER_VAL((double)get_seconds());
}

static Value utcSecondNative(int argCount, Value* args) {
	return NUMBER_VAL((double)get_utc_seconds());
}

COLD_FUNCTION
void importNative_time() {
	defineNative_time("milli", milliNative);
	defineNative_time("second", secondNative);
	defineNative_time("utc", utcSecondNative);
}