#include "es/ffpe/immediate.h"
#include "gles/ffp/main.h"
#include "main.h"

#include <GLES3/gl32.h>

void glBegin(GLenum mode);
void glEnd();

void FFP::registerImmediateFunctions() {
    REGISTERREDIR("glBegin", FFPE::Rendering::ImmediateMode::begin);
    REGISTERREDIR("glEnd", FFPE::Rendering::ImmediateMode::end);

    FFPE::Rendering::ImmediateMode::init();
}

