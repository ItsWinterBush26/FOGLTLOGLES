#pragma once

#include <GLES2/gl2.h>
#include <atomic>
#include <stdexcept>
#include <unordered_set>
#include <string>

static inline std::atomic_bool initialized = ATOMIC_VAR_INIT(false);

class ESExtensions {
public:
    static void init() {
    	if (initialized.load()) return;

        const char* extStr = (const char*) glGetString(GL_EXTENSIONS);
        if (!extStr) return;

        std::string extList(extStr);
        size_t start = 0, end;

        while ((end = extList.find(' ', start)) != std::string::npos) {
            extensions.insert(extList.substr(start, end - start));
            start = end + 1;
        }

        if (start < extList.size()) extensions.insert(extList.substr(start)); 
        
        initialized.store(true);
    }

    static inline bool isSupported(const char* extension) {
        if (!initialized.load()) throw std::runtime_error("Extensions are not yet initialized!");
        return extensions.find(extension) != extensions.end();
    }

private:
    static std::unordered_set<std::string> extensions;
};

inline std::unordered_set<std::string> ESExtensions::extensions;