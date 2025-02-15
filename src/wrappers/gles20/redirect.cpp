#include "wrappers/base.h"

#include <GLES2/gl2.h>

// Easy stuff
REDIRECTV(glClearColor,
		(GLfloat r, GLfloat g, GLfloat b, GLfloat a),
		(r, g, b, a))
REDIRECTV(glClear, glClear, (GLbitfield m), (m))
REDIRECTV(glViewport,
		(GLint x, GLint y, GLsizei w, GLsizei h),
		(x, y, w, h))
REDIRECTV(glScissor,
		(GLint x, GLint y, GLsizei w, GLsizei h),
		(x, y, w, h))
REDIRECTV(glEnable, (GLenum c), (c))
REDIRECTV(glDisable, (GLenum c), (c))
REDIRECT(const GLubyte*, glGetString, (GLenum n), (n))

// Buffer stuff
REDIRECTV(glGenBuffers, (GLsizei n, GLuint *bufs), (n, bufs))
REDIRECTV(glBindBuffer, (GLenum t, GLuint buf), (t, buf))
REDIRECTV(glBufferData, (GLenum t, GLsizeiptr s, const void *d, GLenum u), (t, s, d, u))
REDIRECTV(glBufferSubData,
		(GLenum t, GLintptr o, GLsizeiptr s, const void *d),
		(t, o, s, d))
REDIRECTV(glDeleteBuffers, (GLsizei n, const GLuint *buf), (n, buf))

// Textures
REDIRECTV(glGenTextures, (GLsizei n, GLuint *tex), (n, tex))
REDIRECTV(glBindTexture, (GLenum target, GLuint tex), (target, tex))
// glTexImage2D is in 'emulated.cpp'
REDIRECTV(glTexSubImage2D,
		(GLenum target, GLint l, GLint xoff, GLint yoff, GLsizei w, GLsizei h, GLenum form, GLenum type, const void *px),
		(target, l, xoff, yoff, w, h, form, type, px))
REDIRECTV(glTexParameteri, (GLenum t, GLenum pn, GLint p), (t, pn, p))
REDIRECTV(glActiveTexture, (GLenum tex), (tex))
REDIRECTV(glDeleteTextures, (GLsizei n, const GLuint *texs), (n, texs))

