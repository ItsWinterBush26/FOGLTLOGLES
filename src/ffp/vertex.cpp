#include "es/ffp.h"
#include "ffp/main.h"

#include "gl/header.h"
#include "main.h"

#pragma region Vertex Integer Declarations
void glVertex2i(GLint x, GLint y);
void glVertex3i(GLint x, GLint y, GLint z);
void glVertex4i(GLint x, GLint y, GLint z, GLint w);

void glVertex2iv(const GLint *v);
void glVertex3iv(const GLint *v);
void glVertex4iv(const GLint *v);
#pragma endregion

#pragma region Vertex Float Declarations
void glVertex2f(GLfloat x, GLfloat y);
void glVertex3f(GLfloat x, GLfloat y, GLfloat z);
void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w);

void glVertex2fv(const GLfloat *v);
void glVertex3fv(const GLfloat *v);
void glVertex4fv(const GLfloat *v);
#pragma endregion

void FFP::registerVertexFunctions() {
#pragma region Vertex Float Registrations
    REGISTER(glVertex2i);
    REGISTER(glVertex3i);
    REGISTER(glVertex4i);

    REGISTER(glVertex2iv);
    REGISTER(glVertex3iv);
    REGISTER(glVertex4iv);
#pragma endregion

#pragma region Vertex Float Registrations
    REGISTER(glVertex2f);
    REGISTER(glVertex3f);
    REGISTER(glVertex4f);

    REGISTER(glVertex2fv);
    REGISTER(glVertex3fv);
    REGISTER(glVertex4fv);
#pragma endregion
}


#pragma region Vertex Integer Implementations
void glVertex2i(GLint x, GLint y) {
    intVertexBuffer.push_back(x);
    intVertexBuffer.push_back(y);
}

void glVertex3i(GLint x, GLint y, GLint z) {
    intVertexBuffer.push_back(x);
    intVertexBuffer.push_back(y);
    intVertexBuffer.push_back(z);
}

void glVertex4i(GLint x, GLint y, GLint z, GLint w) {
    intVertexBuffer.push_back(x);
    intVertexBuffer.push_back(y);
    intVertexBuffer.push_back(z);
    intVertexBuffer.push_back(w);
}

void glVertex2iv(const GLint *v) {
    glVertex2i(v[0], v[1]);
}

void glVertex3iv(const GLint *v) {
    glVertex3i(v[0], v[1], v[2]);
}

void glVertex4iv(const GLint *v) {
    glVertex4i(v[0], v[1], v[2], v[3]);
}
#pragma endregion

#pragma region Vertex Float Implementations
void glVertex2f(GLfloat x, GLfloat y) {
    floatVertexBuffer.push_back(x);
    floatVertexBuffer.push_back(y);
}

void glVertex3f(GLfloat x, GLfloat y, GLfloat z) {
    floatVertexBuffer.push_back(x);
    floatVertexBuffer.push_back(y);
    floatVertexBuffer.push_back(z);
}

void glVertex4f(GLfloat x, GLfloat y, GLfloat z, GLfloat w) {
    floatVertexBuffer.push_back(x);
    floatVertexBuffer.push_back(y);
    floatVertexBuffer.push_back(z);
    floatVertexBuffer.push_back(w);
}

void glVertex2fv(const GLfloat *v) {
    glVertex2f(v[0], v[1]);
}

void glVertex3fv(const GLfloat *v) {
    glVertex3f(v[0], v[1], v[2]);
}

void glVertex4fv(const GLfloat *v) {
    glVertex4f(v[0], v[1], v[2], v[3]);
}
#pragma endregion