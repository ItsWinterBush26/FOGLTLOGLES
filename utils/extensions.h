#pragma once

#include <GLES2/gl2.h>
#include <unordered_set>
#include <string>

class ESExtensions {
public:
    // Initialize the extension list (call once after OpenGL ES context is created)
    static void init() {
        const char* extStr = (const char*)glGetString(GL_EXTENSIONS);
        if (!extStr) return;

        std::string extList(extStr);
        size_t start = 0, end;

        while ((end = extList.find(' ', start)) != std::string::npos) {
            extensions.insert(extList.substr(start, end - start));
            start = end + 1;
        }
        if (start < extList.size()) {
            extensions.insert(extList.substr(start)); // Last extension
        }
    }

    // Fast extension check
    static inline bool isSupported(const char* extension) {
        return extensions.find(extension) != extensions.end();
    }

private:
    static std::unordered_set<std::string> extensions;
};

// Definition of the static variable
std::unordered_set<std::string> ESExtensions::extensions;

