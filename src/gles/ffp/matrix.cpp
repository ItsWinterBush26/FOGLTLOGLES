#include "es/ffp.h"
#include "gles/ffp/main.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "main.h"

#include <glm/glm.hpp>
#include <GLES/gl.h>

typedef double GLdouble;

void OV_glMatrixMode(GLenum mode);

void OV_glPushMatrix();
void OV_glPopMatrix();
void OV_glLoadIdentity();

void glLoadMatrixd(const GLdouble *m);
void OV_glLoadMatrixf(const GLfloat *m);
 
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);

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

    REGISTEROV(glPushMatrix);
    REGISTEROV(glPopMatrix);
    REGISTEROV(glLoadIdentity);

    REGISTER(glLoadMatrixd);
    REGISTEROV(glLoadMatrixf);

    REGISTER(glOrtho);
    REGISTER(glFrustum);

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
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glMatrixMode>(mode);
        return;
    }
    
    switch (mode) {
        case GL_MODELVIEW:
        case GL_PROJECTION:
            break;
        default:
            return;
    }
    
    currentMatrixMode = mode;

    glMatrixMode(mode);
}

void OV_glPushMatrix() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glPushMatrix>();
        return;
    }

    matrixStack.push(currentMatrix);
}

void OV_glPopMatrix() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glPopMatrix>();
        return;
    }

    if (matrixStack.empty()) {
        return;
    }

    currentMatrix = matrixStack.top();
    matrixStack.pop();
}

void OV_glLoadIdentity() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glLoadIdentity>();
        return;
    }

    currentMatrix = glm::mat4(1.0f);
}

void glLoadMatrixd(const GLdouble *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glLoadMatrixd>(m);
        return;
    }
    
    currentMatrix = glm::mat4(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9], m[10], m[11],
        m[12], m[13], m[14], m[15]
    );
}

void OV_glLoadMatrixf(const GLfloat *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glLoadMatrixf>(m);
        return;
    }

    currentMatrix = glm::mat4(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9], m[10], m[11],
        m[12], m[13], m[14], m[15]
    );

    glLoadMatrixf(m);
}

void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glOrtho>(left, right, bottom, top, near_val, far_val);
        return;
    }

    currentMatrix = glm::ortho(
        left, right,
        bottom, top,
        near_val, far_val
    );
}

void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glFrustum>(left, right, bottom, top, near_val, far_val);
        return;
    }

    currentMatrix = glm::frustum(
        left, right,
        bottom, top,
        near_val, far_val
    );
}

void glMultMatrixd(const GLdouble *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glMultMatrixd>(m);
        return;
    }

    glm::mat4 matrix = glm::mat4(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9], m[10], m[11],
        m[12], m[13], m[14], m[15]
    );
    currentMatrix = currentMatrix * matrix;
}

void OV_glMultMatrixf(const GLfloat *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glMultMatrixf>(m);
        return;
    }

    glm::mat4 matrix = glm::mat4(
        m[0], m[1], m[2], m[3],
        m[4], m[5], m[6], m[7],
        m[8], m[9], m[10], m[11],
        m[12], m[13], m[14], m[15]
    );
    currentMatrix = currentMatrix * matrix;
}

void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
    OV_glRotatef(static_cast<float>(angle), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void OV_glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glRotatef>(angle, x, y, z);
        return;
    }

    currentMatrix = glm::rotate(currentMatrix, glm::radians(angle), glm::vec3(x, y, z));

    glRotatef(angle, x, y, z);
}

void glScaled(GLdouble x, GLdouble y, GLdouble z) {
    OV_glScalef(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void OV_glScalef(GLfloat x, GLfloat y, GLfloat z) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glScalef>(x, y, z);
        return;
    }

    currentMatrix = glm::scale(currentMatrix, glm::vec3(x, y, z));

    glScalef(x, y, z);
}

void glTranslated(GLdouble x, GLdouble y, GLdouble z) {
    OV_glTranslatef(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void OV_glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<OV_glTranslatef>(x, y, z);
        return;
    }
    
    currentMatrix = glm::translate(currentMatrix, glm::vec3(x, y, z));

    glTranslatef(x, y, z);
}
