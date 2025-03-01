#include "gles30/mappable.h"
#include "main.h"

#include <GLES3/gl32.h>

void GLES30::registerMappableFunctions() {
    // GLES 3.0 core functions
    REGISTER(glGenVertexArrays);
    REGISTER(glBindVertexArray);
    REGISTER(glDeleteVertexArrays);
    REGISTER(glUnmapBuffer);
    REGISTER(glMapBufferRange);
    REGISTER(glDrawArraysInstanced);
    REGISTER(glDrawElementsInstanced);
    REGISTER(glVertexAttribDivisor);
    REGISTER(glGetBufferPointerv);
    REGISTER(glBindBufferRange);
    REGISTER(glBindBufferBase);
    
    // GLES 3.1 functions
    REGISTER(glDispatchCompute);
    REGISTER(glDispatchComputeIndirect);
    REGISTER(glBindImageTexture);
    REGISTER(glMemoryBarrier);
    REGISTER(glProgramUniform1i);
    REGISTER(glProgramUniform1f);
    REGISTER(glProgramUniform2f);
    REGISTER(glProgramUniform3f);
    REGISTER(glProgramUniform4f);
    REGISTER(glProgramUniform1iv);
    REGISTER(glProgramUniform1fv);
    REGISTER(glProgramUniformMatrix4fv);
    REGISTER(glGetProgramResourceIndex);
    
    // GLES 3.2 functions
    REGISTER(glBlendBarrier);
    REGISTER(glCopyImageSubData);
    REGISTER(glDebugMessageControl);
    REGISTER(glDebugMessageInsert);
    REGISTER(glDebugMessageCallback);
    REGISTER(glGetDebugMessageLog);
    REGISTER(glGetObjectLabel);
    REGISTER(glObjectLabel);
    REGISTER(glPopDebugGroup);
    REGISTER(glPushDebugGroup);
    REGISTER(glGetGraphicsResetStatus);
    REGISTER(glReadnPixels);
    REGISTER(glGetnUniformfv);
    REGISTER(glGetnUniformiv);
    REGISTER(glGetSamplerParameterIiv);
    REGISTER(glGetSamplerParameterIuiv);
    REGISTER(glGetTexParameterIiv);
    REGISTER(glGetTexParameterIuiv);
    REGISTER(glSamplerParameterIiv);
    REGISTER(glSamplerParameterIuiv);
    REGISTER(glTexParameterIiv);
    REGISTER(glTexParameterIuiv);
    REGISTER(glTexStorage3DMultisample);
    REGISTER(glFramebufferTexture);
}
