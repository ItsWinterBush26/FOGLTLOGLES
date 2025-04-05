#include "ffp/main.h"
#include "main.h"

#include <GLES/gl.h>

typedef double GLdouble;

void glMatrixMode(GLenum mode);
 
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

void glPushMatrix();
void glPopMatrix();
void glLoadIdentity();

void glLoadMatrixd(const GLdouble *m);
void glLoadMatrixf(const GLfloat *m);

void glMultMatrixd(const GLdouble *m);
void glMultMatrixf(const GLfloat *m);

void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
void glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

void glScaled(GLdouble x, GLdouble y, GLdouble z);
void glScalef(GLfloat x, GLfloat y, GLfloat z);

void glTranslated(GLdouble x, GLdouble y, GLdouble z);
void glTranslatef(GLfloat x, GLfloat y, GLfloat z);


void FFP::registerMatrixFunctions() {
    REGISTER(glMatrixMode);

    REGISTER(glOrtho);
    REGISTER(glFrustum);
    REGISTER(glViewport);

    REGISTER(glPushMatrix);
    REGISTER(glPopMatrix);
    REGISTER(glLoadIdentity);

    REGISTER(glLoadMatrixd);
    REGISTER(glLoadMatrixf);

    REGISTER(glMultMatrixd);
    REGISTER(glMultMatrixf);

    REGISTER(glRotated);
    REGISTER(glRotatef);

    REGISTER(glScaled);
    REGISTER(glScalef);

    REGISTER(glTranslated);
    REGISTER(glTranslatef);
}