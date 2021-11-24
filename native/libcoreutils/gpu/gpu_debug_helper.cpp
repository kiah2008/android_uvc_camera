//
// Created by zhaokai on 2020/7/30.
//

#include <assert.h>
#include <cstdlib>
#include <gpu/gpu_debug_helper.h>
#include <gpu/gl_base.h>
#include <port/clog.h>

namespace cutils {

    bool _FSPCheckForGlErrors() {
        if (eglGetCurrentContext() == EGL_NO_CONTEXT) {
            CLOGE("CheckForGlErrors: has no context");
            return true;
        }
        GLenum error;
        bool had_error = false;
        while ((error = glGetError()) != GL_NO_ERROR) {
            had_error = true;
            switch (error) {
                case GL_INVALID_ENUM:
                    CLOG("CheckForGlErrors: GL_INVALID_ENUM");
                    break;
                case GL_INVALID_VALUE:
                    CLOG("CheckForGlErrors: GL_INVALID_VALUE");
                    break;
                case GL_INVALID_OPERATION:
                    CLOG("CheckForGlErrors: GL_INVALID_OPERATION");
                    break;
                case GL_INVALID_FRAMEBUFFER_OPERATION:
                    CLOG("CheckForGlErrors: GL_INVALID_FRAMEBUFFER_OPERATION");
                    break;
                case GL_OUT_OF_MEMORY:
                    CLOG("CheckForGlErrors: GL_OUT_OF_MEMORY");
                    break;
                default:
                    CLOG("CheckForGlErrors: UNKNOWN ERROR");
                    break;
            }
        }
        return had_error;
    }

    void
    ReadTexture(const GLuint texture, uint32_t width, uint32_t height, void *output, size_t size,
                GLenum format, bool oes) {
        uint32_t bpp = 0;
        switch (format) {
            case GL_RGB:
                bpp = 3;
                break;
            case GL_RGBA:
                bpp = 4;
                break;
            default:
                assert(0);
                break;
        }

        CUTILS_CHECK_GE(size, width * height * bpp);

        GLint current_fbo;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &current_fbo);
        if(current_fbo == 0) {
            CLOGE("no valid frame buffer!");
            return;
        }

        GLint color_attachment_name;
        glGetFramebufferAttachmentParameteriv(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                              GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME,
                                              &color_attachment_name);
        if (color_attachment_name != texture) {
            // Set the data from GLTexture object.
            glViewport(0, 0, width, height);
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                                   oes ? GL_TEXTURE_EXTERNAL_OES : GL_TEXTURE_2D, texture, 0);
            glReadPixels(0, 0, width, height, format,
                         GL_UNSIGNED_BYTE, output);
        } else {
            glReadPixels(0, 0, width, height, format,
                         GL_UNSIGNED_BYTE, output);
        }

        CUTILS_CHECK_FOR_GL_ERROR();
    }
}