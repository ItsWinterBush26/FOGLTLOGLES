#include "EGL/egl.h"
#include "main.h"
#include "utils/defines.h"
#include "utils/log.h"

#include <mutex>

static std::once_flag eglInitFlag;

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


__eglMustCastToProperFunctionPointerType OV_eglGetProcAddress(const char* pn);

void eglInit() {
    LOGI("Initializing EGL overrides...");

    REGISTER(eglBindAPI);
    REGISTER(eglChooseConfig);
    REGISTER(eglCreateContext);
    REGISTER(eglCreatePbufferSurface);
    REGISTER(eglCreateWindowSurface);
    REGISTER(eglDestroyContext);
    REGISTER(eglDestroySurface);
    REGISTER(eglGetConfigAttrib);
    REGISTER(eglGetCurrentContext);
    REGISTER(eglGetDisplay);
    REGISTER(eglGetError);
    REGISTER(eglInitialize);
    REGISTER(eglMakeCurrent);
    REGISTER(eglSwapBuffers);
    REGISTER(eglReleaseThread);
    REGISTER(eglSwapInterval);
    REGISTER(eglTerminate);
    REGISTER(eglGetCurrentSurface);

    // just in case
    registerFunction("eglGetProcAddress", TO_FUNCTIONPTR(OV_eglGetProcAddress));
    LOGI("Done initializing!");
}

__eglMustCastToProperFunctionPointerType OV_eglGetProcAddress(const char *procname) {
    std::call_once(eglInitFlag, eglInit);
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