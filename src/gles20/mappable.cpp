#include "gles20/main.h"
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
    REGISTER(glShaderSource);

    // Removed: Functions overridden in shader.cpp
    // REGISTER(glCompileShader);
    // REGISTER(glGetShaderiv);
    // REGISTER(glCreateProgram);
    // REGISTER(glAttachShader);
    // REGISTER(glLinkProgram);
    // REGISTER(glDeleteProgram);
    
    REGISTER(glGenTextures);
    REGISTER(glActiveTexture);
    REGISTER(glBindTexture);
    REGISTER(glTexParameteri);
    
    // Removed: Functions overridden in translation.cpp
    // REGISTER(glTexImage2D);
    // REGISTER(glGetTexLevelParameteriv);
    // REGISTER(glClearDepth);
    
    REGISTER(glGenerateMipmap);
    REGISTER(glUniform1i);
    REGISTER(glUniform1f);
    REGISTER(glUniform2f);
    REGISTER(glUniform3f);
    REGISTER(glUniform4f);
    REGISTER(glUniform1fv);
    REGISTER(glUniform2fv);
    REGISTER(glUniform3fv);
    REGISTER(glUniform4fv);
    REGISTER(glUniformMatrix2fv);
    REGISTER(glUniformMatrix3fv);
    REGISTER(glUniformMatrix4fv);
    REGISTER(glGetUniformLocation);
    REGISTER(glGetAttribLocation);
    REGISTER(glDrawArrays);
    REGISTER(glDrawElements);
    REGISTER(glClear);
    REGISTER(glClearColor);
    REGISTER(glEnable);
    REGISTER(glDisable);
    REGISTER(glBlendFunc);
    REGISTER(glDepthFunc);
    REGISTER(glDepthMask);
    REGISTER(glViewport);
    REGISTER(glScissor);
    REGISTER(glCullFace);
    REGISTER(glFrontFace);
    REGISTER(glBindFramebuffer);
    REGISTER(glFramebufferTexture2D);
    REGISTER(glGenFramebuffers);
    REGISTER(glDeleteFramebuffers);
    REGISTER(glCheckFramebufferStatus);
    REGISTER(glReadPixels);
    REGISTER(glDeleteTextures);
    REGISTER(glDeleteBuffers);
    REGISTER(glColorMask);
    REGISTER(glBufferSubData);
    REGISTER(glDisableVertexAttribArray);
    REGISTER(glVertexAttrib1f);
    REGISTER(glVertexAttrib2f);
    REGISTER(glVertexAttrib3f);
    REGISTER(glVertexAttrib4f);
    REGISTER(glStencilFunc);
    REGISTER(glStencilMask);
    REGISTER(glStencilOp);
    REGISTER(glGetError);
    REGISTER(glFlush);
    REGISTER(glFinish);
}
