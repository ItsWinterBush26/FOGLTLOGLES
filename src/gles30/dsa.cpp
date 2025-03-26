#include "es/dsa.h"
#include "es/utils.h"
#include "gles30/main.h"
#include "main.h"
#include "utils/log.h"

#include <GLES3/gl3.h>
#include <GLES3/gl32.h>

// Texture
void glTextureParameteri(GLuint texture, GLenum pname, GLint param);
void glTextureParameterf(GLuint texture, GLenum pname, GLfloat param);
void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height);
void glTextureSubImage2D(GLuint texture, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void* pixels);

// Buffer
void glNamedBufferData(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage);
void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data);

// Framebuffer
void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level);

// VAO
void glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride);
void glVertexArrayAttribFormat(GLuint vaobj, GLuint attribindex, GLint size, GLenum type, GLboolean normalized, GLuint relativeoffset);
void glVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex);
void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index);

void GLES30::registerDSAEmulation() {
    REGISTER(glTextureParameteri);
    REGISTER(glTextureParameterf);
    REGISTER(glTextureStorage2D);
    REGISTER(glTextureSubImage2D);
    REGISTER(glNamedBufferData);
    REGISTER(glNamedBufferSubData);
    REGISTER(glNamedFramebufferTexture);
    REGISTER(glVertexArrayVertexBuffer);
    REGISTER(glVertexArrayAttribFormat);
    REGISTER(glVertexArrayAttribBinding);
    REGISTER(glEnableVertexArrayAttrib);

    ESUtils::fakeExtensions.insert("ARB_direct_state_access");
}

void glTextureParameteri(GLuint texture, GLenum pname, GLint param) {
    TextureBindingSaver saver(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    GET_OVFUNC(PFNGLTEXPARAMETERIPROC, glTexParameteri)(GL_TEXTURE_2D, pname, param);
    LOGI("glTextureParameteri emulated: texture=%u pname=0x%x param=%d", texture, pname, param);
}

void glTextureParameterf(GLuint texture, GLenum pname, GLfloat param) {
    TextureBindingSaver saver(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    GET_OVFUNC(PFNGLTEXPARAMETERFPROC, glTexParameterf)(GL_TEXTURE_2D, pname, param);
    LOGI("glTextureParameterf emulated: texture=%u pname=0x%x param=%f", texture, pname, param);
}

void glTextureStorage2D(GLuint texture, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height) {
    TextureBindingSaver saver(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexStorage2D(GL_TEXTURE_2D, levels, internalformat, width, height);
    LOGI("glTextureStorage2D emulated: texture=%u levels=%d format=0x%x width=%d height=%d", texture, levels, internalformat, width, height);
}

void glTextureSubImage2D(
    GLuint texture, GLint level,
    GLint xoffset, GLint yoffset, 
    GLsizei width, GLsizei height,
    GLenum format, GLenum type, 
    const void* pixels
) {
    TextureBindingSaver saver(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture);
    GET_OVFUNC(PFNGLTEXSUBIMAGE2DPROC, glTexSubImage2D)(GL_TEXTURE_2D, level, xoffset, yoffset, width, height, format, type, pixels);
    LOGI("glTextureSubImage2D emulated: texture=%u level=%d", texture, level);
}

void glNamedBufferData(GLuint buffer, GLsizeiptr size, const void* data, GLenum usage) {
    BufferBindingSaver saver(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, usage);
    LOGI("glNamedBufferData emulated: buffer=%u size=%ld usage=0x%x", buffer, size, usage);
}

void glNamedBufferSubData(GLuint buffer, GLintptr offset, GLsizeiptr size, const void* data) {
    BufferBindingSaver saver(GL_ARRAY_BUFFER);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferSubData(GL_ARRAY_BUFFER, offset, size, data);
    LOGI("glNamedBufferSubData emulated: buffer=%u offset=%ld size=%ld", buffer, offset, size);
}

void glNamedFramebufferTexture(GLuint framebuffer, GLenum attachment, GLuint texture, GLint level) {
    FramebufferBindingSaver saver;
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture, level);
    LOGI("glNamedFramebufferTexture emulated: fbo=%u attachment=0x%x texture=%u level=%d", framebuffer, attachment, texture, level);
}

void glVertexArrayVertexBuffer(GLuint vaobj, GLuint bindingindex, GLuint buffer, GLintptr offset, GLsizei stride) {
    VAOBindingSaver saver;
    glBindVertexArray(vaobj);
    glBindVertexBuffer(bindingindex, buffer, offset, stride);
    LOGI("glVertexArrayVertexBuffer emulated: vaobj=%u bindingindex=%u buffer=%u", vaobj, bindingindex, buffer);
}

void glVertexArrayAttribFormat(
    GLuint vaobj,
    GLuint attribindex,
    GLint size, GLenum type, 
    GLboolean normalized, GLuint relativeoffset
) {
    VAOBindingSaver saver;
    glBindVertexArray(vaobj);
    glVertexAttribFormat(attribindex, size, type, normalized, relativeoffset);
    LOGI("glVertexArrayAttribFormat emulated: vaobj=%u attribindex=%u", vaobj, attribindex);
}

void glVertexArrayAttribBinding(GLuint vaobj, GLuint attribindex, GLuint bindingindex) {
    VAOBindingSaver saver;
    glBindVertexArray(vaobj);
    glVertexAttribBinding(attribindex, bindingindex);
    LOGI("glVertexArrayAttribBinding emulated: vaobj=%u attribindex=%u bindingindex=%u", 
         vaobj, attribindex, bindingindex);
}

void glEnableVertexArrayAttrib(GLuint vaobj, GLuint index) {
    VAOBindingSaver saver;
    glBindVertexArray(vaobj);
    glEnableVertexAttribArray(index);
    LOGI("glEnableVertexArrayAttrib emulated: vaobj=%u index=%u", vaobj, index);
}