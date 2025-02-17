#include <egl/egl.h>
#include "main.h"
#include "utils/defines.h"
#include "utils/log.h"

#include <atomic>
#include <mutex>
#include <string>

std::atomic_bool eglFunctionsInit = ATOMIC_VAR_INIT(false);
std::once_flag eglInitFlag;

constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

OVERRIDE(
    EGLContext,
    eglCreateContext,
    (EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)) {
    /* EGLContext ctx = */
    LOGI("This where we init right?");
    return original_eglCreateContext(dpy, config, share_context, attrib_list);
    // return ctx;
}

OVERRIDE(__eglMustCastToProperFunctionPointerType,
    eglGetProcAddress,
    (const char *procname)
) {
    LOGI("What could LWJGL be asking bruh??? procname: %s", procname);
    switch (str2int(procname)) {
        case str2int("eglCreateContext"):
            LOGI("Returning overridden eglCreateContext");
            return reinterpret_cast<__eglMustCastToProperFunctionPointerType>(eglCreateContext);
        default: return original_eglGetProcAddress(procname);
    }

    // return getGlFunctionAddress(std::string(procname));
}