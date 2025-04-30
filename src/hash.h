/*
 * MIT License
 * Copyright (c) 2025 IMSDcrueoft (https://github.com/IMSDcrueoft)
 * See LICENSE file in the root directory for full license text.
*/
#pragma once
#include <stdint.h>
#define FNV1A64_OFFSET_BASIS ((uint64_t)0xcbf29ce484222325ULL)
#define FNV1A64_PRIME         ((uint64_t)0x100000001b3ULL)

static inline uint64_t fnv1a64_hash(const void* data, size_t length) {
    const uint8_t* p = (const uint8_t*)data;
    uint64_t hash = FNV1A64_OFFSET_BASIS;

    while (length--) {
        hash ^= (uint64_t)*p++;
        hash *= FNV1A64_PRIME;
    }

    return hash;
}
#undef FNV1A64_OFFSET_BASIS
#undef FNV1A64_PRIME

#define HASH_64bits(str,len) fnv1a64_hash(str, len)