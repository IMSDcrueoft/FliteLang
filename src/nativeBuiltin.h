/*
* MIT License
* Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
* See LICENSE file in the root directory for full license text.
*/
#pragma once
#include "common.h"
#include "object.h"
#include "table.h"

typedef enum {
	MODULE_ARRAY,
	MODULE_STRING,
	MODULE_SYSTEM,

	BUILTIN_MODULE_COUNT,//as the size too
} BuiltinMouduleType;

void importNative_array();
void importNative_string();
void importNative_system();
void importNative_global();