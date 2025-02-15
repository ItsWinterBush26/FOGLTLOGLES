#include "base/base.h"
#include "utils/extensions.h"

#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>

// Textures
OVERRIDEV(glTexImage2D, GLenum target, GLint l, GLint iform, GLsizei w, GLsizei h, GLint b, GLenum form, GLenum type, const void *p) {
	switch (iform) {
		case 0x8058: // GL_RGBA8 
		case GL_RGBA:
        	iform = GL_RGBA;
        	break;

        case 0x8051: // GL_RGB8
        case GL_RGB:
			iform = GL_RGB;
			break;

		case 0x81A6: // GL_DEPTH_COMPONENT24
        case 0x81A7: // GL_DEPTH_COMPONENT32
            iform = GL_DEPTH_COMPONENT;
            break;

		case 0x8229: // GL_R8
            iform = ESExtensions::isSupported("GL_EXT_texture_rg")
            		? GL_RED_EXT : GL_LUMINANCE; // GL_RED or fallback to GL_LUMINANCE
            break;
        /*
        case 0x8F94: // GL_RGBA32F
            iform = isExtensionSupported("GL_OES_texture_float") ? 0x8814 : GL_RGBA; // GL_RGBA16F if supported, else GL_RGBA
            break;
        case 0x8814: // GL_RGBA16F
            iform = isExtensionSupported("GL_OES_texture_half_float") ? 0x8814 : GL_RGBA; // GL_RGBA16F if supported, else GL_RGBA
            break;
        case 0x8D9F: // GL_SRGB8_ALPHA8
            iform = isExtensionSupported("GL_EXT_sRGB") ? 0x8C42 : GL_RGBA; // GL_SRGB_ALPHA if supported, else GL_RGBA
            break;
        case 0x8C41: // GL_SRGB8
            iform = isExtensionSupported("GL_EXT_sRGB") ? 0x8C41 : GL_RGB; // GL_SRGB if supported, else GL_RGB
            break;
        */

        default:
        	iform = GL_RGBA;
    }

    original_glTexImage2D(target, l, iform, w, h, b, form, type, p);
}
