#pragma once

#include <cstdint>

template<typename K>
class FastHasher {
public:
    size_t operator()(const K& key) const {
        return key; // LLVM method 💀
        // i dont even think we'll have collisions
        // since we return the key and its always unique
    }
};

// Keys should be builtins only!
#define FAST_MAP_BI(key, value) std::unordered_map<key, value, FastHasher<key>>
