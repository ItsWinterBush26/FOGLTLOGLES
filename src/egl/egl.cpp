#include "EGL/egl.h"
#include "main.h"
#include "utils/defines.h"
#include "utils/log.h"

#include <mutex>

static std::once_flag eglInitFlag;

FunctionPtr OV_eglGetProcAddress(const char*);

EGLContext OV_eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
    EGLContext ctx = eglCreateContext(dpy, config, share_context, attrib_list);
    if (ctx == EGL_NO_CONTEXT) {
        LOGE("Failed to create EGL context. EGL error: %i", eglGetError());
        exit(1);
    }
    
    return ctx;
}

EGLBoolean OV_eglInitialize(EGLDisplay dpy, EGLint* maj, EGLint* min) {
    LOGI("eglInitialize!");
    init();
    return eglInitialize(dpy, maj, min);
}

void eglInit() {
    LOGI("Initializing EGL functions...");

    REGISTER(eglBindAPI);
    REGISTER(eglChooseConfig);
    // REGISTER(eglCreateContext);
    REGISTER(eglCreatePbufferSurface);
    REGISTER(eglCreateWindowSurface);
    REGISTER(eglDestroyContext);
    REGISTER(eglDestroySurface);
    REGISTER(eglGetConfigAttrib);
    REGISTER(eglGetCurrentContext);
    REGISTER(eglGetDisplay);
    REGISTER(eglGetError);
    // REGISTER(eglInitialize);
    REGISTER(eglMakeCurrent);
    REGISTER(eglSwapBuffers);
    REGISTER(eglReleaseThread);
    REGISTER(eglSwapInterval);
    REGISTER(eglTerminate);
    REGISTER(eglGetCurrentSurface);
    REGISTER(eglQuerySurface);

    REGISTEROV(eglCreateContext);
    REGISTEROV(eglInitialize);
    REGISTEROV(eglGetProcAddress);

    // registerFunction("eglCreateContext", TO_FUNCTIONPTR(OV_eglCreateContext));
    // registerFunction("eglInitialize", TO_FUNCTIONPTR(OV_eglInitialize));
    // registerFunction("eglGetProcAddress", TO_FUNCTIONPTR(OV_eglGetProcAddress));
    LOGI("Done initializing!");
}

FunctionPtr OV_eglGetProcAddress(const char *procname) {
    std::call_once(eglInitFlag, eglInit);
    return getFunctionAddress(std::string(procname));
}

REDIRECT(
    FunctionPtr,
    eglGetProcAddress,
    OV_eglGetProcAddress,
    (const char* pn),
    (pn)
)
