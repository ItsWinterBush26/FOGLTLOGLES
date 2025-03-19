#pragma once

#include "wrappers/base.h"

#include <memory>

namespace GLES32 {
    class GLES32Wrapper : BaseWrapper {
        public:
            void init();
    };
    
    const inline std::shared_ptr<GLES32Wrapper> wrapper = std::make_shared<GLES32Wrapper>();
}