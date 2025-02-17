#pragma once

#include <string>

typedef void (*GL2ESFunction)(void);

void registerGlFunctions(std::string, GL2ESFunction);
GL2ESFunction getGlFunctionAddress(std::string);
