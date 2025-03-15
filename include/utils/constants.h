#pragma once

#include <unordered_set>

namespace FOGLTLOGLES {
    const inline std::unordered_set<std::string> REQUIRED_EXTENSIONS = {
        "GL_EXT_blend_func_extended"
    }; // TODO: Add checks in ::init() and warn if not present

} /* namespace FOGLTLOGLES */