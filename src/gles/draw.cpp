#include "es/ffpe/draw.h"
#include "es/ffp.h"
#include "gles/draw_overrides.h"
#include "gles/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl32.h>

void GLES::registerDrawOverride() {
    REGISTEROV(glDrawArrays);
}

void OV_glDrawArrays(GLenum mode, GLint first, GLsizei count) {
    if (count <= 0) return;
    
    if (debugEnabled) LOGI("OV_glDrawArrays : mode=%u count=%i", mode, count);
    Lists::displayListManager->addCommand<OV_glDrawArrays>(mode, first, count);

    switch (mode) {
        case GL_QUADS:
            FFPE::Rendering::Arrays::Quads::drawQuads(count);
            break;

        default:
            FFPE::Rendering::Arrays::drawArrays(mode, first, count);
    }
}
