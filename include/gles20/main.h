#pragma once

#include "wrappers/base.h"

class GLES20Wrapper : BaseWrapper {
public:
    void init();
};

static inline GLES20Wrapper gles20Wrapper;