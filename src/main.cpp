#include "main.h"
#include "es/utils.h"
#include "gles20/main.h"
#include "gles30/main.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <unordered_map>
#include <utility>

static std::unordered_map<str, FunctionPtr> registeredFunctions(256);

void FOGLTLOGLES::registerFunction(str name, FunctionPtr function) {
    if (registeredFunctions.find(name) != registeredFunctions.end()) {
        LOGI("Overriding %s", name);
    } else {
        LOGI("Registering %s", name);
    }
    
    registeredFunctions.insert_or_assign(name, function);
}

FunctionPtr FOGLTLOGLES::getFunctionAddress(str name) {
    if (auto it = registeredFunctions.find(name);
        it != registeredFunctions.end()) {
        return it->second;
    }

    LOGW("Function named %s not found", name);
    return nullptr;
}

void FOGLTLOGLES::init() {
    ESUtils::init();
    LOGI("FOGLTLOGLES launched on ES %i.%i", ESUtils::version.first, ESUtils::version.second);

    GLES20::wrapper->init();
    GLES30::wrapper->init();
}
