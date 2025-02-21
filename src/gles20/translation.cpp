#include "gles20/translation.h"
#include "es/utils.h"
#include "gl/types.h"
#include "main.h"
#include "utils/conversions.h"
#include "utils/log.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

void glClearDepth(double d);
void OV_glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void *pixels);

void GLES20::registerTranslatedFunctions() {
    LOGI("Hi from translation.cpp!");

    REGISTER(glClearDepth);
    REGISTEROV(glTexImage2D);
}

void glClearDepth(double d) {
    glClearDepthf(static_cast<float>(d));
}

void OV_glTexImage2D(
    GLenum target,
    GLint level,
    GLint internalformat,
    GLsizei width, GLsizei height,
    GLint border, GLenum format,
    GLenum type, const void *pixels
) {
    bool conversionNeeded = false;
    bool swapChannels = false; // For BGRA/BGR conversion.
    GLenum destInternalFormat;
    GLenum destFormat;
    GLenum destType;
    int channels = 0; // Number of channels in the texture.

    // Map desktop internal formats to GLES-compatible ones.
    // For each case, if the appropriate extension is supported,
    // we can pass through the original type; otherwise, we convert.
    switch (internalformat) {
        case GL_RGBA32F_EXT:
            channels = 4;
            destInternalFormat = GL_RGBA;
            destFormat = GL_RGBA;
            if (ESUtils::isSupported("GL_OES_texture_float") && type == GL_FLOAT) {
                destType = GL_FLOAT;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_RGBA16F_EXT:
            channels = 4;
            destInternalFormat = GL_RGBA;
            destFormat = GL_RGBA;
            if (ESUtils::isSupported("GL_OES_texture_half_float") && type == GL_HALF_FLOAT) {
                destType = GL_HALF_FLOAT_OES;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_RGB32F_EXT:
            channels = 3;
            destInternalFormat = GL_RGB;
            destFormat = GL_RGB;
            if (ESUtils::isSupported("GL_OES_texture_float") && type == GL_FLOAT) {
                destType = GL_FLOAT;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_RGB16F_EXT:
            channels = 3;
            destInternalFormat = GL_RGB;
            destFormat = GL_RGB;
            if (ESUtils::isSupported("GL_OES_texture_half_float") && type == GL_HALF_FLOAT) {
                destType = GL_HALF_FLOAT_OES;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_R32F_EXT:
            channels = 1;
            destInternalFormat = GL_LUMINANCE;
            destFormat = GL_LUMINANCE;
            if (ESUtils::isSupported("GL_OES_texture_float") && type == GL_FLOAT) {
                destType = GL_FLOAT;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_R16F_EXT:
            channels = 1;
            destInternalFormat = GL_LUMINANCE;
            destFormat = GL_LUMINANCE;
            if (ESUtils::isSupported("GL_OES_texture_half_float") && type == GL_HALF_FLOAT) {
                destType = GL_HALF_FLOAT_OES;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_RG32F_EXT:
            channels = 2;
            destInternalFormat = GL_LUMINANCE_ALPHA;
            destFormat = GL_LUMINANCE_ALPHA;
            if (ESUtils::isSupported("GL_OES_texture_float") && type == GL_FLOAT) {
                destType = GL_FLOAT;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        case GL_RG16F_EXT:
            channels = 2;
            destInternalFormat = GL_LUMINANCE_ALPHA;
            destFormat = GL_LUMINANCE_ALPHA;
            if (ESUtils::isSupported("GL_OES_texture_half_float") && type == GL_HALF_FLOAT) {
                destType = GL_HALF_FLOAT_OES;
                conversionNeeded = false;
            } else {
                destType = GL_UNSIGNED_BYTE;
                conversionNeeded = true;
            }
            break;
        // For depth textures, if GL_OES_depth_texture is supported and the type is acceptable, pass through.
        case GL_DEPTH_COMPONENT32_OES:
        case GL_DEPTH_COMPONENT24_OES:
            channels = 1;
            if (ESUtils::isSupported("GL_OES_depth_texture") &&
                (type == GL_UNSIGNED_SHORT || type == GL_UNSIGNED_INT)) {
                destInternalFormat = GL_DEPTH_COMPONENT;
                destFormat = GL_DEPTH_COMPONENT;
                destType = type;
                conversionNeeded = false;
            } else {
                destInternalFormat = GL_DEPTH_COMPONENT16;
                destFormat = GL_DEPTH_COMPONENT;
                destType = GL_UNSIGNED_SHORT;
                conversionNeeded = true;
            }
            break;
        default:
            // For unsized or already GLES-compatible formats, pass through.
            destInternalFormat = internalformat;
            destFormat = format;
            destType = type;
            if (format == GL_RGBA || format == GL_BGRA_EXT)
                channels = 4;
            else if (format == GL_RGB || format == GL_BGR)
                channels = 3;
            else if (format == GL_LUMINANCE_ALPHA)
                channels = 2;
            else
                channels = 1;
            conversionNeeded = false;
            break;
    }

    // Check for BGRA/BGR source formats.
    // If GL_EXT_texture_format_BGRA8888 is supported, BGRA can be passed through directly.
    if ((format == GL_BGRA_EXT) && !ESUtils::isSupported("GL_EXT_texture_format_BGRA8888")) {
        swapChannels = true;
        conversionNeeded = true;
        destFormat = GL_RGBA;
        if (channels == 0) { channels = 4; destInternalFormat = GL_RGBA; }
    } else if (format == GL_BGR) {
        swapChannels = true;
        conversionNeeded = true;
        destFormat = GL_RGB;
        if (channels == 0) { channels = 3; destInternalFormat = GL_RGB; }
    }

    // If no conversion is needed, call glTexImage2D directly.
    if (!conversionNeeded) {
        glTexImage2D(target, level, destInternalFormat, width, height, 0, destFormat, destType, pixels);
        return;
    }

    // Conversion is required.
    size_t pixelCount = static_cast<size_t>(width) * height;
    // CASE: Color texture conversion to GL_UNSIGNED_BYTE.
    if (destType == GL_UNSIGNED_BYTE) {
        std::vector<ubyte> buffer(pixelCount * channels);
        if (type == GL_FLOAT) {
            const float* src = static_cast<const float*>(pixels);
            if (swapChannels && channels >= 3) {
                for (size_t i = 0; i < pixelCount; ++i) {
                    if (channels == 4) {
                        // Swap BGRA → RGBA.
                        float b = src[i * 4 + 0];
                        float g = src[i * 4 + 1];
                        float r = src[i * 4 + 2];
                        float a = src[i * 4 + 3];
                        buffer[i * 4 + 0] = float_to_ubyte(r);
                        buffer[i * 4 + 1] = float_to_ubyte(g);
                        buffer[i * 4 + 2] = float_to_ubyte(b);
                        buffer[i * 4 + 3] = float_to_ubyte(a);
                    } else if (channels == 3) {
                        float b = src[i * 3 + 0];
                        float g = src[i * 3 + 1];
                        float r = src[i * 3 + 2];
                        buffer[i * 3 + 0] = float_to_ubyte(r);
                        buffer[i * 3 + 1] = float_to_ubyte(g);
                        buffer[i * 3 + 2] = float_to_ubyte(b);
                    }
                }
            } else {
                for (size_t i = 0; i < buffer.size(); ++i)
                    buffer[i] = float_to_ubyte(src[i]);
            }
        }
        else if (type == GL_HALF_FLOAT) {
            const uint16_t* src = static_cast<const uint16_t*>(pixels);
            if (swapChannels && channels >= 3) {
                for (size_t i = 0; i < pixelCount; ++i) {
                    if (channels == 4) {
                        float b = half_to_float(src[i * 4 + 0]);
                        float g = half_to_float(src[i * 4 + 1]);
                        float r = half_to_float(src[i * 4 + 2]);
                        float a = half_to_float(src[i * 4 + 3]);
                        buffer[i * 4 + 0] = float_to_ubyte(r);
                        buffer[i * 4 + 1] = float_to_ubyte(g);
                        buffer[i * 4 + 2] = float_to_ubyte(b);
                        buffer[i * 4 + 3] = float_to_ubyte(a);
                    } else if (channels == 3) {
                        float b = half_to_float(src[i * 3 + 0]);
                        float g = half_to_float(src[i * 3 + 1]);
                        float r = half_to_float(src[i * 3 + 2]);
                        buffer[i * 3 + 0] = float_to_ubyte(r);
                        buffer[i * 3 + 1] = float_to_ubyte(g);
                        buffer[i * 3 + 2] = float_to_ubyte(b);
                    }
                }
            } else {
                for (size_t i = 0; i < buffer.size(); ++i)
                    buffer[i] = float_to_ubyte(half_to_float(src[i]));
            }
        }
        // Also handle the case where source is already unsigned byte but requires channel swapping.
        else if (type == GL_UNSIGNED_BYTE && swapChannels && channels >= 3) {
            const unsigned char* src = static_cast<const unsigned char*>(pixels);
            if (channels == 4) {
                for (size_t i = 0; i < pixelCount; ++i) {
                    unsigned char b = src[i * 4 + 0];
                    unsigned char g = src[i * 4 + 1];
                    unsigned char r = src[i * 4 + 2];
                    unsigned char a = src[i * 4 + 3];
                    buffer[i * 4 + 0] = r;
                    buffer[i * 4 + 1] = g;
                    buffer[i * 4 + 2] = b;
                    buffer[i * 4 + 3] = a;
                }
            } else if (channels == 3) {
                for (size_t i = 0; i < pixelCount; ++i) {
                    unsigned char b = src[i * 3 + 0];
                    unsigned char g = src[i * 3 + 1];
                    unsigned char r = src[i * 3 + 2];
                    buffer[i * 3 + 0] = r;
                    buffer[i * 3 + 1] = g;
                    buffer[i * 3 + 2] = b;
                }
            }
        }
        glTexImage2D(target, level, destInternalFormat, width, height, 0, destFormat, destType, buffer.data());
    }
    // CASE: Depth texture conversion to GL_UNSIGNED_SHORT.
    else if (destType == GL_UNSIGNED_SHORT) {
        std::vector<ushort> buffer(pixelCount);
        if (type == GL_FLOAT) {
            const float* src = static_cast<const float*>(pixels);
            for (size_t i = 0; i < pixelCount; ++i) {
                float val = std::clamp(src[i], 0.0f, 1.0f);
                buffer[i] = static_cast<unsigned short>(val * 65535.0f);
            }
        }
        else if (type == GL_UNSIGNED_INT) {
            const unsigned int* src = static_cast<const unsigned int*>(pixels);
            for (size_t i = 0; i < pixelCount; ++i) {
                float val = std::clamp(static_cast<float>(src[i]) / 4294967295.0f, 0.0f, 1.0f);
                buffer[i] = static_cast<unsigned short>(val * 65535.0f);
            }
        }
        glTexImage2D(target, level, destInternalFormat, width, height, 0, destFormat, destType, buffer.data());
    }
    else {
        // Fallback: if no conversion path is explicitly implemented.
        glTexImage2D(target, level, destInternalFormat, width, height, 0, destFormat, destType, pixels);
    }
}