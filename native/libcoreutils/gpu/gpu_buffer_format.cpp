//
// Created by zhaokai on 2020/7/20.
//

#include <map>
#include <vector>
#include <gpu/gpu_buffer_format.h>
#include <gpu/gl_base.h>
#include <port/clog.h>

namespace cutils {

#ifndef GL_RGBA16F
#define GL_RGBA16F 34842
#endif  // GL_RGBA16F

#ifndef GL_HALF_FLOAT
#define GL_HALF_FLOAT 0x140B
#endif  // GL_HALF_FLOAT

#ifdef GL_ES_VERSION_2_0

    static void AdaptGlTextureInfoForGLES2(GlTextureInfo *info) {
        switch (info->gl_internal_format) {
            case GL_R16F:
            case GL_R32F:
                // Should this be GL_RED_EXT instead?
                info->gl_internal_format = info->gl_format = GL_LUMINANCE;
                return;
            case GL_RG16F:
                // Should this be GL_RG_EXT instead?
                info->gl_internal_format = info->gl_format = GL_LUMINANCE_ALPHA;
                return;
            case GL_R8:
                info->gl_internal_format = info->gl_format = GL_RED_EXT;
                return;
            case GL_RG8:
                info->gl_internal_format = info->gl_format = GL_RG_EXT;
                return;
            default:
                return;
        }
    }

#endif  // GL_ES_VERSION_2_0

    const GlTextureInfo &GlTextureInfoForGpuBufferFormat(GpuBufferFormat format,
                                                         int plane) {
        constexpr GlVersion default_version = GlVersion::kGLES3;
        return GlTextureInfoForGpuBufferFormat(format, plane, default_version);
    }

    const GlTextureInfo &GlTextureInfoForGpuBufferFormat(GpuBufferFormat format,
                                                         int plane,
                                                         GlVersion gl_version) {
        static const
        std::map<GpuBufferFormat, std::vector<GlTextureInfo >>
                gles3_format_info{
                {GpuBufferFormat::kBGRA32,
                        {
                                // internal_format, format, type, downscale
                                {GL_RGBA,      GL_RGBA,      GL_UNSIGNED_BYTE, 1},
                        }},
                {GpuBufferFormat::kOneComponent8,
                        {
                                // This should be GL_RED, but it would change the output for existing
                                // shaders. It would not be a good representation of a grayscale texture,
                                // unless we use texture swizzling. We could add swizzle parameters (e.g.
                                // GL_TEXTURE_SWIZZLE_R) in GLES 3 and desktop GL, and use GL_LUMINANCE
                                // in GLES 2. Or we could just punt and make it a red texture.
                                // {GL_R8, GL_RED, GL_UNSIGNED_BYTE, 1},
                                {GL_LUMINANCE, GL_LUMINANCE, GL_UNSIGNED_BYTE, 1},
                        }},

                {GpuBufferFormat::kTwoComponentHalf16,
                        {
                                // TODO: use GL_HALF_FLOAT_OES on GLES2?
                                {GL_RG16F,     GL_RG,        GL_HALF_FLOAT,    1},
                        }},
                {GpuBufferFormat::kGrayHalf16,
                        {
                                {GL_R16F,      GL_RED,       GL_HALF_FLOAT,    1},
                        }},
                {GpuBufferFormat::kGrayFloat32,
                        {
                                {GL_R32F,      GL_RED,       GL_FLOAT,         1},
                        }},
                {GpuBufferFormat::kRGB24,
                        {
                                {GL_RGB,       GL_RGB,       GL_UNSIGNED_BYTE, 1},
                        }},
                {GpuBufferFormat::kRGBAHalf64,
                        {
                                {GL_RGBA16F,   GL_RGBA,      GL_HALF_FLOAT,    1},
                        }},
                {GpuBufferFormat::kRGBAFloat128,
                        {
                                {GL_RGBA,      GL_RGBA,      GL_FLOAT,         1},
                        }},
        };

        static const auto *gles2_format_info = ([] {
            auto formats =
                    new std::map<GpuBufferFormat, std::vector<GlTextureInfo>>(
                            gles3_format_info);
#ifdef GL_ES_VERSION_2_0
            for (auto &format_planes : *formats) {
                for (auto &info : format_planes.second) {
                    AdaptGlTextureInfoForGLES2(&info);
                }
            }
#endif  // GL_ES_VERSION_2_0
            return formats;
        })();

        auto *format_info = &gles3_format_info;
        switch (gl_version) {
            case GlVersion::kGLES2:
                format_info = gles2_format_info;
                break;
            case GlVersion::kGLES3:
            case GlVersion::kGL:
                break;
        }

        auto iter = format_info->find(format);
        CUTILS_CHECK(iter != format_info->end());
        const auto &planes = iter->second;
        CUTILS_CHECK_EQ(planes.size(), 1);
        CUTILS_CHECK_GE(plane, 0);
        CUTILS_CHECK_LT(plane, planes.size());
        return planes[plane];
    }

    ImageFormat ImageFormatForGpuBufferFormat(GpuBufferFormat format) {
        switch (format) {
            case GpuBufferFormat::kBGRA32:
                // TODO: verify we are handling order of channels correctly.
                return IMAGE_FORMAT_SRGBA;
            case GpuBufferFormat::kGrayFloat32:
                return IMAGE_FORMAT_VEC32F1;
            case GpuBufferFormat::kOneComponent8:
                return IMAGE_FORMAT_GRAY8;
            case GpuBufferFormat::kBiPlanar420YpCbCr8VideoRange:
            case GpuBufferFormat::kBiPlanar420YpCbCr8FullRange:
            case GpuBufferFormat::kNV21_OES:
                // TODO: should either of these be YCBCR420P10?
                return IMAGE_FORMAT_YCBCR420P;
            case GpuBufferFormat::kRGB24:
                return IMAGE_FORMAT_SRGB;
            case GpuBufferFormat::kGrayHalf16:
            case GpuBufferFormat::kTwoComponentHalf16:
            case GpuBufferFormat::kRGBAHalf64:
            case GpuBufferFormat::kRGBAFloat128:
            case GpuBufferFormat::kUnknown:
                return IMAGE_FORMAT_UNKNOWN;
        }
    }

    GpuBufferFormat GpuBufferFormatForImageFormat(ImageFormat format) {
        switch (format) {
            case IMAGE_FORMAT_SRGB:
                return GpuBufferFormat::kRGB24;
            case IMAGE_FORMAT_SRGBA:
                // TODO: verify we are handling order of channels correctly.
                return GpuBufferFormat::kBGRA32;
            case IMAGE_FORMAT_VEC32F1:
                return GpuBufferFormat::kGrayFloat32;
            case IMAGE_FORMAT_GRAY8:
                return GpuBufferFormat::kOneComponent8;
            case IMAGE_FORMAT_YCBCR420P:
                // TODO: or video range?
                return GpuBufferFormat::kBiPlanar420YpCbCr8FullRange;
            case IMAGE_FORMAT_UNKNOWN:
            default:
                return GpuBufferFormat::kUnknown;
        }
    }
}