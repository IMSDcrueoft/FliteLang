/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#pragma once
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdarg.h>
#include <float.h>

#include "options.h"
#include "optimize.h"

typedef char* STR;
typedef const char* C_STR;

#define UINT8_COUNT 0x100
#define UINT16_COUNT 0x10000

#define mem_alloc malloc
#define mem_realloc realloc
#define mem_free free