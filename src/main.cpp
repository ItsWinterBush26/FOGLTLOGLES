#include "main.h"
#include "gles20/main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <string>
#include <unordered_map>
#include <utility>

std::unordered_map<std::string, FunctionPtr> registeredFunctions;

void registerFunction(std::string name, FunctionPtr function) {
    if (registeredFunctions.find(name) != registeredFunctions.end()) {
        LOGI("Overriding %s", name.c_str());
    } else {
        LOGI("Registering %s", name.c_str());
    }
    
    registeredFunctions[name] = function;
}

FunctionPtr getFunctionAddress(std::string name) {
    if (auto it = registeredFunctions.find(name);
        it != registeredFunctions.end()) {
        // LOGI("Function found named %s", name.c_str());
        return it->second;
    }

    LOGW("Function named %s not found", name.c_str());
    return nullptr;
}

void init() {
    LOGI("hi");
    // ESExtensions::init();//ohmd
    LOGI("%s", (const char*) glGetString(GL_EXTENSIONS));
    GLES20::wrapper->init();
}
