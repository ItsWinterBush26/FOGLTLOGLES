#include "ffp/main.h"


void FFP::FFPWrapper::init() {
    FFP::registerImmediateFunctions();
    FFP::registerVertexFunctions();
    FFP::registerMatrixFunctions();
}
    