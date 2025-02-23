#include "gles30/mappable.h"
#include "main.h"

#include <GLES3/gl3.h>

void GLES30::registerMappableFunctions() {
    REGISTER(glGenVertexArrays);
    REGISTER(glBindVertexArray);
    REGISTER(glUnmapBuffer);
}