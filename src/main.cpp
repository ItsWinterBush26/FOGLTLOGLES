#include "main.h"
#include "es/utils.h"
#include "gles20/main.h"
#include "gles30/main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>
#include <jni.h>
#include <unordered_map>

inline std::unordered_map<std::string, FunctionPtr> registeredFunctions(256);

void FOGLTLOGLES::registerFunction(std::string name, FunctionPtr function) {
    if (registeredFunctions.find(name) != registeredFunctions.end()) {
        LOGI("Overriding %s", name.c_str());
    } else {
        LOGI("Registering %s", name.c_str());
    }
    
    registeredFunctions.insert({ name, function });
}

FunctionPtr FOGLTLOGLES::getFunctionAddress(std::string name) {
    if (name.empty()) return nullptr; // fast path
    if (auto it = registeredFunctions.find(name);
        it != registeredFunctions.end()) {
        // LOGI("Found function named %s", name.c_str());
        return it->second;
    }

    // LOGW("Function named %s not found", name.c_str());
    return nullptr;
}

void FOGLTLOGLES::init() {
    ESUtils::init();
    LOGI("FOGLTLOGLES launched on:");
    LOGI("GLES : %i.%i", ESUtils::version.first, ESUtils::version.second);

    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        LOGI("ESSL : %i", ESUtils::shadingVersion);

    if (ESUtils::version.first >= 2) GLES20::wrapper->init();
    if (ESUtils::version.first >= 3) GLES30::wrapper->init();
}

extern "C" {
    JNIEXPORT jint JNI_OnLoad(JavaVM* vm, void* reserved) {
        JNIEnv* env = NULL;
    
        if (vm->GetEnv((void**) &env, JNI_VERSION_1_6) != JNI_OK) {
            LOGE("Failed to get Java environment");
            return JNI_ERR;
        }

        LOGI("Hello from JNI!!!!!!!");

        return JNI_VERSION_1_6;
    }
}
