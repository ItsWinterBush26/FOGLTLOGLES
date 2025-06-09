#pragma once

#include "GLES3/gl32.hpp"

#include "utils/types.hpp"
#include "utils/defines.hpp"

PUBLIC_API FunctionPtr glXGetProcAddress(const GLchar* pn);

void initDebug();