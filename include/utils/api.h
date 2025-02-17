#pragma once

#define EXTERN extern "C"
#define PUBLIC __attribute__((visibility("default")))
#define PUBLIC_API PUBLIC EXTERN
#define DEFINE_API(type, func, ...) PUBLIC_API type func __VA_ARGS__