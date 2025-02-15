#include "base/base.h"

#include "GLES2/gl2.h"

// Easy stuff
REDIRECTV(glClearColor,
		glClearColor,
		(GLfloat r, GLfloat g, GLfloat b, GLfloat a),
		(r, g, b, a))
REDIRECTV(glClear, glClear, (GLbitfield m), (m))
REDIRECTV(glViewport,
		glViewport,
		(GLint x, GLint y, GLsizei w, GLsizei h),
		(x, y, w, h))
REDIRECTV(glScissor,
		glScissor,
		(GLint x, GLint y, GLsizei w, GLsizei h),
		(x, y, w, h))
REDIRECTV(glEnable, glEnable, (GLenum c), (c))
REDIRECTV(glDisable, glDisable, (GLenum c), (c))
REDIRECT(const GLubyte*, glGetString, glGetString, (GLenum n), (n))

// Buffer stuff
REDIRECTV(glGenBuffers, glGenBuffers, (GLsizei n, GLuint *buf), (n, buf))
REDIRECTV(glBindBuffer, glBindBuffer, (GLenum t, GLuint buf), (t, buf))
REDIRECTV(glBufferData, glBufferData, (GLenum t, GLsizeiptr s, const void *d, GLenum u), (t, s, d, u))
REDIRECTV(glBufferSubData,
		glBufferSubData,
		(GLenum t, GLintptr o, GLsizeiptr s, const void *d),
		(t, o, s, d))
REDIRECTV(glDeleteBuffers, glDeleteBuffers, (GLsizei n, const GLuint *buf), (n, buf))

// Textures
REDIRECTV(glGenTextures, glGenTextures, (GLsizei n, GLuint *tex), (n, tex))
REDIRECTV(glBindTexture, glBindTexture, (GLenum target, GLuint tex), (target, tex))
// glTexImage2D is in 'emulated.cpp'
