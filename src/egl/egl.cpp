#include "egl/egl.h"
#include "main.h"
#include "utils/log.h"
#include "utils/types.h"

#include <EGL/egl.h>
#include <dlfcn.h>
#include <mutex>
#include <vector>

FunctionPtr eglGetProcAddress(str procname) {
    std::call_once(eglInitFlag, eglInit);
    FunctionPtr tmp = FOGLTLOGLES::getFunctionAddress(procname);
    
    if (tmp) return tmp;
    if (real_eglGetProcAddress) return real_eglGetProcAddress(procname);
    
    return nullptr;
}

EGLBoolean OV_eglChooseConfig(
    EGLDisplay dpy,
    const EGLint* attrib_list,
    EGLConfig* configs,
    EGLint config_size,
    EGLint* num_config
) {
    EGLint count = 0;
    while (attrib_list[count] != EGL_NONE) {
        count += 2;
    }

    std::vector<EGLint> attrib_list_mod(attrib_list, attrib_list + count);

    bool foundRenderableType;
    int index = 0;
    for (const auto& attrib : attrib_list_mod) {
        if (attrib == EGL_RENDERABLE_TYPE) {
            foundRenderableType = true;

            index++;
            continue;
        }

        if (foundRenderableType) {
            if (!(attrib_list_mod[index] & EGL_OPENGL_ES_BIT)) attrib_list_mod[index] = attrib_list_mod[index] | EGL_OPENGL_ES_BIT;
            break;
        }

        index++;
    }
    attrib_list_mod.push_back(EGL_NONE);

    return eglChooseConfig(dpy, attrib_list_mod.data(), configs, config_size, num_config);
}

EGLBoolean OV_eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
    EGLBoolean result = eglMakeCurrent(dpy, draw, read, ctx);
    std::call_once(rendererInitFlag, FOGLTLOGLES::init);
    return result;
}

inline void eglInit() {
    LOGI("Initializing EGL functions...");

    REGISTER(eglBindAPI);
    REGISTEROV(eglChooseConfig);
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

    void* eglGetProcAddressSymbol = dlsym(eglLibHandle, "eglGetProcAddress");
    if (!eglLibHandle || !eglGetProcAddressSymbol) {
        LOGW("Failed to load libEGL.so/eglGetProcAddress! You may experience some issues...");
        return;
    }

    real_eglGetProcAddress = reinterpret_cast<PFNEGLGETPROCADDRESSPROC>(eglGetProcAddressSymbol);

    LOGI("Done initializing!");
}
