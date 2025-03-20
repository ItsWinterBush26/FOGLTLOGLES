#pragma once

#include "wrappers/base.h"

#include <memory>

#define REGISTEREXT(name, prefix) FOGLTLOGLES::registerFunction(#name, TO_FUNCPTR(eglGetProcAddress(#name prefix)));

namespace GLES20Ext {
    class GLES20ExtWrapper : BaseWrapper {
        public:
            void init();
    };

    const inline std::shared_ptr<GLES20ExtWrapper> wrapper = std::make_shared<GLES20ExtWrapper>();

    void register_ARB_buffer_storage();
}