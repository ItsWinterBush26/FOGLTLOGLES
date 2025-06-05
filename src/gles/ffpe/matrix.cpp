#include "es/ffp.h"
#include "gles/ffp/enums.h"
#include "gles/ffp/main.h"
#include "glm/ext/matrix_clip_space.hpp"
#include "glm/ext/matrix_float4x4.hpp"
#include "glm/ext/matrix_transform.hpp"
#include "glm/trigonometric.hpp"
#include "main.h"
#include "utils/pointers.h"

#include <GLES3/gl32.h>

void glMatrixMode(GLenum mode);

void glPushMatrix();
void glPopMatrix();
void glLoadIdentity();

void glLoadMatrixd(const GLdouble *m);
void glLoadMatrixf(const GLfloat *m);
 
void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);
void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val);

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

    REGISTER(glPushMatrix);
    REGISTER(glPopMatrix);
    REGISTER(glLoadIdentity);

    REGISTER(glLoadMatrixd);
    REGISTER(glLoadMatrixf);

    REGISTER(glOrtho);
    REGISTER(glFrustum);

    REGISTER(glMultMatrixd);
    REGISTER(glMultMatrixf);

    REGISTER(glRotated);
    REGISTER(glRotatef);

    REGISTER(glScaled);
    REGISTER(glScalef);

    REGISTER(glTranslated);
    REGISTER(glTranslatef);

    Matrices::matricesStateManager = MakeAggregateShared<Matrices::MatricesStateManager>();
}

void glMatrixMode(GLenum mode) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glMatrixMode>(mode);
        return;
    }
    
    switch (mode) {
        case GL_MODELVIEW:
        case GL_PROJECTION:
        case GL_TEXTURE:
            break;
        default:
            return;
    }
    
    Matrices::matricesStateManager->setCurrentMatrix(mode);
}

void glPushMatrix() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glPushMatrix>();
        return;
    }

    Matrices::matricesStateManager->pushCurrentMatrix();
}

void glPopMatrix() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glPopMatrix>();
        return;
    }

    Matrices::matricesStateManager->popTopMatrix();
}

void glLoadIdentity() {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glLoadIdentity>();
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([](glm::mat4) { return glm::mat4(1.0f); });
}

void glLoadMatrixd(const GLdouble *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glLoadMatrixd>(m);
        return;
    }
    
    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4) {
        return glm::mat4(
            m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
        );
    });

}

void glLoadMatrixf(const GLfloat *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glLoadMatrixf>(m);
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4) {
        return glm::mat4(
            m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
        );
    });
}

void glOrtho(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glOrtho>(left, right, bottom, top, near_val, far_val);
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) {
        return currentMatrix * glm::ortho(
            left, right,
            bottom, top,
            near_val, far_val
        );
    });
}

void glFrustum(GLdouble left, GLdouble right, GLdouble bottom, GLdouble top, GLdouble near_val, GLdouble far_val) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glFrustum>(left, right, bottom, top, near_val, far_val);
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) {
        return currentMatrix * glm::frustum(
            left, right,
            bottom, top,
            near_val, far_val
        );
    });
}

void glMultMatrixd(const GLdouble *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glMultMatrixd>(m);
        return;
    }
    
    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) {
        return currentMatrix * glm::mat4(
            m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
        );
    });
}

void glMultMatrixf(const GLfloat *m) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glMultMatrixf>(m);
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) {
        return currentMatrix * glm::mat4(
            m[0], m[1], m[2], m[3],
            m[4], m[5], m[6], m[7],
            m[8], m[9], m[10], m[11],
            m[12], m[13], m[14], m[15]
        );
    });
}

void glRotated(GLdouble angle, GLdouble x, GLdouble y, GLdouble z) {
    glRotatef(static_cast<float>(angle), static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void glRotatef(GLfloat angle, GLfloat x, GLfloat y, GLfloat z) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glRotatef>(angle, x, y, z);
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) {
        return glm::rotate(currentMatrix, glm::radians(angle), glm::vec3(x, y, z));
    });
}

void glScaled(GLdouble x, GLdouble y, GLdouble z) {
    glScalef(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void glScalef(GLfloat x, GLfloat y, GLfloat z) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glScalef>(x, y, z);
        return;
    }

    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) {
        return glm::scale(currentMatrix, glm::vec3(x, y, z));
    });
}

void glTranslated(GLdouble x, GLdouble y, GLdouble z) {
    glTranslatef(static_cast<float>(x), static_cast<float>(y), static_cast<float>(z));
}

void glTranslatef(GLfloat x, GLfloat y, GLfloat z) {
    if (Lists::displayListManager->isRecording()) {
        Lists::displayListManager->addCommand<glTranslatef>(x, y, z);
        return;
    }
    
    Matrices::matricesStateManager->modifyCurrentMatrix([&](glm::mat4 currentMatrix) { 
        return glm::translate(currentMatrix, glm::vec3(x, y, z));
    });
}
