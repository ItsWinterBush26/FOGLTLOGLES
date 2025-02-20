#pragma once

#include "api.h"
#include <dlfcn.h>

#ifndef WRAP
#define WRAP(type, func, params, args)                                  \
using PFN_##func = decltype(&func);                                       \
static PFN_##func original_##func = nullptr;                             \
PUBLIC_API type func params {                                             \
    if (!original_##func) {                                               \
        original_##func = (PFN_##func)dlsym(RTLD_NEXT, #func);            \
    }                                                                     \
    return original_##func args;                                          \
}
#endif

#ifndef WRAPV
#define WRAPV(func, params, args)                                         \
using PFN_##func = decltype(&func);                                       \
static PFN_##func original_##func = nullptr;                             \
PUBLIC_API void func params {                                             \
    if (!original_##func) {                                               \
        original_##func = (PFN_##func)dlsym(RTLD_NEXT, #func);            \
    }                                                                     \
    original_##func args;                                                 \
}
#endif

#ifndef REDIRECT
#define REDIRECT(type, name, target, params, args)  \
PUBLIC_API type name params {                       \
    return target args;                             \
}
#endif

#ifndef REDIRECTV
#define REDIRECTV(name, target, params, args)  \
PUBLIC_API void name params {                  \
    target args;                             \
}
#endif

#ifndef OVERRIDE
#define OVERRIDE(type, func, ...)                                       \
using PFN_##func = decltype(&func);                                     \
static PFN_##func original_##func = nullptr;                           \
DEFINE_API(type, func, __VA_ARGS__)
#endif

#ifndef OVERRIDEV
#define OVERRIDEV(func, ...)                                            \
using PFN_##func = decltype(&func);                                     \
static PFN_##func original_##func = nullptr;                           \
DEFINE_API(void, func, __VA_ARGS__)
#endif
