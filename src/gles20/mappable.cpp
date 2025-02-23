#include "gles20/mappable.h"
#include "main.h"

#include <GLES2/gl2.h>

void GLES20::registerMappableFunctions() {
    REGISTER(glGetString);
    REGISTER(glGenBuffers);
    REGISTER(glBindBuffer);
    REGISTER(glBufferData);
    REGISTER(glVertexAttribPointer);
    REGISTER(glEnableVertexAttribArray);
    REGISTER(glCreateShader);
    REGISTER(glCompileShader);
    REGISTER(glGetShaderiv);
    REGISTER(glCreateProgram);
    REGISTER(glAttachShader);
    REGISTER(glLinkProgram);
    REGISTER(glUseProgram);
    REGISTER(glGenTextures);
    REGISTER(glActiveTexture);
    REGISTER(glBindTexture);
    REGISTER(glTexParameteri);
    REGISTER(glGenerateMipmap);
    REGISTER(glDrawArrays);
    REGISTER(glDrawElements);
    REGISTER(glClear);
    REGISTER(glEnable);
    REGISTER(glDisable);
    REGISTER(glBlendFunc);
    REGISTER(glDepthFunc);
    REGISTER(glViewport);
}
