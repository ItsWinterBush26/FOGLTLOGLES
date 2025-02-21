#pragma once

#include "utils/types.h"
#include <string>

namespace FOGLTLOGLES {
    void registerFunction(std::string, FunctionPtr);
    FunctionPtr getFunctionAddress(std::string);

    void init();
}

#ifndef REGISTER
#define REGISTER(func) FOGLTLOGLES::registerFunction(#func, reinterpret_cast<FunctionPtr>(func))
#endif

#ifndef REGISTEROV
#define REGISTEROV(func) FOGLTLOGLES::registerFunction(#func, reinterpret_cast<FunctionPtr>(OV_##func));
#endif