#pragma once

#define EXTERN extern "C"
#define PUBLIC __attribute__((visibility("default")))
#define DEFINE(type, func, ...) PUBLIC EXTERN type func __VA_ARGS__

#define WRAP(type, func, params, args)      \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
PUBLIC EXTERN type func params {            \
	return original_##func args;            \
}

#define WRAPV(func, params, args)           \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
PUBLIC EXTERN void func params {            \
	original_##func args;                   \
}

#define REDIRECT(type, name, target, params, args)  \
PUBLIC EXTERN type name params {                    \
	return ##target args;                           \
}

#define REDIRECTV(name, target, params, args)  \
PUBLIC EXTERN void name params {               \
	##target args;                             \
}

#define OVERRIDE(type, func, ...)           \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
DEFINE(type, func, __VA_ARGS__)

#define OVERRIDEV(func, ...)                \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
DEFINE(void, func, __VA_ARGS__)



