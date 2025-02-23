#include "gles30/translation.h"
#include "main.h"

#include <GLES3/gl3.h>

void GLES30::registerTranslatedFunctions() {
    REGISTER(glGenVertexArrays);
    REGISTER(glBindVertexArray);
    REGISTER(glUnmapBuffer);
}