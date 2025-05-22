#pragma once

#include "utils/env.h"
#include "wrappers/base.h"

#include <memory>

inline bool debugEnabled = getEnvironmentVar("LIBGL_VGPU_DUMP", "0") == "1";

namespace GLES {
    class GLESWrapper : BaseWrapper {
        public:
            void init();
    };
    
    const inline std::shared_ptr<GLESWrapper> wrapper = std::make_shared<GLESWrapper>();
    
    void registerBrandingOverride();
    void registerTranslatedFunctions();
    void registerNoErrorOverride();
    void registerDrawOverride();
    void registerTexturesOverride();
}