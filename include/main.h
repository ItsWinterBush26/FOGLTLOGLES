#pragma once

#include <string>

typedef void (*FunctionPtr)(void);

#ifndef TO_FUNCTIONPTR
#define TO_FUNCTIONPTR(func) reinterpret_cast<FunctionPtr>(func)
#endif

void registerFunction(std::string, FunctionPtr);
FunctionPtr getFunctionAddress(std::string);

#ifndef REGISTER
#define REGISTER(func) registerFunction(#func, TO_FUNCTIONPTR(func))
#endif

void init();
