#include "gles32/main.h"
#include "utils/log.h"

void GLES32::GLES32Wrapper::init() {
    LOGI("GLES3.2 entrypoint!");

    GLES32::registerBaseVertexFunction();
}