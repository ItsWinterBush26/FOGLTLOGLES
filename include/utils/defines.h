#pragma once

#include "api.h"

#define WRAP(type, func, params, args)      \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
PUBLIC_API type func params {               \
	return original_##func args;            \
}

#define WRAPV(func, params, args)           \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
PUBLIC_API void func params {               \
	original_##func args;                   \
}

#define REDIRECT(type, name, target, params, args)  \
PUBLIC_API type name params {                       \
	return ##target args;                           \
}

#define REDIRECTV(name, target, params, args)  \
PUBLIC_API void name params {                  \
	##target args;                             \
}

#define OVERRIDE(type, func, ...)           \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
DEFINE_API(type, func, __VA_ARGS__)

#define OVERRIDEV(func, ...)                \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
DEFINE_API(void, func, __VA_ARGS__)



