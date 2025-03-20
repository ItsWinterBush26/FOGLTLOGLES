#pragma once

#include "wrappers/base.h"

#include <memory>

#define REGISTEREXT(type, name, prefix) \
    static type real_##name = reinterpret_cast<type>(eglGetProcAddress(#name prefix)); \
    FOGLTLOGLES::registerFunction(#name, TO_FUNCPTR(real_##name));

namespace GLES20Ext {
    class GLES20ExtWrapper : BaseWrapper {
        public:
            void init();
    };

    const inline std::shared_ptr<GLES20ExtWrapper> wrapper = std::make_shared<GLES20ExtWrapper>();

    void register_ARB_buffer_storage();
}