#pragma once

#include "wrappers/base.h"

#include <memory>

#define GETFUNCEXT(name, suffix) TO_FUNCPTR(real_eglGetProcAddress(#name suffix))
#define REGISTEREXT(name, suffix) FOGLTLOGLES::registerFunction(#name, GETFUNCEXT(name, suffix))

namespace GLES20Ext {
    class GLES20ExtWrapper : BaseWrapper {
        public:
            void init();
    };

    const inline std::shared_ptr<GLES20ExtWrapper> wrapper = std::make_shared<GLES20ExtWrapper>();

    void register_EXT_buffer_storage();
    void register_OES_mapbuffer();
    void register_EXT_blend_func_extended();
    void register_GL_ARB_compute_shader();
}