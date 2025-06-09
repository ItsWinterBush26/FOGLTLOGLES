#include "gles/ffp/main.h"
#include "es/ffpe/shadergen/common.h"
#include "utils/log.h"

void FFP::FFPWrapper::init() {
    LOGI("GLES 1.0 :: FFP entrypoint!");

    FFP::registerStubFunctions();
    FFP::registerImmediateFunctions();
    FFP::registerColorFunctions();
    FFP::registerTexCoordFunctions();
    FFP::registerVertexFunctions();
    FFP::registerMatrixFunctions();
    FFP::registerListsFunctions();
    FFP::registerAlphaTestFunctions();
    FFP::registerTextureFunctions();
    FFP::registerArrayFunctions();
}
