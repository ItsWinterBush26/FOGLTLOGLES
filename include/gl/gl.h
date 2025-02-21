#pragma once

#include "gl/header.h"
#include "utils/types.h"
#include "utils/defines.h"

PUBLIC_API FunctionPtr glXGetProcAddress(char const* pn);

void glInit();