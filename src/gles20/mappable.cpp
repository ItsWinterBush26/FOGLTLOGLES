#include "gles20/mappable.h"
#include "main.h"

#include <GLES2/gl2.h>

void registerExtraFunctions() {
	REGISTER(glBlendFunc);
	REGISTER(glBlendEquation);
	// glClearDepth in translation.cpp 
	REGISTER(glClearStencil);
	REGISTER(glDepthFunc);
	REGISTER(glDepthMask);
	REGISTER(glDepthRangef);
	REGISTER(glStencilFunc);
	REGISTER(glStencilOp);
	REGISTER(glStencilMask);
}

void registerBufferFunctions() {
	REGISTER(glGenBuffers);
	REGISTER(glBindBuffer);
	REGISTER(glBufferData);
	REGISTER(glBufferSubData);
	REGISTER(glDeleteBuffers);
}

void registerTextureFunctions() {
	REGISTER(glGenTextures);
	REGISTER(glBindTexture);
	REGISTER(glTexImage2D);
	REGISTER(glTexSubImage2D);
	REGISTER(glTexParameteri);
	REGISTER(glTexParameterf);
	REGISTER(glActiveTexture);
	REGISTER(glDeleteTextures);
}

void registerBufferExtFunctions() {
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
}

void registerShaderFunctions() {
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
}

void registerVertexAttribFunctions() {
	REGISTER(glEnableVertexAttribArray);
	REGISTER(glDisableVertexAttribArray);
	REGISTER(glVertexAttribPointer);
	REGISTER(glVertexAttrib1f);
	REGISTER(glVertexAttrib2f);
	REGISTER(glVertexAttrib3f);
	REGISTER(glVertexAttrib4f);
}

void registerUniformFunctions() {
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
}

void registerDrawingFunctions() {
	REGISTER(glDrawArrays);
	REGISTER(glDrawElements);
	REGISTER(glFinish);
	REGISTER(glFlush);
}

void registerStateQueriesFunctions() {
	REGISTER(glGetIntegerv);
	REGISTER(glGetFloatv);
	REGISTER(glGetBooleanv);
	REGISTER(glGetError);
	REGISTER(glIsEnabled);
	REGISTER(glIsTexture);
	REGISTER(glIsBuffer);
	REGISTER(glIsFramebuffer);
	REGISTER(glIsRenderbuffer);
}

// void glBindFragDataLocationEXT(GLuint program, GLuint colorNumber, const GLchar* name);

void GLES20::registerMappableFunctions() {
	REGISTER(glClearColor);
	REGISTER(glClear);
	REGISTER(glViewport);
	REGISTER(glScissor);
	REGISTER(glEnable);
	REGISTER(glDisable);
	REGISTER(glGetString);
	
	registerExtraFunctions();
	registerBufferFunctions();
	registerTextureFunctions();
	registerBufferExtFunctions();
	registerShaderFunctions();
	registerDrawingFunctions();
	registerVertexAttribFunctions();
	registerUniformFunctions();
	registerStateQueriesFunctions();

	// REGISTERREDIR("glBindFragDataLocation", glBindFragDataLocationEXT);
}