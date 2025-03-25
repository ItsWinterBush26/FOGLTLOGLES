#pragma once

#include "wrappers/base.h"

#include <memory>

namespace GLES20 {
    class GLES20Wrapper : BaseWrapper {
        public:
            void init();
    };
    
    const inline std::shared_ptr<GLES20Wrapper> wrapper = std::make_shared<GLES20Wrapper>();

    void registerShaderOverrides();
    void registerTextureOverrides();
    void registerMultiDrawEmulation();
    void registerBindFunction();
    void registerRenderbufferWorkaround();
}