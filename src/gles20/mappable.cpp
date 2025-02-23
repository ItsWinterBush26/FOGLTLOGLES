#include "gles20/mappable.h"
#include "main.h"

#include <GLES2/gl2.h>

void GLES20::registerMappableFunctions() {
    REGISTER(glGetString);
    REGISTER(glGenVertexArrays);
    REGISTER(glBindVertexArray);
    REGISTER(glGenBuffers);
    REGISTER(glBindBuffer);
    REGISTER(glBufferData);
    REGISTER(glMapBuffer);
    REGISTER(glUnmapBuffer);
    REGISTER(glVertexAttribPointer);
    REGISTER(glEnableVertexAttribArray);
    REGISTER(glCreateShader);
    REGISTER(glShaderSource);
    REGISTER(glCompileShader);
    REGISTER(glGetShaderiv);
    REGISTER(glCreateProgram);
    REGISTER(glAttachShader);
    REGISTER(glLinkProgram);
    REGISTER(glUseProgram);
    REGISTER(glGenTextures);
    REGISTER(glActiveTexture);
    REGISTER(glBindTexture);
    REGISTER(glTexImage2D);
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
