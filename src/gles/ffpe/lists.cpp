#include "es/ffp.hpp"
#include "gles/ffp/main.hpp"
#include "main.hpp"
#include "utils/log.hpp"

#include <GLES3/gl32.h>

GLuint glGenLists(GLsizei range);
void glDeleteLists(GLuint list, GLsizei range);

void glNewList(GLuint list, GLenum mode);
void glEndList();

void glCallList(GLuint list);
void glCallLists(GLsizei n, GLenum type, const void* lists);

void glListBase(GLuint base);
GLboolean glIsList(GLuint list);

void FFP::registerListsFunctions() {
    REGISTER(glGenLists);
    REGISTER(glDeleteLists);

    REGISTER(glNewList);
    REGISTER(glEndList);

    REGISTER(glCallList);
    REGISTER(glCallLists);

    REGISTER(glListBase);
    REGISTER(glIsList);
}

GLuint glGenLists(GLsizei range) {
    return Lists::displayListManager->genDisplayLists(range);
}

void glDeleteLists(GLuint list, GLsizei range) {
    Lists::displayListManager->deleteDisplayLists(list, range);
}

void glNewList(GLuint list, GLenum mode) {
    Lists::displayListManager->startDisplayList(list, mode);
}

void glEndList() {
    Lists::displayListManager->endDisplayList();
}

void glCallList(GLuint list) {
    Lists::displayListManager->callDisplayList(list);
}

void glCallLists(GLsizei n, GLenum type, const void* lists) {
    if (type == GL_UNSIGNED_INT) {
        const GLuint* listArray = static_cast<const GLuint*>(lists);

        Lists::displayListManager->callBeginBatch();
        for (GLsizei i = 0; i < n; ++i) {
            Lists::displayListManager->callDisplayList(listArray[i]);
        }
        Lists::displayListManager->callEndBatch();
    } else if (type == GL_UNSIGNED_BYTE) {
        const GLubyte* listArray = static_cast<const GLubyte*>(lists);

        Lists::displayListManager->callBeginBatch();
        for (GLsizei i = 0; i < n; ++i) {
            Lists::displayListManager->callDisplayList(listArray[i]);
        }
        Lists::displayListManager->callEndBatch();
    }
}

void glListBase(GLuint base) {
    LOGI("glListBase: %u", base);
}

GLboolean glIsList(GLuint list) {
    return Lists::displayListManager->isList(list);
}
