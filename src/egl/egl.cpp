#include <egl/egl.h>
#include "main.h"
#include "utils/defines.h"
#include "utils/log.h"

#include <atomic>
#include <mutex>

std::atomic_bool eglFunctionsInit = ATOMIC_VAR_INIT(false);
std::once_flag eglInitFlag;

constexpr unsigned int str2int(const char* str, int h = 0) {
    return !str[h] ? 5381 : (str2int(str, h+1) * 33) ^ str[h];
}

__eglMustCastToProperFunctionPointerType OV_eglGetProcAddress(const char* pn);

void eglInit() {
    LOGI("Initializing EGL overrides");

    registerFunction("eglCreateContext", TO_FUNCTIONPTR(eglCreateContext));
    registerFunction("eglGetProcAddress", TO_FUNCTIONPTR(OV_eglGetProcAddress));
}

OVERRIDE(
    EGLContext,
    eglCreateContext,
    (EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)) {
    /* EGLContext ctx = */
    LOGI("This where we init right?");
    init();
    return original_eglCreateContext(dpy, config, share_context, attrib_list);
    // return ctx;
}

__eglMustCastToProperFunctionPointerType OV_eglGetProcAddress(const char *procname) {
    std::call_once(eglInitFlag, eglInit /* reinterpret_cast<__eglMustCastToProperFunctionPointerType>(eglInit) */);
    LOGI("What could LWJGL be asking bruh??? procname: %s", procname);

    return getFunctionAddress(std::string(procname));
}

REDIRECT(
    __eglMustCastToProperFunctionPointerType,
    eglGetProcAddress,
    OV_eglGetProcAddress,
    (const char* pn),
    (pn)
)