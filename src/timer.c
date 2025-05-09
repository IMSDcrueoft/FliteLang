/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#include "timer.h"
#include <time.h>

int64_t get_milliseconds() {
    return (double)clock() / CLOCKS_PER_SEC * 1000;
}