#pragma once

typedef double GLdouble;

#define EXTERN extern "C"
#define PUBLIC __attribute__((visibility("default")))
#define DEFINE(type, func, ...) PUBLIC EXTERN type func(__VA_ARGS__)

#define OVERRIDE(type, func, ...)               \
    using PFN_##func = decltype(&::func);       \
    static PFN_##func original_##func = ::func; \
    DEFINE(type, func, __VA_ARGS__)

