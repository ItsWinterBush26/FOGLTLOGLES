#pragma once

#include <string>

namespace FOGLTLOGLES {
    static inline std::string getEnvironmentVar(std::string const& key) {
        char* val = getenv( key.c_str() );
        return val == NULL ? std::string("") : std::string(val);
    }
}