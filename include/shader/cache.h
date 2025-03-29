#pragma once

#include "utils/env.h"
#include "utils/log.h"
#include "utils/fast_map.h"

#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <unordered_map>

// key is hash, value is path
inline FAST_MAP_BI(size_t, std::string) shaderCache;

inline const std::string CACHE_DIRECTORY = getEnvironmentVar("MESA_GLSL_CACHE_DIR") + "/converted";

namespace ShaderConverter::Cache {
    inline bool isShaderInCache(size_t hashKey) {
        if (!hashKey) return false;
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

    inline bool invalidateShaderCache(size_t key) {
        if (!isShaderInCache(key)) return false;

        std::string filename = shaderCache.at(key);
        shaderCache.erase(key);

        if (std::filesystem::exists(filename)) {
            std::filesystem::remove(filename);

            LOGI("Invalidated shader with key %i", key);
            return true;
        }

        return false;
    }

    inline size_t getHash(std::string& key) {
        return std::hash<std::string>{}(key);
    }

    inline void init() {
        LOGI("Indexing shader caches!");

        if (!std::filesystem::is_directory(CACHE_DIRECTORY) || !std::filesystem::exists(CACHE_DIRECTORY)) {
            LOGI("Cache doesn't exist, creating...");
            std::filesystem::create_directory(CACHE_DIRECTORY);
        }

        for (const auto& entry : std::filesystem::directory_iterator { CACHE_DIRECTORY }) {
            if (std::filesystem::is_regular_file(entry.status())) {
                try {
                    size_t realFilename = std::stoull(entry.path().filename().string());
                    shaderCache.insert({ realFilename, entry.path().string() });
                } catch (const std::exception& e) {
                    continue;
                }
            }
        }

        LOGI("Shader cache size : %i", shaderCache.size());
    }
}