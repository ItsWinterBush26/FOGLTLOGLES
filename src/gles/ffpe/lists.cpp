#include "es/ffp.hpp"
#include "es/utils.hpp"
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
    ESUtils::TypeTraits::dispatchAsType(type, [&]<typename T>() {
        Lists::displayListManager->callDisplayLists(
            n, ESUtils::TypeTraits::asTypedArray<T>(lists)
        );
    });
}

void glListBase(GLuint base) {
    LOGI("glListBase: %u", base);
}

GLboolean glIsList(GLuint list) {
    return Lists::displayListManager->isList(list);
}
