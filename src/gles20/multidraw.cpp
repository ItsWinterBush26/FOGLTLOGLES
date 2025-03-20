#include "es/validation.h"
#include "gles20/main.h"
#include "main.h"

#include <GLES2/gl2.h>

#ifndef DRAWCOUNT_THRESHOLD
#define DRAWCOUNT_THRESHOLD 256
#endif

void glMultiDrawArrays(GLenum mode, const GLint* first, const GLsizei* count, GLsizei drawcount);
void glMultiDrawElements(GLenum mode, const GLsizei* count, GLenum type, const void* const* indices, GLsizei drawcount);

void GLES20::registerMultiDrawEmulation() {
    REGISTER(glMultiDrawArrays);
    REGISTER(glMultiDrawElements);
}

void glMultiDrawArrays(
    GLenum mode,
    const GLint* first,
    const GLsizei* count,
    GLsizei drawcount
) {
    if (drawcount == 0) return;

    #pragma omp parallel for if (drawcount > DRAWCOUNT_THRESHOLD)
    for (GLsizei i = 0; i < drawcount; ++i) {
        if (count[i] > 0) glDrawArrays(mode, first[i], count[i]);
    }
}

void glMultiDrawElements(
    GLenum mode,
    const GLsizei* count,
    GLenum type,
    const void* const* indices,
    GLsizei drawcount
) {
    if (drawcount == 0) return;
    // if (!Validation::isCurrentFramebufferValid()) return;

    #pragma omp parallel for if (drawcount > DRAWCOUNT_THRESHOLD)
    for (GLsizei i = 0; i < drawcount; ++i) {
        if (count[i] > 0) glDrawElements(mode, count[i], type, indices[i]);
    }
}
