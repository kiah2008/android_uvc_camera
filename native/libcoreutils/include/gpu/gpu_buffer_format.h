//
// Created by zhaokai on 2020/7/14.
//

#ifndef HUMANVIDEOEFFECTS_GPU_BUFFER_FORMAT_H
#define HUMANVIDEOEFFECTS_GPU_BUFFER_FORMAT_H

#include "gl_base.h"
#include <base/image_format.h>

namespace cutils {
    // The behavior of multi-char constants is implementation-defined, so out of an
// excess of caution we define them in this portable way.
#define MEDIAPIPE_FOURCC(a, b, c, d) \
  (((a) << 24) + ((b) << 16) + ((c) << 8) + (d))

    enum class GpuBufferFormat : uint32_t {
        kUnknown = 0,
        kBGRA32 = MEDIAPIPE_FOURCC('B', 'G', 'R', 'A'),
        kGrayFloat32 = MEDIAPIPE_FOURCC('L', '0', '0', 'f'),
        kGrayHalf16 = MEDIAPIPE_FOURCC('L', '0', '0', 'h'),
        kOneComponent8 = MEDIAPIPE_FOURCC('L', '0', '0', '8'),
        kTwoComponentHalf16 = MEDIAPIPE_FOURCC('2', 'C', '0', 'h'),
        kBiPlanar420YpCbCr8VideoRange = MEDIAPIPE_FOURCC('4', '2', '0', 'v'),
        kBiPlanar420YpCbCr8FullRange = MEDIAPIPE_FOURCC('4', '2', '0', 'f'),
        kRGB24 = 0x00000018,  // Note: prefer BGRA32 whenever possible.
        kRGBAHalf64 = MEDIAPIPE_FOURCC('R', 'G', 'h', 'A'),
        kRGBAFloat128 = MEDIAPIPE_FOURCC('R', 'G', 'f', 'A'),
        kNV21_OES = MEDIAPIPE_FOURCC('4', '2', '0', 'p'),
    };

// TODO: make this more generally applicable.
    enum class GlVersion {
        kGL = 1,
        kGLES2 = 2,
        kGLES3 = 3,
    };

    struct GlTextureInfo {
        GLint gl_internal_format;
        GLenum gl_format;
        GLenum gl_type;
        // For multiplane buffers, this represents how many times smaller than
        // the nominal image size a plane is.
        int downscale;
    };

    const GlTextureInfo &GlTextureInfoForGpuBufferFormat(GpuBufferFormat format,
                                                         int plane);

    const GlTextureInfo &GlTextureInfoForGpuBufferFormat(GpuBufferFormat format,
                                                         int plane,
                                                         GlVersion gl_version);

    ImageFormat ImageFormatForGpuBufferFormat(GpuBufferFormat format);

    GpuBufferFormat GpuBufferFormatForImageFormat(ImageFormat format);
}
#endif //HUMANVIDEOEFFECTS_GPU_BUFFER_FORMAT_H
