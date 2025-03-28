#pragma once

#include "xxhash.h"

#include <cstdint>

template<typename K>
class XXHasher {
public:
    size_t operator()(const K& key) const {
#if INTPTR_MAX == INT32_MAX
        return XXH32(&key, sizeof(K), 0);
#else
        return XXH64(&key, sizeof(K), 0);
#endif
    }
};

// Keys should be builtins only!
#define XXHASH_MAP_BI(key, value) std::unordered_map<key, value, XXHasher<key>>