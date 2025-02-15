#include "wrappers/base.h"

#include <GLES2/gl2.h>

// Easy stuff
WRAPV(glClearColor,
	(GLfloat r, GLfloat g, GLfloat b, GLfloat a),
	(r, g, b, a))
WRAPV(glClear, (GLbitfield m), (m));
WRAPV(glViewport,
	(GLint x, GLint y, GLsizei w, GLsizei h),
	(x, y, w, h))
WRAPV(glScissor,
	(GLint x, GLint y, GLsizei w, GLsizei h),
	(x, y, w, h))
WRAPV(glEnable, (GLenum c), (c))
WRAPV(glDisable, (GLenum c), (c))
WRAP(const GLubyte*, glGetString, (GLenum n), (n))

// Buffer stuff
WRAPV(glGenBuffers, (GLsizei n, GLuint *bufs), (n, bufs))
WRAPV(glBindBuffer, (GLenum t, GLuint buf), (t, buf))
WRAPV(glBufferData, (GLenum t, GLsizeiptr s, const void *d, GLenum u), (t, s, d, u))
WRAPV(glBufferSubData,
	(GLenum t, GLintptr o, GLsizeiptr s, const void *d),
	(t, o, s, d))
WRAPV(glDeleteBuffers, (GLsizei n, const GLuint *buf), (n, buf))

// Textures
WRAPV(glGenTextures, (GLsizei n, GLuint *tex), (n, tex))
WRAPV(glBindTexture, (GLenum target, GLuint tex), (target, tex))
// glTexImage2D is in 'emulated.cpp'
WRAPV(glTexSubImage2D,
	(GLenum target, GLint l, GLint xoff, GLint yoff, GLsizei w, GLsizei h, GLenum form, GLenum type, const void *px),
	(target, l, xoff, yoff, w, h, form, type, px))
WRAPV(glTexParameteri, (GLenum t, GLenum pn, GLint p), (t, pn, p))
WRAPV(glActiveTexture, (GLenum tex), (tex))
WRAPV(glDeleteTextures, (GLsizei n, const GLuint *texs), (n, texs))

