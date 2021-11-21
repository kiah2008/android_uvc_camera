//
// Created by zhaokai on 2020/6/23.
//

#ifndef FASTPIPELINE_IMAGE_FORMAT_H
#define FASTPIPELINE_IMAGE_FORMAT_H
namespace cutils {

    enum ImageFormat : int {
        IMAGE_FORMAT_UNKNOWN = 0,
        IMAGE_FORMAT_SRGB = 1,
        IMAGE_FORMAT_SRGBA = 2,
        IMAGE_FORMAT_GRAY8 = 3,
        IMAGE_FORMAT_GRAY16 = 4,
        IMAGE_FORMAT_YCBCR420P = 5,
        IMAGE_FORMAT_YCBCR420P10 = 6,
        IMAGE_FORMAT_SRGB48 = 7,
        IMAGE_FORMAT_SRGBA64 = 8,
        IMAGE_FORMAT_VEC32F1 = 9,
        IMAGE_FORMAT_LAB8 = 10,
        IMAGE_FORMAT_SBGRA = 11,
        IMAGE_FORMAT_NV21,
        IMAGE_FORMAT_PLANAR_UC2,
    };

    /// compliance with AncCommonRotation
    enum ImageOrientation : int {
        IMAGE_ORIENTATION_0 = 0,
        IMAGE_ORIENTATION_90 = 90,
        IMAGE_ORIENTATION_180 = 180,
        IMAGE_ORIENTATION_270 = 270,
    };
//
//    class ImageFormat {
//    public:
//        ImageFormat() {};
//        virtual ~ImageFormat();
//
//        ImageFormat(const ImageFormat& from);
//        ImageFormat(ImageFormat&& from) noexcept
//                : ImageFormat() {
//            *this = ::std::move(from);
//        }
//
//        inline ImageFormat& operator=(const ImageFormat& from) {
//            CopyFrom(from);
//            return *this;
//        }
//        inline ImageFormat& operator=(ImageFormat&& from) noexcept {
//            if (GetArenaNoVirtual() == from.GetArenaNoVirtual()) {
//                if (this != &from) InternalSwap(&from);
//            } else {
//                CopyFrom(from);
//            }
//            return *this;
//        }
//
//        static const ImageFormat& default_instance();
//
//        void Swap(ImageFormat* other);
//        friend void swap(ImageFormat& a, ImageFormat& b) {
//            a.Swap(&b);
//        }
//
//
//    };


}
#endif //FASTPIPELINE_IMAGE_FORMAT_H
