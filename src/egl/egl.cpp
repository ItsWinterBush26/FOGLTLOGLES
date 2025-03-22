#include "egl/egl.h"
#include "main.h"
#include "utils/log.h"
#include "utils/types.h"

#include <EGL/egl.h>
#include <dlfcn.h>
#include <mutex>

inline std::once_flag eglInitFlag;
inline std::once_flag rendererInitFlag;

FunctionPtr eglGetProcAddress(str procname) {
    std::call_once(eglInitFlag, eglInit);
    FunctionPtr tmp = FOGLTLOGLES::getFunctionAddress(procname);
    
    if (tmp != nullptr) return tmp;
    else return real_eglGetProcAddress(procname);
}

EGLBoolean OV_eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
    EGLBoolean result = eglMakeCurrent(dpy, draw, read, ctx);
    std::call_once(rendererInitFlag, FOGLTLOGLES::init);
    return result;
}

inline void eglInit() {
    LOGI("Initializing EGL functions...");

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
    REGISTER(eglGetProcAddress);
    REGISTER(eglInitialize);
    REGISTEROV(eglMakeCurrent);
    REGISTER(eglSwapBuffers);
    REGISTER(eglReleaseThread);
    REGISTER(eglSwapInterval);
    REGISTER(eglTerminate);
    REGISTER(eglGetCurrentSurface);
    REGISTER(eglQuerySurface);
    
    LOGI("Done initializing!");

    real_eglGetProcAddress = reinterpret_cast<PFNEGLGETPROCADDRESSPROC>(dlsym(RTLD_NEXT, "eglGetProcAddress"));
}
