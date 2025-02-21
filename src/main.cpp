#include "main.h"
#include "egl/egl.h"
#include "gles20/main.h"
#include "gles30/main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <string>
#include <unordered_map>
#include <utility>

static std::unordered_map<std::string, FunctionPtr> registeredFunctions;

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
    const GLubyte* glesVersion = glGetString(GL_VERSION);
    LOGI("FOGLTLOGLES launched on %s", glesVersion);

    GLES20::wrapper->init();
    GLES30::wrapper->init();

    LOGI("Extensions: %s", reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
}

__attribute__((constructor(1000)))
static void inline init() {
    eglInit();
}