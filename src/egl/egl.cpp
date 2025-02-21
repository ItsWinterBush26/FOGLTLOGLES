#include "EGL/egl.h"
#include "egl/egl.h"
#include "main.h"
#include "utils/log.h"
#include "utils/types.h"

FunctionPtr eglGetProcAddress(const char *procname) {
    return FOGLTLOGLES::getFunctionAddress(std::string(procname));
}

EGLContext OV_eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list) {
    return eglCreateContext(dpy, config, share_context, attrib_list);
}

EGLBoolean OV_eglInitialize(EGLDisplay dpy, EGLint* maj, EGLint* min) {
    FOGLTLOGLES::init();
    return eglInitialize(dpy, maj, min);
}

void eglInit() {
    LOGI("Initializing EGL functions...");

    REGISTER(eglBindAPI);
    REGISTER(eglChooseConfig);
    REGISTER(eglCreatePbufferSurface);
    REGISTER(eglCreateWindowSurface);
    REGISTER(eglDestroyContext);
    REGISTER(eglDestroySurface);
    REGISTER(eglGetConfigAttrib);
    REGISTER(eglGetCurrentContext);
    REGISTER(eglGetDisplay);
    REGISTER(eglGetError);
    REGISTER(eglMakeCurrent);
    REGISTER(eglSwapBuffers);
    REGISTER(eglReleaseThread);
    REGISTER(eglSwapInterval);
    REGISTER(eglTerminate);
    REGISTER(eglGetCurrentSurface);
    REGISTER(eglQuerySurface);
    REGISTER(eglGetProcAddress);

    REGISTEROV(eglCreateContext);
    REGISTEROV(eglInitialize);
    LOGI("Done initializing!");
}
