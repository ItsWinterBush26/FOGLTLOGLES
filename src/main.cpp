#include "main.h"
#include "gles20/main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <string>
#include <unordered_map>
#include <utility>

std::unordered_map<std::string, FunctionPtr> registeredFunctions;

void FOGLTLOGLES::registerFunction(std::string name, FunctionPtr function) {
    if (registeredFunctions.find(name) != registeredFunctions.end()) {
        LOGI("Overriding %s", name.c_str());
    } else {
        LOGI("Registering %s", name.c_str());
    }
    
    registeredFunctions[name] = function;
}

FunctionPtr FOGLTLOGLES::getFunctionAddress(std::string name) {
    if (auto it = registeredFunctions.find(name);
        it != registeredFunctions.end()) {
        return it->second;
    }

    LOGW("Function named %s not found", name.c_str());
    return nullptr;
}

void FOGLTLOGLES::init() {
    GLES20::wrapper->init();
}
