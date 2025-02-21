#pragma once

#include "wrappers/base.h"
#include <memory>

namespace GLES30 {
    class GLES30Wrapper : BaseWrapper {
        public:
            void init();
    };
    
    const inline std::shared_ptr<GLES30Wrapper> wrapper = std::make_shared<GLES30Wrapper>();
}