#pragma once

typedef double GLdouble;

#define EXTERN extern "C"
#define PUBLIC __attribute__((visibility("default")))
#define DEFINE(type, func, ...) PUBLIC EXTERN type func(__VA_ARGS__)

#define REDIRECT(type, func, target, params, args) \
using PFN_##target = decltype(&::target);          \
static PFN_##target original_##target = ::target;  \
PUBLIC EXTERN type func params {                   \
	return original_##target args;                 \
}

#define REDIRECTV(func, target, params, args)     \
using PFN_##target = decltype(&::target);         \
static PFN_##target original_##target = ::target; \
PUBLIC EXTERN void func params {                  \
	original_##target args;                       \
}

#define OVERRIDE(type, func, ...)           \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
DEFINE(type, func, __VA_ARGS__)

#define OVERRIDEV(func, ...)                \
using PFN_##func = decltype(&::func);       \
static PFN_##func original_##func = ::func; \
DEFINE(void, func, __VA_ARGS__)



