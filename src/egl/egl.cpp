#include "egl/egl.h"
#include "main.h"
#include "utils/defines.h"
#include "utils/log.h"

#include <string>

OVERRIDE(EGLContext,
    eglCreateContext,
    (EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list)
) {
    /* EGLContext ctx = */
    LOGI("This where we init right?");
    return original_eglCreateContext(dpy, config, share_context, attrib_list);
    // return ctx;
}

WRAP(EGLBoolean,
    eglDestroyContext,
    (EGLDisplay dpy, EGLContext ctx),
    (dpy, ctx)
)

WRAP(EGLBoolean,
    eglMakeCurrent,
    (EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx),
    (dpy, draw, read, ctx)
)

OVERRIDE(__eglMustCastToProperFunctionPointerType,
    eglGetProcAddress,
    (const char *procname)
) {
    LOGI("What could LWJGL be asking bruh??? procname: %s", procname);
    return getGlFunctionAddress(std::string(procname));
}