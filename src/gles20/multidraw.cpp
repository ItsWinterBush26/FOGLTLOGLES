#include "es/validation.h"
#include "gles20/main.h"
#include "main.h"

#include <GLES2/gl2.h>

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

    #pragma omp parallel for if (drawcount > 32)
    for (GLsizei i = 0; i < drawcount; ++i) {
        glDrawArrays(mode, first[i], count[i]);
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
    if (!Validation::isCurrentFramebufferValid()) return;

    #pragma omp parallel for if (drawcount > 32)
    for (GLsizei i = 0; i < drawcount; ++i) {
        glDrawElements(mode, count[i], type, indices[i]);
    }
}
