#pragma once

#include "wrappers/base.h"

#include <memory>

namespace GLES {
    class GLESWrapper : BaseWrapper {
        public:
            void init();
    };
    
    const inline std::shared_ptr<GLESWrapper> wrapper = std::make_shared<GLESWrapper>();

    void registerBrandingOverride();
    void registerTranslatedFunctions();
}