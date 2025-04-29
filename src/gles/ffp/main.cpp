#include "gles/ffp/main.h"
#include "utils/log.h"

void FFP::FFPWrapper::init() {
    LOGI("GLES 1.0 :: FFP entrypoint!");

    FFP::registerImmediateFunctions();
    FFP::registerVertexFunctions();
    FFP::registerMatrixFunctions();
}
    