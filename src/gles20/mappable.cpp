#include "gles20/mappable.h"
#include "main.h"

#include <GLES2/gl2.h>

void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}

void GLES20::initMappableFunctions() {
	REGISTER(glClearColor);
	REGISTER(glClear);
	REGISTER(glViewport);
	REGISTER(glScissor);
	REGISTER(glEnable);
	REGISTER(glDisable);
	REGISTER(glGetString);
    REGISTER(glClearDepth);
    
	// Buffer operations
	REGISTER(glGenBuffers);
	REGISTER(glBindBuffer);
	REGISTER(glBufferData);
	REGISTER(glBufferSubData);
	REGISTER(glDeleteBuffers);

	// Textures
	REGISTER(glGenTextures);
	REGISTER(glBindTexture);
	REGISTER(glTexImage2D);
	REGISTER(glTexSubImage2D);
	REGISTER(glTexParameteri);
	REGISTER(glTexParameterf);
	REGISTER(glActiveTexture);
	REGISTER(glDeleteTextures);

	// Framebuffer & Renderbuffer
	REGISTER(glGenFramebuffers);
	REGISTER(glBindFramebuffer);
	REGISTER(glFramebufferTexture2D);
	REGISTER(glDeleteFramebuffers);
	REGISTER(glCheckFramebufferStatus);

	REGISTER(glGenRenderbuffers);
	REGISTER(glBindRenderbuffer);
	REGISTER(glRenderbufferStorage);
	REGISTER(glFramebufferRenderbuffer);
	REGISTER(glDeleteRenderbuffers);

	// Shader & Program Management
	REGISTER(glCreateShader);
	REGISTER(glShaderSource);
	REGISTER(glCompileShader);
	REGISTER(glDeleteShader);
	REGISTER(glGetShaderiv);
	REGISTER(glGetShaderInfoLog);

	REGISTER(glCreateProgram);
	REGISTER(glAttachShader);
	REGISTER(glLinkProgram);
	REGISTER(glUseProgram);
	REGISTER(glDeleteProgram);
	REGISTER(glGetProgramiv);
	REGISTER(glGetProgramInfoLog);

	// Vertex Attributes
	REGISTER(glEnableVertexAttribArray);
	REGISTER(glDisableVertexAttribArray);
	REGISTER(glVertexAttribPointer);
	REGISTER(glVertexAttrib1f);
	REGISTER(glVertexAttrib2f);
	REGISTER(glVertexAttrib3f);
	REGISTER(glVertexAttrib4f);

	// Uniforms
	REGISTER(glGetUniformLocation);
	REGISTER(glUniform1f);
	REGISTER(glUniform2f);
	REGISTER(glUniform3f);
	REGISTER(glUniform4f);
	REGISTER(glUniform1i);
	REGISTER(glUniform2i);
	REGISTER(glUniform3i);
	REGISTER(glUniform4i);
	REGISTER(glUniformMatrix4fv);

	// Drawing
	REGISTER(glDrawArrays);
	REGISTER(glDrawElements);
	REGISTER(glFinish);
	REGISTER(glFlush);

	// State Queries
	REGISTER(glGetIntegerv);
	REGISTER(glGetFloatv);
	REGISTER(glGetBooleanv);
	REGISTER(glGetError);
	REGISTER(glIsEnabled);
	REGISTER(glIsTexture);
	REGISTER(glIsBuffer);
	REGISTER(glIsFramebuffer);
	REGISTER(glIsRenderbuffer);

	// Additional OpenGL ES 2.0 functions
	REGISTER(glBlendFunc);
	REGISTER(glBlendEquation);
	REGISTER(glClearDepthf);
	REGISTER(glClearStencil);
	REGISTER(glDepthFunc);
	REGISTER(glDepthMask);
	REGISTER(glDepthRangef);
	REGISTER(glStencilFunc);
	REGISTER(glStencilOp);
	REGISTER(glStencilMask);
}
