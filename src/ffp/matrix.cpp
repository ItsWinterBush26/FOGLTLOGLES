#include "es/ffp.h"
#include "ffp/main.h"
#include "main.h"

#include <glm/glm.hpp>
#include <GLES/gl.h>

typedef double GLdouble;

void OV_glMatrixMode(GLenum mode);
 
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void OV_glViewport(GLint x, GLint y, GLsizei width, GLsizei height);

void OV_glPushMatrix();
void OV_glPopMatrix();
void OV_glLoadIdentity();

void glLoadMatrixd(const GLdouble *m);
void OV_glLoadMatrixf(const GLfloat *m);

void glMultMatrixd(const GLdouble *m);
void OV_glMultMatrixf(const GLfloat *m);

void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z);
void OV_glRotatef( GLfloat angle, GLfloat x, GLfloat y, GLfloat z);

void glScaled(GLdouble x, GLdouble y, GLdouble z);
void OV_glScalef(GLfloat x, GLfloat y, GLfloat z);

void glTranslated(GLdouble x, GLdouble y, GLdouble z);
void OV_glTranslatef(GLfloat x, GLfloat y, GLfloat z);


void FFP::registerMatrixFunctions() {
    REGISTEROV(glMatrixMode);

    REGISTER(glOrtho);
    REGISTER(glFrustum);
    REGISTEROV(glViewport);

    REGISTEROV(glPushMatrix);
    REGISTEROV(glPopMatrix);
    REGISTEROV(glLoadIdentity);

    REGISTER(glLoadMatrixd);
    REGISTEROV(glLoadMatrixf);

    REGISTER(glMultMatrixd);
    REGISTEROV(glMultMatrixf);

    REGISTER(glRotated);
    REGISTEROV(glRotatef);

    REGISTER(glScaled);
    REGISTEROV(glScalef);

    REGISTER(glTranslated);
    REGISTEROV(glTranslatef);
}

void OV_glMatrixMode(GLenum mode) {
    switch (mode) {
        case GL_MODELVIEW:
        case GL_PROJECTION:
        case GL_TEXTURE:
            break;
        default:
            return;
    }
    
    currentMatrixMode = mode;
}