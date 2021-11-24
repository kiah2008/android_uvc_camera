//
// Created by zhaokai on 2020/7/30.
//

#ifndef HUMANVIDEOEFFECTS_GPU_DEBUG_HELPER_H
#define HUMANVIDEOEFFECTS_GPU_DEBUG_HELPER_H

#include <GLES2/gl2.h>
#include <port/clog.h>


namespace cutils {
    bool _FSPCheckForGlErrors();

    void
    ReadTexture(const GLuint texture, uint32_t width, uint32_t height, void *output, size_t size,
                GLenum format, bool oes = false);

}

#define CUTILS_CHECK_FOR_GL_ERROR()  \
        do{\
            if(cutils::_FSPCheckForGlErrors()) CLOGE("GLError Check %s:%d", __FUNCTION__, __LINE__); \
        } while(0)

#endif //HUMANVIDEOEFFECTS_GPU_DEBUG_HELPER_H
