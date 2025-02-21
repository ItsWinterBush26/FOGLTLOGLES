#pragma once

#include "utils/types.h"

namespace FOGLTLOGLES {
    void registerFunction(str, FunctionPtr);
    FunctionPtr getFunctionAddress(str);

    void init();
}

#ifndef REGISTER
#define REGISTER(func) FOGLTLOGLES::registerFunction(#func, reinterpret_cast<FunctionPtr>(func))
#endif

#ifndef REGISTEROV
#define REGISTEROV(func) FOGLTLOGLES::registerFunction(#func, reinterpret_cast<FunctionPtr>(OV_##func));
#endif