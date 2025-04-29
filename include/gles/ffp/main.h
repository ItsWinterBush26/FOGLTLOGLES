#pragma once

#include "wrappers/base.h"

#include <memory>

namespace FFP {
    class FFPWrapper : BaseWrapper {
        public:
            void init();
    };
    
    const inline std::shared_ptr<FFPWrapper> wrapper = std::make_shared<FFPWrapper>();

    void registerImmediateFunctions();
    void registerVertexFunctions();
    void registerMatrixFunctions();
}