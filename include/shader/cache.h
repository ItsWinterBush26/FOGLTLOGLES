#pragma once

#include <string>
#include <sys/types.h>
#include <unordered_map>
#include <shaderc/shaderc.hpp>

class ShaderCache {
public:
    ShaderCache(uint max_mem_cache_size = 10)
     : cachedShaders(max_mem_cache_size) {
    }

    void compileShaders(
        std::string name
        shaderc_shader_kind kind,
        std::string source
    ) {
        // if (auto cachedShader = cachedShaders.find(name); findit != myMap.end())
    }

private:
    shaderc::Compiler spirvCompiler;

    std::unordered_map<std::string, std::string> cachedShaders;
};