/*
** Copyright (c) 2013-2017 The Khronos Group Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and/or associated documentation files (the
** "Materials"), to deal in the Materials without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Materials, and to
** permit persons to whom the Materials are furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be included
** in all copies or substantial portions of the Materials.
**
** THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
*/
/*
** This header is generated from the Khronos OpenGL / OpenGL ES XML
** API Registry. The current version of the Registry, generator scripts
** used to make the header, and the header can be found at
**   http://www.khronos.org/registry/egl
**
** Khronos $Git commit SHA1: bae3518c48 $ on $Git commit date: 2018-05-17 10:56:57 -0700 $
*/

#pragma once

#include "main.h"

#include <EGL/eglplatform.h>

/*
typedef unsigned int EGLBoolean;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;

PUBLIC_API EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, EGLContext share_context, const EGLint *attrib_list);
PUBLIC_API EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx);
PUBLIC_API EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx);
*/

void eglInit();

FunctionPtr OV_eglGetProcAddress(const char*);