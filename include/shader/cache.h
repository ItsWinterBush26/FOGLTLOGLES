#pragma once

#include "utils/env.h"
#include "utils/log.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <unordered_map>

// key is hash, value is path
inline std::unordered_map<size_t, std::string> shaderCache;

inline const std::string CACHE_DIRECTORY = getEnvironmentVar("MESA_GLSL_CACHE_DIR") + "/converted";

namespace ShaderConverter::Cache {
    inline bool isShaderInCache(size_t hashKey) {
        return shaderCache.find(hashKey) != shaderCache.end();
    }

    inline void putShaderInCache(size_t key, std::string& source) {
        if (isShaderInCache(key)) {
            LOGW("Key already in cache, skipping...");
            return;
        }

        std::string filePath = CACHE_DIRECTORY + "/" + std::to_string(key);

        std::ofstream newFile(filePath, std::fstream::out | std::fstream::trunc);
        newFile << source << std::endl;

        newFile.flush();
        newFile.close();

        shaderCache.insert({ key, filePath });
    }

    inline std::string getCachedShaderSource(size_t key) {
        if (!isShaderInCache(key)) return "";

        std::ifstream file(shaderCache.at(key));
        std::stringstream buffer;
        buffer << file.rdbuf();

        return buffer.str();
    }

    inline std::string invalidateShaderCache(size_t key) {
        // TODO
    }

    inline size_t getHash(std::string key) {
        return std::hash<std::string>{}(key);
    }

    inline void init() {
        if (!std::filesystem::is_directory(CACHE_DIRECTORY) || !std::filesystem::exists(CACHE_DIRECTORY)) {
            LOGI("Cache doesn't exist, creating...");
            std::filesystem::create_directory(CACHE_DIRECTORY);
        }

        for (const auto& entry : std::filesystem::directory_iterator { CACHE_DIRECTORY }) {
            
        }
    }
}